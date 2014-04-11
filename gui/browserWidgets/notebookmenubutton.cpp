/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "notebookmenubutton.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include <QMenu>
#include <QAction>
#include "global.h"


extern Global global;

NotebookMenuButton::NotebookMenuButton(QWidget *parent) :
    QPushButton(parent)
{
    setMenu(&rootMenu);
    QIcon icon(":notebook_small.png");
    setIcon(icon);
    currentNotebookName = "";
    loadData();
    QFont f = rootMenu.font();
    f.setBold(false);
    f.setPointSize(8);
    rootMenu.setFont(f);
    currentAction = 0;
}


void NotebookMenuButton::setCurrentNotebook(int lid, Note note) {
    if (actions.size() == 0)
        loadData();

    if (lid <=0)
        return;

    blockSignals(true);
    currentNoteLid = lid;
    NotebookTable notebookTable(global.db);
    Notebook notebook;
    notebookTable.get(notebook, note.notebookGuid);
    if (currentAction < actions.size())
        actions[currentAction]->setChecked(false);
    this->setText(notebook.name);
    for (int i=0; i<actions.size(); i++) {
        actions[i]->setChecked(false);
        QString notebookname = "";
        if (notebook.name.isSet())
            notebookname = notebook.name;
        if (actions[i]->text().toLower().trimmed() == notebookname.toLower().trimmed()) {
            currentAction = i;
            actions[currentAction]->setChecked(true);
        }
    }
    notebookLid = notebookTable.getLid(note.notebookGuid);
    notebookName = notebook.name;
    blockSignals(false);
}


// Read in all of the data and build the menu.
void NotebookMenuButton::loadData() {
    rootMenu.clear();
    NotebookTable notebookTable(global.db);

    QList<qint32> lids;
    notebookTable.getAllOrderByName(lids);

    if (notebookTable.findByName(currentNotebookName) <= 0)
        currentNotebookName = "";

    for (qint32 i=0; i<lids.size(); i++) {
        Notebook book;
        if (notebookTable.get(book, lids[i])) {

            QAction *action = new QAction(this);
            actions.append(action);
            action->setText(book.name);
            action->setCheckable(true);
            connect(action, SIGNAL(triggered()), this, SLOT(notebookSelected()));
            QFont f = action->font();
            f.setPixelSize(10);
            action->setFont(f);
            QMenu *currentMenu = findStack(book);

            addNotebookMenuItem(currentMenu, action);

            if (currentNotebookName == "" && book.defaultNotebook.isSet() &&
                    book.defaultNotebook) {
                currentNotebookName = book.name;
                setText(currentNotebookName);
                currentAction = actions.size()-1;
            }
            QString bookname = "";
            if (book.name.isSet())
                bookname = book.name;
            if (bookname == currentNotebookName) {
                action->setChecked(true);
            }
        }
    }
}


// Add a new stack to the menu.  The action item is a hidden action
// item with the menu's text.
void NotebookMenuButton::addStackMenuItem(QMenu *newMenu) {
    QList<QAction*> items = rootMenu.actions();
    for (int i=0; i<items.size(); i++) {
        if ( newMenu->title().toLower() == items[i]->text().toLower() ) {
            rootMenu.insertMenu(items[i], newMenu);
            return;
        }
    }
}



// Add a new notebook menu item to the specified menu.  This will
// be the root menu for anything without a stack, otherwise it
// will be the stack itself.
void NotebookMenuButton::addNotebookMenuItem(QMenu *menu, QAction *action) {
    QList<QAction*> items = menu->actions();
    for (int i=0; i<items.size(); i++) {
        if ( action->text().toLower() == items[i]->text().toLower() ) {
            menu->insertAction(items[i], action);
            return;
        }
    }
    menu->addAction(action);
}



// Search through the list of known stack menu items & find the menu for
// this notebook's stack.  If one doesn't exist we add it.
QMenu* NotebookMenuButton::findStack(Notebook n) {
    QString stack = "";
    if (n.stack.isSet())
        stack = n.stack;
    stack = stack.trimmed();
    if (stack == "")
        return &rootMenu;

    for (int i=0; i<stackMenus.size(); i++) {
        if (stackMenus.at(i)->title().toLower() == stack.toLower())
            return stackMenus.at(i);
    }


    // Create a new stack.  We add a dummy action item to the
    // menu so we know where to add the menu later.  This
    // keeps things in sorted order
    QMenu *newMenu = new QMenu(this);
    newMenu->setTitle(stack);
    QFont f = newMenu->font();
    f.setPointSize(10);
    f.setBold(false);
    newMenu->setFont(f);
    stackMenus.append(newMenu);
    QAction *placeHolder = new QAction(this);
    placeHolder->setVisible(false);
    placeHolder->setText(stack);
    addNotebookMenuItem(&rootMenu, placeHolder);
    addStackMenuItem(newMenu);
    return newMenu;
}


void NotebookMenuButton::notebookSelected() {
    blockSignals(true);
    if (!actions[currentAction]->isChecked())
        actions[currentAction]->setChecked(true);
    else {
        actions[currentAction]->setChecked(false);
        for (int i=0; i<actions.size(); i++) {
            if (actions[i]->isChecked()) {
                currentAction = i;
                i=actions.size();
            }
        }
    }
    this->setText(actions[currentAction]->text());
    blockSignals(false);
    NoteTable noteTable(global.db);
    NotebookTable notebookTable(global.db);
    QString name = text();
    qint32 notebookLid = notebookTable.findByName(name);
    if (notebookLid > 0) {
        noteTable.updateNotebook(currentNoteLid, notebookLid, true);
        this->notebookLid = notebookLid;
        this->notebookName = name;
        emit(notebookChanged());
    }
}



void NotebookMenuButton::reloadData() {
    for (int i=actions.size()-1; i>=0; i--) {
        delete actions[i];
    }
    for (int i=stackMenus.size()-1; i>=0; i--) {
        delete stackMenus[i];
    }
    stackMenus.clear();
    actions.clear();
    loadData();

    // Restore the proper notebook selection
    if (currentNoteLid > 0) {
        Note n;
        NoteTable noteTable(global.db);
        NotebookTable notebookTable(global.db);
        noteTable.get(n, currentNoteLid, false, false);
        QString notebookGuid = n.notebookGuid;
        QList<qint32> bookList;
        notebookTable.getAll(bookList);
        QString bookName;

        for (int i=0; i<bookList.size(); i++) {
            Notebook book;
            notebookTable.get(book, bookList[i]);
            if (notebookGuid == book.guid) {
                bookName = book.name;
                i=bookList.size();
            }
        }
        setCurrentNotebook(currentNoteLid, n);
    }
    return;
}


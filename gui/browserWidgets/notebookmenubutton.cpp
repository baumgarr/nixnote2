#include "notebookmenubutton.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include <QMenu>
#include <QAction>



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
    f.setPointSize(10);
    rootMenu.setFont(f);
}


void NotebookMenuButton::setCurrentNotebook(int lid, Note note) {
    if (actions.size() == 0)
        loadData();

    if (lid <=0)
        return;

    blockSignals(true);
    currentNoteLid = lid;
    NotebookTable notebookTable;
    Notebook notebook;
    notebookTable.get(notebook, note.notebookGuid);
    actions[currentAction]->setChecked(false);
    this->setText(QString::fromStdString(notebook.name));
    for (int i=0; i<actions.size(); i++) {
        if (actions[i]->text().toLower().trimmed() == QString::fromStdString(notebook.name).toLower().trimmed()) {
            currentAction = i;
            actions[currentAction]->setChecked(true);
            blockSignals(false);
            return;
        }
    }
    blockSignals(false);
}


// Read in all of the data and build the menu.
void NotebookMenuButton::loadData() {
    NotebookTable notebookTable;

    QList<qint32> lids;
    notebookTable.getAll(lids);

    if (notebookTable.findByName(currentNotebookName) <= 0)
        currentNotebookName = "";

    for (qint32 i=0; i<lids.size(); i++) {
        Notebook book;
        if (notebookTable.get(book, lids[i])) {

            QAction *action = new QAction(this);
            actions.append(action);
            action->setText(QString::fromStdString(book.name));
            action->setCheckable(true);
            connect(action, SIGNAL(triggered()), this, SLOT(notebookSelected()));
            QFont f = action->font();
            f.setPixelSize(10);
            action->setFont(f);
            QMenu *currentMenu = findStack(book);

            addNotebookMenuItem(currentMenu, action);

            if (currentNotebookName == "" && book.__isset.defaultNotebook &&
                    book.defaultNotebook) {
                currentNotebookName = QString::fromStdString(book.name);
                setText(currentNotebookName);
                currentAction = actions.size()-1;
            }
            if (QString::fromStdString(book.name) == currentNotebookName) {
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
    if (!n.__isset.stack || QString::fromStdString(n.stack).trimmed() == "")
        return &rootMenu;

    QString stack = QString::fromStdString(n.stack).trimmed();
    for (int i=0; i<stackMenus.size(); i++) {
        if (stackMenus.at(i)->title().toLower() == stack.toLower())
            return stackMenus.at(i);
    }


    // Create a new stack.  We add a dummy action item to the
    // menu so we know where to add the menu later.  This
    // keeps things in sorted order
    QMenu *newMenu = new QMenu();
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
    NoteTable noteTable;
    NotebookTable notebookTable;
    QString name = text();
    qint32 notebookLid = notebookTable.findByName(name);
    if (notebookLid > 0) {
        noteTable.updateNotebook(currentNoteLid, notebookLid, true);
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

    Note n;
    NoteTable noteTable;
    NotebookTable notebookTable;
    noteTable.get(n, currentNoteLid, false,false);
    QString notebookGuid = QString::fromStdString(n.notebookGuid);
    QList<qint32> bookList;
    notebookTable.getAll(bookList);
    QString bookName;

    for (int i=0; i<bookList.size(); i++) {
        Notebook book;
        notebookTable.get(book, bookList[i]);
        if (notebookGuid == QString::fromStdString(book.guid)) {
            bookName = QString::fromStdString(book.name);
            i=bookList.size();
        }
    }
    setCurrentNotebook(currentNoteLid, n);
    return;
}


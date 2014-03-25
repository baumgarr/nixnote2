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

#include "nnotebookview.h"
#include "global.h"
#include "nnotebookviewitem.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QtSql>
#include <QPainter>
#include <QMessageBox>
#include <QTextDocument>
#include <QFontMetrics>

#include "sql/notebooktable.h"
#include "sql/linkednotebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/notetable.h"
#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "dialog/notebookproperties.h"
#include "gui/nnotebookviewdelegate.h"
#include "sql/nsqlquery.h"

#define NAME_POSITION 0

// Suppress C++ string wanings
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic push


extern Global global;

// Constructor
NNotebookView::NNotebookView(QWidget *parent) :
    QTreeWidget(parent)
{
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    filterPosition = -1;
    maxCount = 0;  // Highest count of any notebook.  Used in calculating column width
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
//    this->setStyleSheet("QTreeWidget { border-image: none; border:none; background-color: transparent; }");

    root = new NNotebookViewItem(0);
    root->setType(NNotebookViewItem::Stack);
    root->setData(NAME_POSITION, Qt::UserRole, "rootsynchronized");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Notebooks"));
    QFont rootFont = root->font(NAME_POSITION);
    rootFont.setBold(true);
    root->setFont(NAME_POSITION, rootFont);

    root->setRootColor(false);

    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    this->rebuildNotebookTreeNeeded = true;
    this->loadData();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    addAction = context.addAction(tr("Create New Notebook"));
    addAction->setShortcut(QKeySequence(Qt::Key_Insert));
    addAction->setShortcutContext(Qt::WidgetShortcut);

    addShortcut = new QShortcut(this);
    addShortcut->setKey(QKeySequence(Qt::Key_Insert));
    addShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    deleteAction = context.addAction(tr("Delete"));
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    deleteShortcut = new QShortcut(this);
    deleteShortcut->setKey(QKeySequence(Qt::Key_Delete));
    deleteShortcut->setContext(Qt::WidgetShortcut);

    // Start building the stack menu
    stackMenu = context.addMenu(tr("Add to stack"));
    QAction *newAction;
    NotebookTable table(global.db);
    QStringList stacks;
    table.getStacks(stacks);
    for (int i=0; i<stacks.size(); i++) {
        newAction = stackMenu->addAction(stacks[i]);
        connect(newAction, SIGNAL(triggered()), this, SLOT(moveToStackRequested()));
    }
    sortStackMenu();
    if (stacks.size() > 0) {
        stackMenu->addSeparator();
    }
    newStackAction = stackMenu->addAction(tr("New stack"));
    connect(newStackAction, SIGNAL(triggered()), this, SLOT(moveToNewStackRequested()));

    removeFromStackAction = context.addAction(tr("Remove from stack"));
    removeFromStackAction->setShortcutContext(Qt::WidgetShortcut);
    removeFromStackAction->setVisible(false);

    renameAction = context.addAction(tr("Rename"));
    renameAction->setShortcutContext(Qt::WidgetShortcut);

    renameShortcut = new QShortcut(this);
    renameShortcut->setKey(QKeySequence(Qt::Key_F2));
    renameShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    propertiesAction = context.addAction(tr("Properties"));

    connect(addAction, SIGNAL(triggered()), this, SLOT(addRequested()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteRequested()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(renameRequested()));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(propertiesRequested()));

    connect(addShortcut, SIGNAL(activated()), this, SLOT(addRequested()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(deleteRequested()));
    connect(renameShortcut, SIGNAL(activated()), this, SLOT(renameRequested()));
    connect(removeFromStackAction, SIGNAL(triggered()), this, SLOT(removeFromStackRequested()));

    this->setAcceptDrops(true);
    this->setItemDelegate(new NNotebookViewDelegate());
    root->setExpanded(true);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->setFrameShape(QFrame::NoFrame);
}


// Destructor
NNotebookView::~NNotebookView() {
    this->blockSignals(true);
    delete root;
}


void NNotebookView::calculateHeight()
{
    int h = 0;

    int topLevelCount = topLevelItemCount();

    for(int i = 0;i < topLevelCount;i++)    {
        QTreeWidgetItem * item = topLevelItem(i);
        h += calculateHeightRec(item);
        h += item->sizeHint(0).height() + 5;
    }

    if(h != 0)   {
        setMinimumHeight(h+10);
        setMaximumHeight(h+10);
    }
    this->setMaximumWidth(this->sizeHint().width());
}

int NNotebookView::calculateHeightRec(QTreeWidgetItem * item)
{
    if(!item)
        return 0;

    QModelIndex index = indexFromItem(item);

    if(!item->isExpanded())
    {
        return rowHeight(index);
    }

    //int h = item->sizeHint(0).height() + 2 + rowHeight(index);
    int h = item->sizeHint(0).height() +rowHeight(index);
    int childCount = item->childCount();
    for(int i = 0; i < childCount;i++)
    {
        h += calculateHeightRec(item->child(i));
    }

    return h;
}



// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NNotebookView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    if (selected)
        selectionModel()->select(item, QItemSelectionModel::Deselect);

    for (int i=0; i<this->selectedItems() .size(); i++) {
        if (this->selectedIndexes().at(i).data(Qt::UserRole).toString().startsWith("root", Qt::CaseInsensitive)) {
            if (!root->isExpanded())
                root->setExpanded(true);
            selectionModel()->select(this->selectedIndexes().at(i), QItemSelectionModel::Deselect);
        }
    }
}


// Load up the data from the database
void NNotebookView::loadData() {
    NSqlQuery query(*global.db);
    NotebookTable notebookTable(global.db);
    QList<qint32> closedLids;
    notebookTable.getClosedNotebooks(closedLids);


    QHash<qint32, NNotebookViewItem*>::iterator i1;
    for (i1=dataStore.begin(); i1!=dataStore.end(); ++i1) {
        i1.value()->setHidden(true);
    }

    QHash<QString, NNotebookViewItem*>::iterator i2;
    for (i2=stackStore.begin(); i2!=stackStore.end(); ++i2) {
        i2.value()->setHidden(true);
    }

    dataStore.clear();
    query.exec("Select lid, name, stack, username from NotebookModel order by username, name");
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        if (!notebookTable.isDeleted(query.value(0).toInt())) {
            NNotebookViewItem *newWidget = new NNotebookViewItem(lid);
            newWidget->setData(NAME_POSITION, Qt::DisplayRole, query.value(1).toString());
            newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
            if (closedLids.contains(lid))
                newWidget->setHidden(true);
            QString username = query.value(3).toString();
            if (username.trimmed() != "")
                newWidget->stack = username;
            else
                newWidget->stack = query.value(2).toString();
            this->dataStore.insert(query.value(0).toInt(), newWidget);
            root->addChild(newWidget);

            if (newWidget->stack != "" && !stackStore.contains(newWidget->stack)) {
                NNotebookViewItem *stackWidget = new NNotebookViewItem(0);
                stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
                stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
                if (username != "")
                    stackWidget->setType(NNotebookViewItem::LinkedStack);
                stackStore.insert(newWidget->stack, stackWidget);
                root->addChild(stackWidget);
            }
        }
    }
    this->rebuildTree();
    this->resetSize();
}


// Rebuild the notebook tree view
void NNotebookView::rebuildTree() {
    if (!this->rebuildNotebookTreeNeeded)
        return;

    // Go through all the widgets in the view.  If
    // it should be hidden (because the notebook is closed
    // then hide it, othwise make it visible.  If it has
    // a stack, then save the stack name later so we can
    // display stacks properly.
    NotebookTable notebookTable(global.db);
    QList<qint32> closedLids;
    notebookTable.getClosedNotebooks(closedLids);
    QHashIterator<qint32, NNotebookViewItem *> i(dataStore);
    while (i.hasNext()) {
        i.next();
        NNotebookViewItem *widget = i.value();
        if (closedLids.contains(widget->lid))
            widget->setHidden(true);
        else
            widget->setHidden(false);
        if (i.value()->stack != "") {
            NNotebookViewItem *stackWidget = NULL;
            if (stackStore.contains(i.value()->stack)) {
                stackWidget = stackStore[i.value()->stack];
            } else {
                NNotebookViewItem *stackWidget = new NNotebookViewItem(0);
                stackWidget->setData(NAME_POSITION, Qt::DisplayRole, i.value()->stack);
                stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
                stackStore.insert(widget->stack, stackWidget);
                root->addChild(stackWidget);
            }
            i.value()->parent()->removeChild(i.value());
            stackWidget->childrenLids.append(i.key());
            stackWidget->addChild(i.value());
        }
    }

    // Remove any empty stacks
    QHashIterator<QString, NNotebookViewItem *> s(stackStore);
    while (s.hasNext()) {
        s.next();
        if (s.value()->childCount() == 0) {
            root->removeChild(s.value());
            stackStore.remove(s.key());
        }
    }

    this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
    this->rebuildNotebookTreeNeeded = false;
    this->resetSize();
}

void NNotebookView::notebookUpdated(qint32 lid, QString name, QString stackName, bool isLinked, bool shared) {
    this->rebuildNotebookTreeNeeded = true;

    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NNotebookViewItem *newWidget = dataStore[lid];
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->stack = stackName;
        if (stackName != "" && !this->stackStore.contains(newWidget->stack)) {
            NNotebookViewItem *stackWidget = new NNotebookViewItem(0);
            stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
            stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
            if (isLinked)
                stackWidget->setType(NNotebookViewItem::LinkedStack);
            stackStore.insert(newWidget->stack, stackWidget);
            root->addChild(stackWidget);
        }
        root->addChild(newWidget);
    } else {
        NNotebookViewItem *newWidget = new NNotebookViewItem(lid);
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->stack = stackName;
        this->dataStore.insert(lid, newWidget);
        NNotebookViewItem *stackWidget = NULL;
        if (stackName !="" && this->stackStore.contains(newWidget->stack)) {
            stackWidget = stackStore[stackName];
        }
        if (stackName !="" && !this->stackStore.contains(newWidget->stack)) {
            stackWidget = new NNotebookViewItem(0);
            stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
            stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
            if (isLinked)
                stackWidget->setType(NNotebookViewItem::LinkedStack);
            if (shared)
                newWidget->setType(NNotebookViewItem::Shared);
            stackStore.insert(newWidget->stack, stackWidget);
            root->addChild(stackWidget);
        }

        if (stackWidget == NULL) {
            root->addChild(newWidget);
        } else {
            stackWidget->addChild(newWidget);
            stackWidget->childrenLids.append(newWidget->lid);
        }
        if (this->dataStore.count() == 1) {
            this->expandAll();
        }
    }
    resetSize();
    this->sortByColumn(NAME_POSITION);
}


void NNotebookView::resetSize() {
    calculateHeight();
}




//*************************************************************
// This function is called when a user selects something
// within this tree.  It also emits a signal that will
// be picked up by the main nixnote program.
//*************************************************************
void NNotebookView::buildSelection() {
    QLOG_TRACE() << "Inside NNotebookView::buildSelection()";

    QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
    if (selectedItems.size() > 0 && selectedItems[0]->data(0,Qt::UserRole).toString().startsWith("root"), Qt::CaseInsensitive)
        return;

    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            global.filterCriteria.removeLast();
    }

    int currentCount = global.filterCriteria.size();
    FilterCriteria *newFilter = new FilterCriteria();
    if (currentCount > 0) {
        FilterCriteria *currentFilter = global.filterCriteria[currentCount-1];
        if (currentFilter->isLidSet()) {
            newFilter->setLid(currentFilter->getLid());
        }
        if (currentFilter->isSelectedNotesSet()) {
            QList<qint32> lids;
            currentFilter->getSelectedNotes(lids);
            newFilter->setSelectedNotes(lids);
        }
    }
    global.filterCriteria.push_back(newFilter);
    filterPosition++;
    global.filterPosition++;

    if (selectedItems.size() > 0) {
        newFilter->setNotebook(*(selectedItems[0]));
    }
    qint32 notebookLid = 0;
    if (selectedItems.size() > 0)
        notebookLid = selectedItems[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    newFilter->resetAttribute = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetNotebook = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetTags = true;
    newFilter->resetSearchString = true;

    emit updateSelectionRequested();
    emit notebookSelectionChanged(notebookLid);

    QLOG_TRACE() << "Leaving NNotebookView::buildSelection()";
}


//*************************************************************
// This function is called from the main NixNote class.
// it will reset the items which are selected based upon
// what the user did somewhere else (outside this widget).
//*************************************************************
void NNotebookView::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetNotebook; i++) {
            selectedItems[i]->setSelected(false);
        }


        if (criteria->isNotebookSet()) {
            criteria->getNotebook()->setSelected(true);
        }
    }
    filterPosition = global.filterPosition;

    if (selectedItems().size() == 0)
        root->setSelected(false);

    blockSignals(false);
}





void NNotebookView::notebookExpunged(qint32 lid) {
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NNotebookViewItem *item = this->dataStore.value(lid);
        NNotebookViewItem *parent = (NNotebookViewItem*)item->parent();
        this->removeItemWidget(item, 0);
        dataStore.remove(lid);
        if (parent != NULL) {
            parent->removeChild(item);
            if (parent->childCount() == 0) {
                this->removeItemWidget(parent, 0);
                stackStore.remove(parent->stack);
                dataStore.remove(parent->lid);
                delete parent;
            }
        }
        delete item;

    }
    this->resetSize();
}














void NNotebookView::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem*> items = selectedItems();
    QAction *stackAction = stackMenu->menuAction();
    context.removeAction(stackAction);

    if (items.size() == 0) {
        propertiesAction->setEnabled(false);
        deleteAction->setEnabled(false);
        renameAction->setEnabled(false);
        removeFromStackAction->setVisible(false);
    } else {
        propertiesAction->setEnabled(true);
        deleteAction->setEnabled(true);
        renameAction->setEnabled(true);
        removeFromStackAction->setVisible(false);
        NotebookTable table(global.db);

        for (int i=0; i<items.size(); i++) {
            if (items[i]->data(NAME_POSITION, Qt::UserRole).toString() == "STACK") {
                deleteAction->setEnabled(false);
                removeFromStackAction->setVisible(false);
            } else {
                qint32 lid = items[i]->data(NAME_POSITION, Qt::UserRole).toInt();
                if (table.isStacked(lid)) {
                    removeFromStackAction->setVisible(true);
                } else {
                    removeFromStackAction->setVisible(false);
                    context.insertAction(renameAction, stackAction);
                }
            }
        }
    }
    context.exec(event->globalPos());
}



void NNotebookView::addRequested() {
    NotebookProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    dialog.setLid(0);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    NotebookTable table(global.db);
    QString name = dialog.name.text().trimmed();
    qint32 lid = table.findByName(name);
    NNotebookViewItem *newWidget = new NNotebookViewItem(lid);
    newWidget->total = 0;
    newWidget->subTotal = 0;
    newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
    newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
    this->dataStore.insert(lid, newWidget);
    root->addChild(newWidget);
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
    emit(notebookAdded(lid));
}

void NNotebookView::propertiesRequested() {
    NotebookProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    QString oldName = items[0]->data(NAME_POSITION, Qt::DisplayRole).toString();
    dialog.setLid(lid);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    QString newName = dialog.name.text().trimmed();
    if (newName != oldName) {
        items[0]->setData(NAME_POSITION, Qt::DisplayRole, newName);

        this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
        this->rebuildNotebookTreeNeeded = false;
        this->resetSize();
        emit(notebookRenamed(lid, oldName, newName));
    }
}

void NNotebookView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    if (global.confirmDeletes()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Are you sure you want to delete this notebook?"));
        msgBox.setWindowTitle(tr("Verify Delete"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
            return;
    }
    NotebookTable table(global.db);
    table.deleteNotebook(lid);
    items[0]->setHidden(true);
    dataStore.remove(lid);
    emit(notebookDeleted(lid, items[0]->data(NAME_POSITION, Qt::UserRole).toString()));
}

void NNotebookView::renameRequested() {
    editor = new TreeWidgetEditor(this);
    connect(editor, SIGNAL(editComplete()), this, SLOT(editComplete()));
    QList<QTreeWidgetItem*> items = selectedItems();
    editor->setText(items[0]->text(NAME_POSITION));
    if (items[0]->data(NAME_POSITION, Qt::UserRole).toString() != "STACK") {
        editor->lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    } else {
        editor->lid = -1;
        editor->stackName = items[0]->data(NAME_POSITION, Qt::DisplayRole).toString();
    }
    editor->setTreeWidgetItem(items[0], NAME_POSITION);
    QFontMetrics m(font());
    editor->setMinimumHeight(m.height()+4);
    editor->setMaximumHeight(m.height()+4);
    setItemWidget(items[0], NAME_POSITION, editor);
    editor->setFocus();
}


void NNotebookView::editComplete() {
    editor->setVisible(false);
    QString text = editor->text().trimmed();

    // Check if this is a notebook or a stack
    if (editor->lid > 0) {
        qint32 lid = editor->lid;
        NotebookTable table(global.db);
        Notebook notebook;
        table.get(notebook, lid);
        QString oldName = QString::fromStdString(notebook.name);

        // Check that this notebook doesn't already exist
        // if it exists, we go back to the original name
        qint32 check = table.findByName(text);
        if (check != 0) {
            NNotebookViewItem *item = dataStore[lid];
            item->setData(NAME_POSITION, Qt::DisplayRole, QString::fromStdString(notebook.name));
        } else {
            notebook.name = text.toStdString();
            table.update(notebook, true);
        }

        //delete editor;
        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
        emit(notebookRenamed(lid, oldName, text));
    } else {
        // This is if we are renaming a stack
        QString oldName = editor->stackName;
        NotebookTable table(global.db);
        table.renameStack(oldName, text);

        // Rename it in the stackStore
        NNotebookViewItem *item = stackStore[oldName];
        stackStore.remove(oldName);
        stackStore.insert(text, item);

        // Remove the old menu item
        for (int i=0; i<stackMenu->actions().size(); i++) {
            if(stackMenu->actions().at(i)->text() == oldName) {
                stackMenu->removeAction(stackMenu->actions().at(i));
                i = stackMenu->actions().size();
            }
        }
        // Create a new menu item
        bool found = false;
        for (int i=0; i<stackMenu->actions().size()-1; i++) {
            if (stackMenu->actions().at(i)->text().toUpper() > text.toUpper()) {
                QAction *newAction = stackMenu->addAction(text);
                stackMenu->removeAction(newAction);
                stackMenu->insertAction(stackMenu->actions().at(i), newAction);
                connect(newAction, SIGNAL(triggered()), this, SLOT(moveToStackRequested()));
                i=stackMenu->actions().size();
                found = true;
            }
        }
        if (!found) {
            QAction *newAction = stackMenu->addAction(text);
            stackMenu->removeAction(newAction);
            int endPos = stackMenu->actions().size()-1;
            stackMenu->insertAction(stackMenu->actions().at(endPos), newAction);
            connect(newAction, SIGNAL(triggered()), this, SLOT(moveToStackRequested()));
        }

        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
        emit(stackRenamed(oldName, text));
    }
}


void NNotebookView::moveToStackRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0)
        return;

    QAction *action = (QAction*)sender();
    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    Notebook notebook;
    NotebookTable table(global.db);
    table.get(notebook, lid);
    notebook.stack = action->text().toStdString();
    notebook.__isset.stack = true;
    table.update(notebook, true);

    // Now move it in the actual tree
    NNotebookViewItem  *stackWidget, *notebookWidget;
    stackWidget = stackStore[action->text()];
    notebookWidget = dataStore[lid];

    notebookWidget->parent()->removeChild(notebookWidget);
    stackWidget->addChild(notebookWidget);

    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
}


void NNotebookView::moveToNewStackRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0)
        return;

    QString newStackName = "New Stack";
    int i=1;
    QString iStr;
    QList<int> books;
    NotebookTable table(global.db);

    while (i>=0) {
        books.clear();
        table.findByStack(books, newStackName);
        if (books.size() == 0) {
            i=-1;
        } else {
            iStr = QVariant(i).toString();
            newStackName = tr("New Stack (") +iStr +tr(")");
            i++;
        }
    }

    // Create the new stack & move the child to it
    NNotebookViewItem *newStack = new NNotebookViewItem(0);
    newStack->setText(NAME_POSITION, newStackName);
    newStack->setData(NAME_POSITION, Qt::UserRole, "STACK");
    stackStore.insert(newStackName, newStack);
    topLevelItem(0)->addChild(newStack);
    items[0]->parent()->removeChild(items[0]);
    newStack->addChild(items[0]);

    // Create a new action item for the menu
    QAction *newAction = stackMenu->addAction(newStackName);
    connect(newAction, SIGNAL(triggered()), this, SLOT(moveToStackRequested()));
    menuData.insert(newStackName, newAction);
    sortStackMenu();

    // Update the note in the database
    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    Notebook book;
    table.get(book, lid);
    book.stack = newStackName.toStdString();
    book.__isset.stack = true;
    table.update(book, true);

    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
    newStack->setExpanded(true);
    emit(stackAdded(newStackName));
}


void NNotebookView::removeFromStackRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();
    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    NotebookTable table(global.db);
    table.removeFromStack(lid);

    // Now move it in the actual tree
    NNotebookViewItem  *stackWidget, *notebookWidget;
    notebookWidget = dataStore[lid];
    stackWidget = (NNotebookViewItem*)notebookWidget->parent();

    stackWidget->removeChild(notebookWidget);
    stackWidget->parent()->addChild(notebookWidget);

    if (stackWidget->childCount() == 0) {
        stackWidget->parent()->removeChild(stackWidget);
        QString text = stackWidget->text(NAME_POSITION);
        for (int i=0; i<stackMenu->actions().size(); i++) {
            if (stackMenu->actions().at(i)->text() == text) {
                stackMenu->actions().at(i)->setVisible(false);
                emit(stackDeleted(text));
                i = stackMenu->actions().size();
            }
        }
    }

    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
}


void NNotebookView::sortStackMenu() {
    QList<QString> keyList  = menuData.keys();
    for (int i=0; i<keyList.size(); i++) {
        stackMenu->removeAction(menuData[keyList[i]]);
    }

    // Sort the key
    qSort(keyList);

    for (int i=0; i<keyList.size(); i++) {
        stackMenu->insertAction(stackMenu->actions().at(0), menuData[keyList[i]]);
    }
}



void NNotebookView::updateTotals(qint32 lid, qint32 subTotal, qint32 total) {
    if (dataStore.contains(lid)) {
        NNotebookViewItem *item = dataStore[lid];
        item->subTotal = subTotal;
        item->total = total;
        if (subTotal > maxCount)
            maxCount = subTotal;
        repaint();
    }
}


// Handle what happens when something is dropped onto a tag item
bool NNotebookView::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) {
    index=index; // suppress unused variable
    action=action; // suppress unused variable

    // If this is a note-to-tag drop we are assigning tags to a note
    if (data->hasFormat("application/x-nixnote-note")) {
        QByteArray d = data->data("application/x-nixnote-note");
        QString data(d);

        // Find the tag lid we dropped onto
        qint32 bookLid = parent->data(NAME_POSITION, Qt::UserRole).toInt();
        if (bookLid <=0)
            return false;
        NotebookTable bookTable(global.db);
        Notebook notebook;
        bookTable.get(notebook, bookLid);

        // The string has a long list of note lids.  We parse them out & update the note
        QStringList stringLids = data.split(" ");
        for (int i=0; i<stringLids.size(); i++) {
            if (stringLids[i].trimmed() != "") {
                qint32 noteLid = stringLids.at(i).toInt();
                if (noteLid > 0) {
                    NoteTable noteTable(global.db);
                    qint32 currentNotebook = noteTable.getNotebookLid(noteLid);
                    if (currentNotebook != bookLid) {
                        noteTable.updateNotebook(noteLid, bookLid, true);
                        emit(updateNoteList(noteLid, NOTE_TABLE_NOTEBOOK_POSITION, QString::fromStdString(notebook.name)));
                        qint64 dt = QDateTime::currentMSecsSinceEpoch();
                        noteTable.updateDate(noteLid,  dt, NOTE_UPDATED_DATE, true);
                        emit(updateNoteList(noteLid, NOTE_TABLE_DATE_UPDATED_POSITION, dt));
                    }
                }
            }
        }
        if (stringLids.size() > 0) {
            emit(updateCounts());
        }
        return true;
    }


    return false;
}


// Implement of dropEvent so dropMimeData gets called
void NNotebookView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
}


// Drag tag event.  Determine if dragging is even possible
void NNotebookView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        event->accept();
        return;
    }
    event->ignore();
}


// Accept the drag move event if possible
void NNotebookView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        if (event->answerRect().intersects(childrenRect()))
            event->acceptProposedAction();
        return;
    }
}


QSize NNotebookView::sizeHint() {
    return QTreeView::sizeHint();
//    QSize sz = QTreeView::sizeHint();
//    int width=0;
//    for (int i=0; i<columnCount(); ++i) {
//        width += 2 + columnWidth(i);
//    }
//    // Calculate the spacing at the end to leave for totals
//    QFontMetrics fm(this->font());
//    QString numString = QString("(")+QString::number(maxCount) +QString(")");
//    int numWidth = fm.width(numString);

//    sz.setWidth(width+numWidth+14);  // Add some extra at the end for totals
//    return sz;
}



void NNotebookView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
    if (index.data(Qt::UserRole).toString() == "rootsynchronized")
        return;

    QTreeView::drawBranches(painter, rect, index);
}





#pragma GCC diagnostic pop

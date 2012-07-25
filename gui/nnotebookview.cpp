#include "nnotebookview.h"
#include "global.h"
#include "nnotebookviewitem.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QtSql>
#include <QMessageBox>

#include "sql/notebooktable.h"
#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

#define NAME_POSITION 0

extern Global global;

// Constructor
NNotebookView::NNotebookView(QWidget *parent) :
    QTreeWidget(parent)
{
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);


    filterPosition = -1;
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
    // QString qss = "QTreeWidget::branch:closed:has-children  {border-image: none; image: url(qss/branch-closed.png); } QTreeWidget::branch:open:has-children { border-image: none; image: url(qss/branch-open.png); }";
    //this->setStyleSheet(qss + QString("QTreeWidget { background-color: rgb(216,216,216);  border: none; image: url(qss/branch-closed.png);  }"));
    this->setStyleSheet(QString("QTreeWidget { border: none; background-color: transparent; }"));

    // Build the root item
    QIcon icon(":notebook_small.png");
    root = new NNotebookViewItem(this);
    root->setIcon(NAME_POSITION,icon);
    root->setData(NAME_POSITION, Qt::UserRole, "root");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Synchronized Notebooks"));
    root->setRootColor(true);
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
    NotebookTable table;
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
}


// Destructor
NNotebookView::~NNotebookView() {
    delete root;
}


void NNotebookView::calculateHeight()
{
    int h = 0;

    int topLevelCount = topLevelItemCount();

    for(int i = 0;i < topLevelCount;i++)
    {
        QTreeWidgetItem * item = topLevelItem(i);
        h += calculateHeightRec(item);
        h += item->sizeHint(0).height() + 5;
    }

    if(h != 0)
    {
       // h += header()->sizeHint().height();

        setMinimumHeight(h);
        setMaximumHeight(h);
    }
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

    int h = item->sizeHint(0).height() + 2 + rowHeight(index);
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
        if (this->selectedIndexes().at(i).data(Qt::UserRole) == "root") {
            selectionModel()->select(this->selectedIndexes().at(i), QItemSelectionModel::Deselect);
        }
    }
}


// Load up the data from the database
void NNotebookView::loadData() {
    QSqlQuery query;
    NotebookTable notebookTable;
    dataStore.clear();
    query.exec("Select lid, name, stack from NotebookModel order by name");
    while (query.next()) {
        if (!notebookTable.isDeleted(query.value(0).toInt())) {
            NNotebookViewItem *newWidget = new NNotebookViewItem();
            newWidget->setData(NAME_POSITION, Qt::DisplayRole, query.value(1).toString());
            newWidget->setData(NAME_POSITION, Qt::UserRole, query.value(0).toInt());
            newWidget->stack = query.value(2).toString();
            this->dataStore.insert(query.value(0).toInt(), newWidget);
            root->addChild(newWidget);
            if (newWidget->stack != "" && !stackStore.contains(newWidget->stack)) {
                NNotebookViewItem *stackWidget = new NNotebookViewItem();
                stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
                stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
                stackStore.insert(newWidget->stack, stackWidget);
                root->addChild(stackWidget);
            }
        }
    }
    this->rebuildTree();
    this->resetSize();
}

void NNotebookView::rebuildTree() {
    if (!this->rebuildNotebookTreeNeeded)
        return;

    QHashIterator<int, NNotebookViewItem *> i(dataStore);
    while (i.hasNext()) {
        i.next();
        if (i.value()->stack != "") {
            NNotebookViewItem *stackWidget = stackStore[i.value()->stack];
            i.value()->parent()->removeChild(i.value());
            stackWidget->childrenLids.append(i.key());
            stackWidget->addChild(i.value());
        }
    }

    this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
    this->rebuildNotebookTreeNeeded = false;
    this->resetSize();
}

void NNotebookView::notebookUpdated(int lid, QString name) {
    this->rebuildNotebookTreeNeeded = true;

    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NotebookTable notebookTable;
        Notebook notebook;
        notebookTable.get(notebook, lid);
        NNotebookViewItem *newWidget = dataStore.value(lid);
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->stack = QString::fromStdString(notebook.stack);
        if (notebook.stack != "" && !this->stackStore.contains(newWidget->stack)) {
            NNotebookViewItem *stackWidget = new NNotebookViewItem();
            stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
            stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
            stackStore.insert(newWidget->stack, stackWidget);
            root->addChild(stackWidget);
        }
        root->addChild(newWidget);
    } else {
        NotebookTable notebookTable;
        Notebook notebook;
        notebookTable.get(notebook, lid);
        NNotebookViewItem *newWidget = new NNotebookViewItem();
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->stack = QString::fromStdString(notebook.stack);
        this->dataStore.insert(lid, newWidget);
        if (notebook.stack != "" && !this->stackStore.contains(newWidget->stack)) {
            NNotebookViewItem *stackWidget = new NNotebookViewItem();
            stackWidget->setData(NAME_POSITION, Qt::DisplayRole, newWidget->stack);
            stackWidget->setData(NAME_POSITION, Qt::UserRole, "STACK");
            stackStore.insert(newWidget->stack, stackWidget);
            root->addChild(stackWidget);
        }
        root->addChild(newWidget);
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
    if (selectedItems.size() > 0 && selectedItems[0]->data(0,Qt::UserRole) == "root")
        return;

    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            global.filterCriteria.removeLast();
    }

    filterPosition++;
    FilterCriteria *newFilter = new FilterCriteria();
    global.filterCriteria.push_back(newFilter);
    global.filterPosition++;

    if (selectedItems.size() > 0) {
        newFilter->setNotebook(*(selectedItems[0]));
    }
    newFilter->resetAttribute = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetNotebook = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetTags = true;
    newFilter->resetSearchString = true;

    emit updateSelectionRequested();

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
        NotebookTable table;

        for (int i=0; i<items.size(); i++) {
            if (items[i]->data(NAME_POSITION, Qt::UserRole).toString() == "STACK") {
                deleteAction->setEnabled(false);
                removeFromStackAction->setVisible(false);
            } else {
                int lid = items[i]->data(NAME_POSITION, Qt::UserRole).toInt();
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
//    TagProperties dialog;
//    QList<QTreeWidgetItem*> items = selectedItems();

//    dialog.setLid(0);

//    dialog.exec();
//    if (!dialog.okPressed)
//        return;

//    TagTable table;
//    NTagViewItem *newWidget = new NTagViewItem();
//    QString name = dialog.name.text().trimmed();
//    int lid = table.findByName(name);
//    newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
//    newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
//    this->dataStore.insert(lid, newWidget);
//    root->addChild(newWidget);
//    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
//    resetSize();
//    this->sortByColumn(NAME_POSITION);
}

void NNotebookView::propertiesRequested() {
//    TagProperties dialog;
//    QList<QTreeWidgetItem*> items = selectedItems();

//    int lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
//    dialog.setLid(lid);

//    dialog.exec();
//    if (!dialog.okPressed)
//        return;
//    items[0]->setData(NAME_POSITION, Qt::DisplayRole, dialog.name.text().trimmed());
}

void NNotebookView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    int lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
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
    NotebookTable table;
    table.deleteNotebook(lid);
    items[0]->setHidden(true);
    dataStore.remove(lid);
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
    QString text = editor->text().trimmed();

    // Check if this is a notebook or a stack
    if (editor->lid > 0) {
        int lid = editor->lid;
        NotebookTable table;
        Notebook notebook;
        table.get(notebook, lid);
        QString oldName = QString::fromStdString(notebook.name);

        // Check that this notebook doesn't already exist
        // if it exists, we go back to the original name
        int check = table.findByName(text);
        if (check != 0) {
            NNotebookViewItem *item = dataStore[lid];
            item->setData(NAME_POSITION, Qt::DisplayRole, QString::fromStdString(notebook.name));
        } else {
            notebook.name = text.toStdString();
            table.update(notebook, true);
        }

        delete editor;
        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
        emit(notebookRenamed(lid, oldName, text));
    } else {
        // This is if we are renaming a stack
        QString oldName = editor->stackName;
        NotebookTable table;
        table.renameStack(oldName, text);

        // Rename it in the stackStore
        NNotebookViewItem *item = stackStore[oldName];
        stackStore.remove(oldName);
        stackStore.insert(text, item);

        // Remove the old menu item
        for (int i=0; i<stackMenu->actions().size(); i++) {
            QLOG_DEBUG() << stackMenu->actions().at(i)->text() << " " << oldName << " " << text;
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

        delete editor;
        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
    }
}


void NNotebookView::moveToStackRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0)
        return;

    QAction *action = (QAction*)sender();
    int lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    Notebook notebook;
    NotebookTable table;
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
    NotebookTable table;

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
    NNotebookViewItem *newStack = new NNotebookViewItem();
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
    int lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    Notebook book;
    table.get(book, lid);
    book.stack = newStackName.toStdString();
    book.__isset.stack = true;
    table.update(book, true);

    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
    newStack->setExpanded(true);
}


void NNotebookView::removeFromStackRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();
    int lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    NotebookTable table;
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

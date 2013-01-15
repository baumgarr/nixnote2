#include "nsearchview.h"
#include "global.h"
#include "nsearchviewitem.h"
#include "dialog/savedsearchproperties.h"
#include "sql/searchtable.h"
#include "gui/treewidgeteditor.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QtSql>
#include <QAction>
#include <QMessageBox>

#define NAME_POSITION 0

extern Global global;


// Constructor
NSearchView::NSearchView(QWidget *parent) :
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

    // Build the root item
    QIcon icon(":search.png");
    root = new NSearchViewItem(this);
    root->setIcon(NAME_POSITION,icon);
    root->setRootColor(true);
    root->setData(NAME_POSITION, Qt::UserRole, "root");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Saved Searches"));
    this->addTopLevelItem(root);
    this->loadData();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //QString qss = "QTreeWidget::branch:closed:has-children  {border-image: none; image: url(qss/branch-closed.png); } QTreeWidget::branch:open:has-children { border-image: none; image: url(qss/branch-open.png); }";
    //this->setStyleSheet(qss + " QTreeWidget { background-color: rgb(216,216,216);  border: none  }");
    this->setStyleSheet(" QTreeWidget {  border: none; background-color:transparent; }");
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    addAction = context.addAction(tr("Create Saved Search"));
    //addAction->setFont(f);
    addAction->setShortcut(QKeySequence(Qt::Key_Insert));
    addAction->setShortcutContext(Qt::WidgetShortcut);

    addShortcut = new QShortcut(this);
    addShortcut->setKey(QKeySequence(Qt::Key_Insert));
    addShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    deleteAction = context.addAction(tr("Delete"));
    //deleteAction->setFont(f);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    deleteShortcut = new QShortcut(this);
    deleteShortcut->setKey(QKeySequence(Qt::Key_Delete));
    deleteShortcut->setContext(Qt::WidgetShortcut);

    renameAction = context.addAction(tr("Rename"));
    //renameAction->setFont(f);
    renameAction->setShortcutContext(Qt::WidgetShortcut);

    renameShortcut = new QShortcut(this);
    renameShortcut->setKey(QKeySequence(Qt::Key_F2));
    renameShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    propertiesAction = context.addAction(tr("Properties"));
    //propertiesAction->setFont(f);

    connect(addAction, SIGNAL(triggered()), this, SLOT(addRequested()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteRequested()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(renameRequested()));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(propertiesRequested()));

    connect(addShortcut, SIGNAL(activated()), this, SLOT(addRequested()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(deleteRequested()));
    connect(renameShortcut, SIGNAL(activated()), this, SLOT(renameRequested()));
}


// Destructor
NSearchView::~NSearchView() {
    delete root;
}



// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NSearchView::mousePressEvent(QMouseEvent *event)
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
void NSearchView::loadData() {
    QSqlQuery query;
    query.exec("Select lid, name from SearchModel order by name");
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        SearchTable table;
        if (!table.isDeleted(lid)) {
            NSearchViewItem *newWidget = new NSearchViewItem();
            newWidget->setData(NAME_POSITION, Qt::DisplayRole, query.value(1).toString());
            newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
            this->dataStore.insert(query.value(0).toInt(), newWidget);
            root->addChild(newWidget);
        }
    }
}

void NSearchView::searchUpdated(qint32 lid, QString name) {
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NSearchViewItem *newWidget = this->dataStore.value(lid);
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
    } else {
        NSearchViewItem *newWidget = new NSearchViewItem();
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        this->dataStore.insert(lid, newWidget);
        root->addChild(newWidget);
        if (this->dataStore.count() == 1) {
            this->expandAll();
        }
    }
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
}



void NSearchView::searchExpunged(qint32 lid) {
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NSearchViewItem *item = this->dataStore.value(lid);
        this->removeItemWidget(item, 0);
        delete item;
    }
    this->resetSize();
}





void NSearchView::calculateHeight()
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
        setMinimumHeight(h);
        setMaximumHeight(h);
    }
}

int NSearchView::calculateHeightRec(QTreeWidgetItem * item)
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


void NSearchView::resetSize() {
    calculateHeight();
}





//*************************************************************
// This function is called when a user selects something
// within this tree.  It also emits a signal that will
// be picked up by the main nixnote program.
//*************************************************************
void NSearchView::buildSelection() {
    QLOG_TRACE() << "Inside NNotebookView::buildSelection()";

    QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
    if (selectedItems.size() > 0 && selectedItems[0]->data(0,Qt::UserRole) == "root")
        return;
    if (selectedItems.size() == 0)
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
    newFilter->resetNotebook =true;
    newFilter->resetTags = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetSearchString = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving NNotebookView::buildSelection()";
}


//*************************************************************
// This function is called from the main NixNote class.
// it will reset the items which are selected based upon
// what the user did somewhere else (outside this widget).
//*************************************************************
void NSearchView::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetSavedSearch; i++) {
            selectedItems[i]->setSelected(false);
        }


        if (criteria->isNotebookSet()) {
            criteria->getNotebook()->setSelected(true);
        }
    }
    filterPosition = global.filterPosition;

    blockSignals(false);
}



void NSearchView::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0) {
        propertiesAction->setEnabled(false);
        deleteAction->setEnabled(false);
        renameAction->setEnabled(false);
    } else {
        propertiesAction->setEnabled(true);
        deleteAction->setEnabled(true);
        renameAction->setEnabled(true);
    }
    context.exec(event->globalPos());
}



void NSearchView::addRequested() {
    SavedSearchProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    dialog.setLid(0);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    SearchTable table;
    NSearchViewItem *newWidget = new NSearchViewItem();
    QString name = dialog.name.text().trimmed();
    qint32 lid = table.findByName(name);
    newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
    newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
    this->dataStore.insert(lid, newWidget);
    root->addChild(newWidget);
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);

    dataStore.insert(lid, newWidget);
}

void NSearchView::propertiesRequested() {
    SavedSearchProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    dialog.setLid(lid);

    dialog.exec();
    if (!dialog.okPressed)
        return;
    items[0]->setData(NAME_POSITION, Qt::DisplayRole, dialog.name.text().trimmed());
}

void NSearchView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    if (global.confirmDeletes()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Are you sure you want to delete this saved search?"));
        msgBox.setWindowTitle(tr("Verify Delete"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
            return;
    }
    SearchTable s;
    s.deleteSearch(lid);
    items[0]->setHidden(true);
    dataStore.remove(lid);
}

void NSearchView::renameRequested() {
    editor = new TreeWidgetEditor(this);
    connect(editor, SIGNAL(editComplete()), this, SLOT(editComplete()));

    QList<QTreeWidgetItem*> items = selectedItems();
    editor->setText(items[0]->text(NAME_POSITION));
    editor->lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    editor->setTreeWidgetItem(items[0], NAME_POSITION);
    QFontMetrics m(font());
    editor->setMinimumHeight(m.height()+4);
    editor->setMaximumHeight(m.height()+4);
    setItemWidget(items[0], NAME_POSITION, editor);
    editor->setFocus();
}


void NSearchView::editComplete() {
    QString text = editor->text().trimmed();
    qint32 lid = editor->lid;
    SearchTable table;
    SavedSearch s;
    table.get(s, lid);

    // Check that this search doesn't already exist
    // if it exists, we go back to the original name
    qint32 check = table.findByName(text);
    if (check != 0) {
        NSearchViewItem *item = dataStore[lid];
        item->setData(NAME_POSITION, Qt::DisplayRole, QString::fromStdString(s.name));
    } else {
        s.name = text.toStdString();
        table.update(lid, s, true);
    }
    delete editor;
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
}



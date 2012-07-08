#include "nnotebookview.h"
#include "global.h"
#include "nnotebookviewitem.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QtSql>
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
    dataStore.clear();
    query.exec("Select lid, name, stack from NotebookModel order by name");
    while (query.next()) {
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

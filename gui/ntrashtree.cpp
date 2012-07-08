#include "ntrashtree.h"
#include "global.h"
#include <QHeaderView>
#include <QMouseEvent>


extern Global global;


NTrashTree::NTrashTree(QWidget *parent) :
    QTreeWidget(parent)
{
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    filterPosition = -1;
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
    this->setStyleSheet("QTreeWidget {  border: none; background-color:transparent; }");

    // Build the root item
    QIcon icon(":trash.png");
    root = new QTreeWidgetItem(this);
    root->setIcon(0,icon);
    root->setData(0, Qt::UserRole, "root");
    root->setData(0, Qt::DisplayRole, tr("Trash"));
    QFont font = root->font(0);
    font.setBold(true);
    root->setFont(0,font);
    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

}


void NTrashTree::calculateHeight()
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
//        h += header()->sizeHint().height();

        setMinimumHeight(h);
        setMaximumHeight(h);
    }
}

int NTrashTree::calculateHeightRec(QTreeWidgetItem * item)
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
    for(int i = 0; i<childCount; i++) {
        h += calculateHeightRec(item->child(i));
    }
    return h;
}


void NTrashTree::resetSize() {
    calculateHeight();
}


// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NTrashTree::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    if (selected) {
        selectionModel()->select(item, QItemSelectionModel::Deselect);
    }
}



//*************************************************************
// This function is called when a user selects something
// within this tree.  It also emits a signal that will
// be picked up by the main nixnote program.
//*************************************************************
void NTrashTree::buildSelection() {
    QLOG_TRACE() << "Inside NTrashTree::buildSelection()";

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


    if (root->isSelected())
        newFilter->setDeletedOnly(true);
    else
        newFilter->setDeletedOnly(false);
    newFilter->resetSavedSearch = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetAttribute = true;
    newFilter->resetNotebook =true;
    newFilter->resetTags = true;
    newFilter->resetSearchString = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving NTrashTree::buildSelection()";
}


//*************************************************************
// This function is called from the main NixNote class.
// it will reset the items which are selected based upon
// what the user did somewhere else (outside this widget).
//*************************************************************
void NTrashTree::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetDeletedOnly; i++) {
            selectedItems[i]->setSelected(false);
        }

        if (criteria->isDeletedOnlySet() && criteria->getDeletedOnly()) {
            root->setSelected(true);
        }
    }
    filterPosition = global.filterPosition;

    blockSignals(false);
}

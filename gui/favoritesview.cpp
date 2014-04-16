/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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



#include "favoritesview.h"
#include "global.h"
#include "gui/favoritesviewdelegate.h"
#include "gui/favoritesviewitem.h"
#include "sql/favoritesrecord.h"
#include "sql/favoritestable.h"
#include "sql/tagtable.h"
#include "sql/notebooktable.h"
#include "sql/searchtable.h"
#include "sql/notetable.h"
#include "gui/nnotebookviewitem.h"
#include "sql/linkednotebooktable.h"
#include "sql/sharednotebooktable.h"
#include <QMessageBox>

extern Global global;

#define NAME_POSITION 0

FavoritesView::FavoritesView(QWidget *parent) :
    QTreeWidget(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    dataStore.clear();
    targetStore.clear();
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    filterPosition = -1;
    maxCount = 0;  // Highest count of any notebook.  Used in calculating column width
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(true);
    this->header()->setVisible(false);
    this->setStyleSheet("QTreeView {border-image:none; image:none;} ");
    root = new FavoritesViewItem(0);
    root->setData(NAME_POSITION, Qt::UserRole, "root");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Favorites"));
    QFont rootFont = root->font(NAME_POSITION);
    rootFont.setBold(true);
    root->setFont(NAME_POSITION, rootFont);
    root->setIcon(NAME_POSITION, QIcon(":favorites.png"));

    root->setRootColor(false);

    expandedImage = new QImage(":expanded.png");
    collapsedImage = new QImage(":collapsed.png");
    this->setAcceptDrops(true);
    this->setItemDelegate(new FavoritesViewDelegate());
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    this->rebuildFavoritesTreeNeeded = true;
    this->loadData();

    context.addSeparator();
    deleteAction = context.addAction(tr("Remove from favorites"));
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    deleteShortcut = new QShortcut(this);
    deleteShortcut->setKey(QKeySequence(Qt::Key_Delete));
    deleteShortcut->setContext(Qt::WidgetShortcut);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteRequested()));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    root->setExpanded(true);
    resetSize();
}



// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void FavoritesView::mousePressEvent(QMouseEvent *event)
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



void FavoritesView::loadData() {
    QHashIterator<qint32, FavoritesViewItem *> i(dataStore);

    // Cleanup old items in the tree.  We go through
    // it twice.  The first time is to remove children
    // not in root (i.e. notebooks in a stack).  The
    // second time is to remove items at a root level.
    while(i.hasNext()) {
        i.next();
        if (i.value() != NULL && i.value()->parent() != root) {
            i.value()->parent()->removeChild(i.value());
        }
    }
    i.toFront();
    while(i.hasNext()) {
        i.next();
        if (i.value() != NULL) {
            i.value()->setHidden(true);
            root->removeChild(i.value());
            delete i.value();
            dataStore.remove(i.key());
        }
    }


    // Now start rebuilding the table
    FavoritesTable ftable(global.db);
    dataStore.clear();
    targetStore.clear();
    QList<qint32> lids;
    ftable.getAll(lids);
    QList<qint32> children;
    // First pass, we just get the top level ones
    for (int i=0; i<lids.size(); i++) {
        FavoritesRecord record;
        if (ftable.get(record, lids[i])) {
            if (record.parent == 0) {
                if (record.type == FavoritesRecord::LinkedStack || record.type == FavoritesRecord::NotebookStack) {
                    if (!ftable.childrenFound(lids[i])) {
                        ftable.expunge(lids[i]);
                     } else {
                        buildTreeEntry(root, &record);
                     }
                } else {
                    buildTreeEntry(root, &record);
                }
            } else {
                children.append(lids[i]);
            }
        }
    }

//    // Second pass we get any children
    for (int i=0; i<children.size(); i++) {
        FavoritesRecord record;
        if (ftable.get(record, children[i])) {
            FavoritesViewItem *parent = dataStore[record.parent];
            if (parent != NULL)
                buildTreeEntry(parent, &record);
        }
    }

    emit updateCounts();
    sortByColumn(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
}




void FavoritesView::buildTreeEntry(FavoritesViewItem *parent, const FavoritesRecord *record) {
    FavoritesViewItem *item = new FavoritesViewItem();
    item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);
    item->record.lid = record->lid;
    item->setData(NAME_POSITION, Qt::UserRole, record->lid);
    item->record.type = record->type;
    item->record.target = record->target;
    item->record.order = record->order;
    item->setText(NAME_POSITION, record->displayName);
    switch (record->type) {
    case FavoritesRecord::Tag :
        item->setIcon(NAME_POSITION, QIcon(":tag.png"));
        break;
    case FavoritesRecord::Note :
        item->setIcon(NAME_POSITION, QIcon(":newNote.png"));
        break;
    case FavoritesRecord::ConflictNotebook :
        item->setIcon(NAME_POSITION, QIcon(":notebook-conflict.png"));
        break;
    case FavoritesRecord::LinkedNotebook :
        item->setIcon(NAME_POSITION, QIcon(":notebook-linked.png"));
        break;
    case FavoritesRecord::LinkedStack :
        item->setIcon(NAME_POSITION, QIcon(":silhouette.png"));
        break;
    case FavoritesRecord::LocalNotebook :
        item->setIcon(NAME_POSITION, QIcon(":local-notebook.png"));
        break;
    case FavoritesRecord::Search :
        item->setIcon(NAME_POSITION, QIcon(":search.png"));
        break;
    case FavoritesRecord::SharedNotebook :
        item->setIcon(NAME_POSITION, QIcon(":notebook-shared.png"));
        break;
    case FavoritesRecord::SynchronizedNotebook :
        item->setIcon(NAME_POSITION, QIcon(":notebook_small.png"));
        break;
    case FavoritesRecord::NotebookStack :
        item->setIcon(NAME_POSITION, QIcon(":stack.png"));
        break;
    }
    dataStore.insert(record->lid, item);
    if (record->target.toInt() > 0)
        targetStore.insert(record->target.toInt(), item);
    parent->addChild(item);
}




void FavoritesView::calculateHeight()
{
    int h = 0;

    int topLevelCount = topLevelItemCount();

    for(int i = 0;i < topLevelCount;i++)    {
        QTreeWidgetItem * item = topLevelItem(i);
        h += calculateHeightRec(item);
        h += item->sizeHint(0).height() + 5;
    }

    if(h != 0)   {
        setMinimumHeight(h);
        setMaximumHeight(h);
    }
    this->setMaximumWidth(this->sizeHint().width());
}




int FavoritesView::calculateHeightRec(QTreeWidgetItem * item)
{
    if(!item)
        return 0;

    QModelIndex index = indexFromItem(item);

    if(!item->isExpanded())
    {
        return rowHeight(index);
    }

    int h = item->sizeHint(0).height() +rowHeight(index);
    int childCount = item->childCount();
    for(int i = 0; i < childCount;i++)
    {
        h += calculateHeightRec(item->child(i));
    }

    return h;
}




void FavoritesView::resetSize() {
    calculateHeight();
}



QSize FavoritesView::sizeHint() {
    return QTreeView::sizeHint();
}


void FavoritesView::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0) {
        deleteAction->setEnabled(false);
    } else {
        deleteAction->setEnabled(true);
    }
    context.exec(event->globalPos());
}



void FavoritesView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
    if (!index.child(0,0).isValid())
        return;

    painter->save();
    if (isExpanded(index)) {
        int offset = rect.width()-expandedImage->width()-1;
        painter->drawImage(offset, rect.y(),*expandedImage);
    } else {
        int offset = rect.width()-collapsedImage->width()-1;
        painter->drawImage(offset, rect.y(),*collapsedImage);
    }
    painter->restore();
    return;
}



// Implement of dropEvent so dropMimeData gets called
void FavoritesView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
    const QMimeData* data = event->mimeData();
    QModelIndex droppedIndex = indexAt( event->pos() );
    if (!droppedIndex.isValid())
        return;
    int row = droppedIndex.row();

    qint32 lid = 0;
    if (data->hasFormat("application/x-nixnote-tag")) {
        QByteArray d = data->data("application/x-nixnote-tag");
        lid = d.trimmed().toInt();
        dropRecord(lid, FavoritesRecord::Tag, row);
    }

    if (data->hasFormat("application/x-nixnote-note")) {
        QByteArray d = data->data("application/x-nixnote-note");
        lid = d.trimmed().toInt();
        dropRecord(lid, FavoritesRecord::Note, row);
    }
    if (data->hasFormat("application/x-nixnote-search")) {
        QByteArray d = data->data("application/x-nixnote-search");
        lid = d.trimmed().toInt();
        dropRecord(lid, FavoritesRecord::Search, row);
    }
    if (data->hasFormat("application/x-nixnote-favorite")) {
        QByteArray d = data->data("application/x-nixnote-favorite");
        lid = d.trimmed().toInt();
        FavoritesTable ftable(global.db);
        FavoritesRecord rec;
        if (ftable.get(rec,lid)) {
            ftable.expunge(lid);
            rec.order = row;
            ftable.insert(rec);
        }
    }
    if (data->hasFormat("application/x-nixnote-notebook")) {
        QString d = data->data("application/x-nixnote-notebook");
        int pos = d.indexOf("/");
        if (pos <=0) return;
        int type = d.mid(0,pos).toInt();
        d = d.mid(pos+1);
        pos = d.indexOf("/");
        if (pos <=0) return;
        lid= d.mid(0,pos).toInt();
        QString stack = d.mid(pos+1);
        FavoritesRecord::FavoritesRecordType rectype = FavoritesRecord::LocalNotebook;
        switch (type) {
        case NNotebookViewItem::Local :
            rectype = FavoritesRecord::LocalNotebook;
            break;
        case NNotebookViewItem::Synchronized :
            rectype = FavoritesRecord::SynchronizedNotebook;
            break;
        case NNotebookViewItem::LinkedStack :
            rectype = FavoritesRecord::LinkedStack;
            break;
        case NNotebookViewItem::Conflict :
            rectype = FavoritesRecord::ConflictNotebook;
            break;
        case NNotebookViewItem::Stack :
            rectype = FavoritesRecord::NotebookStack;
            break;
        case NNotebookViewItem::Shared:
            rectype = FavoritesRecord::SharedNotebook;
            break;
        case NNotebookViewItem::Linked :
            rectype = FavoritesRecord::LinkedNotebook;
            break;
        }
        if (lid > 0)
            dropRecord(lid, rectype, row);
        else {
            FavoritesTable table(global.db);
            FavoritesRecord record;
            record.type = rectype;
            record.target= stack;
            record.lid = 0;
            record.order = row;
            record.parent = 0;
            qint32 newLid = table.insert(record);

            QList<qint32> lids;
            NotebookTable ntable(global.db);
            ntable.findByStack(lids, stack);
            for (int i=0; i<lids.size(); i++) {
                Notebook book;
                if (ntable.get(book, lids[i])) {
                    FavoritesRecord rec;
                    rec.parent = newLid;
                    if (book.name.isSet())
                        rec.displayName = book.name;
                    rec.type = FavoritesRecord::SynchronizedNotebook;
                    if (ntable.isLocal(lids[i]))
                        rec.type = FavoritesRecord::LocalNotebook;
                    LinkedNotebookTable ltable(global.db);
                    if (ltable.exists(lids[i]))
                        rec.type = FavoritesRecord::LinkedNotebook;
                    SharedNotebookTable stable(global.db);
                    if (stable.exists(lids[i]))
                        rec.type = FavoritesRecord::SharedNotebook;
                    rec.target = lids[i];
                    rec.order = 1;
                    table.add(rec);
                }
            }

        }
    }


    rebuildFavoritesTreeNeeded = true;
    this->loadData();

}



void FavoritesView::dropRecord(qint32 lid, FavoritesRecord::FavoritesRecordType t, int row) {
    FavoritesTable table(global.db);
    FavoritesRecord record;
    record.type = t;
    record.target= lid;
    record.lid = 0;
    record.order = row;\
    record.parent = 0;
    table.insert(record);
}





void FavoritesView::mouseMoveEvent(QMouseEvent *event)
{
    if (currentItem() == NULL)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setData("application/x-nixnote-favorite", currentItem()->data(NAME_POSITION, Qt::UserRole).toByteArray());
    drag->setMimeData(mimeData);

    drag->exec(Qt::MoveAction);
}



// Drag event.  Determine if dragging is even possible
void FavoritesView::dragEnterEvent(QDragEnterEvent *event) {

    // Check that we are dropping proper data and that it has a proper lid or stack type

    if (event->mimeData()->hasFormat("application/x-nixnote-tag")) {
        if (event->mimeData()->data("application/x-nixnote-tag").trimmed().toInt() > 0)
            event->accept();
        else
            event->ignore();
        return;
    }

    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        if (event->mimeData()->data("application/x-nixnote-note").trimmed().toInt() > 0)
            event->accept();
        else
            event->ignore();
        return;
    }

    if (event->mimeData()->hasFormat("application/x-nixnote-search")) {
        if (event->mimeData()->data("application/x-nixnote-search").trimmed().toInt() > 0)
            event->accept();
        else
            event->ignore();
        return;
    }

    if (event->mimeData()->hasFormat("application/x-nixnote-notebook")) {
        QLOG_DEBUG() << event->mimeData()->data("application/x-nixnote-notebook").trimmed();
        if (event->mimeData()->data("application/x-nixnote-notebook").trimmed() != "")
            event->accept();
        else
            event->ignore();
        return;
    }

    if (event->mimeData()->hasFormat("application/x-nixnote-favorite")) {
        event->accept();
        return;
    }

    event->ignore();
}



// Accept the drag move event if possible
void FavoritesView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-favorites")) {
        if (event->answerRect().intersects(childrenRect()))
            event->acceptProposedAction();
        return;
    }
}



// Remove one of the favorites.  Any children will be removed automatically
void FavoritesView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    if (items[0] == root)
        return;

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    if (global.confirmDeletes()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Are you sure you want to remove this favorite?"));
        msgBox.setWindowTitle(tr("Verify Delete"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
            return;
    }
    FavoritesTable s(global.db);
    s.expunge(lid);
    items[0]->setHidden(true);
    for (int i=items[0]->childCount()-1; i>=0; i--) {
        FavoritesViewItem *child = (FavoritesViewItem*)items[0]->child(i);
        child->setHidden(true);
        child->parent()->removeChild(child);
        child = dataStore.take(child->record.lid);
        if (child != NULL) {
            targetStore.remove(child->record.target.toInt());
            delete child;
        }
    }
    FavoritesViewItem *parent = (FavoritesViewItem*)items[0]->parent();
    parent->removeChild(items[0]);
    FavoritesViewItem *item = dataStore.take(lid);
    if (item != NULL) {
        targetStore.remove(item->record.target.toInt());
        delete item;
     }
    if (parent != root) {
        if (parent->childCount() == 0) {
            root->removeChild(parent);
            parent = dataStore.take(parent->record.lid);
            if (parent != NULL) {
                targetStore.remove(parent->record.target.toInt());
                delete parent;
            }
        }
    }
    resetSize();
}



//*************************************************************
// This function is called when a user selects something
// within this tree.  It also emits a signal that will
// be picked up by the main nixnote program.
//*************************************************************
void FavoritesView::buildSelection() {
    QLOG_TRACE() << "Inside FavoritesView::buildSelection()";

    QList<QTreeWidgetItem*> selectedItems = this->selectedItems();

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
        qint32 lid = selectedItems[0]->data(NAME_POSITION, Qt::UserRole).toInt();
        newFilter->setFavorite(lid);
    }

    newFilter->resetAttribute = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetNotebook =true;
    newFilter->resetTags = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetSearchString = true;
    newFilter->resetSelectedNotes = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving FavoritesView::buildSelection()";
}



//*************************************************************
// This function is called from the main NixNote class.
// it will reset the items which are selected based upon
// what the user did somewhere else (outside this widget).
//*************************************************************
void FavoritesView::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size(); i++) {
            selectedItems[i]->setSelected(false);
        }
    }


    if (criteria->isFavoriteSet()) {
        dataStore[criteria->getFavorite()]->setSelected(true);
    }

    filterPosition = global.filterPosition;

    blockSignals(false);
}



// An item was rename
void FavoritesView::itemRenamed(qint32 lid, QString oldName, QString newName) {
    Q_UNUSED(oldName);
    if (targetStore.contains(lid)) {
        FavoritesViewItem *item = targetStore[lid];
        item->setText(NAME_POSITION, newName);
        item->record.displayName = newName;
        FavoritesTable ftable(global.db);
        ftable.add(item->record);
        repaint();
    }
}


// Update the total counts for the tag.
void FavoritesView::updateTotals(qint32 lid, qint32 subTotal, qint32 total) {
    // This is terribly inefficient, but we shouldn't need to do it too often.
    // We look for a key in the targetStore.

    if (targetStore.contains(lid)) {
        FavoritesViewItem *item = targetStore[lid];
        if (item == NULL)
            return;
        item->subTotal = subTotal;
        item->total = total;
        if (total > maxCount)
            maxCount = total;
    }
    repaint();
}

void FavoritesView::itemExpunged(qint32 lid, QString name) {
    itemExpunged(lid);
    Q_UNUSED(name);
}



void FavoritesView::itemExpunged(qint32 lid) {

    if (dataStore[lid] == NULL)
        return;

    FavoritesTable s(global.db);
    s.expunge(lid);
    dataStore[lid]->setHidden(true);
    for (int i=dataStore[lid]->childCount()-1; i>=0; i--) {
        FavoritesViewItem *child = (FavoritesViewItem*)dataStore[lid]->child(i);
        child->setHidden(true);
        child->parent()->removeChild(child);
        child = dataStore.take(child->record.lid);
        if (child != NULL) {
            targetStore.remove(child->record.target.toInt());
            delete child;
        }
    }
    FavoritesViewItem *parent = (FavoritesViewItem*)dataStore[lid]->parent();
    parent->removeChild(dataStore[lid]);
    FavoritesViewItem *item = dataStore.take(lid);
    if (item != NULL)
        delete item;
    if (parent != root) {
        if (parent->childCount() == 0) {
            root->removeChild(parent);
            parent = dataStore.take(parent->record.lid);
            if (parent != NULL) {
                targetStore.remove(parent->record.target.toInt());
                delete parent;
            }
        }
    }
    resetSize();
}




void FavoritesView::stackRenamed(QString oldName, QString newName) {
    FavoritesTable ftable(global.db);
    QList<qint32> lids;
    ftable.getAll(lids);
    for (int i=0; i<lids.size(); i++) {
        FavoritesRecord rec;
        if (ftable.get(rec, lids[i])) {
            if ((rec.type == FavoritesRecord::NotebookStack || rec.type == FavoritesRecord::LinkedStack) &&
                 rec.target.toString() == oldName) {
                rec.target = newName;
                ftable.add(rec);
                dataStore[rec.lid]->setText(NAME_POSITION, newName);
                return;
            }
        }
    }

}

void FavoritesView::stackExpunged(QString stackname) {
    FavoritesTable ftable(global.db);
    QList<qint32> lids;
    ftable.getAll(lids);
    for (int i=0; i<lids.size(); i++) {
        FavoritesRecord rec;
        if (ftable.get(rec, lids[i])) {
            if ((rec.type == FavoritesRecord::NotebookStack || rec.type == FavoritesRecord::LinkedStack) &&
                 rec.target.toString() == stackname) {
                ftable.expunge(lids[i]);
                dataStore[rec.lid]->setHidden(true);
                return;
            }
        }
    }
}

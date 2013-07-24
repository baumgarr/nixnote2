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

#include "ntagview.h"
#include "global.h"
#include "ntagviewitem.h"
#include "sql/tagtable.h"
#include "dialog/tagproperties.h"
#include "filters/filtercriteria.h"
#include "gui/ntagviewdelegate.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QByteArray>
#include <QtSql>
#include "sql/linkednotebooktable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include <QMessageBox>



#define NAME_POSITION 0

extern Global global;

// Constructor
NTagView::NTagView(QWidget *parent) :
    QTreeWidget(parent)
{
    accountFilter = 0;
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    filterPosition = 0;
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
    this->setStyleSheet("QTreeWidget { border: none; background-color:transparent;}");

    // Build the root item
    QIcon icon(":tag.png");
    root = new NTagViewItem(this);
    root->setIcon(NAME_POSITION,icon);
    root->setData(NAME_POSITION, Qt::UserRole, "root");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Tags from Personal"));
    root->setExpanded(true);
    QFont rootFont = root->font(NAME_POSITION);
    rootFont.setBold(true);
    root->setFont(NAME_POSITION, rootFont);

    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    this->rebuildTagTreeNeeded = true;
    this->loadData();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    setAcceptDrops(true);
    setDragEnabled(true);

    global.settings->beginGroup("SaveState");
    hideUnassigned = global.settings->value("hideUnassigned", false).toBool();
    global.settings->endGroup();


    addAction = context.addAction(tr("Create New Tag"));
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

    renameAction = context.addAction(tr("Rename"));
    renameAction->setShortcutContext(Qt::WidgetShortcut);

    mergeAction = context.addAction(tr("Merge"));

    renameShortcut = new QShortcut(this);
    renameShortcut->setKey(QKeySequence(Qt::Key_F2));
    renameShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    hideUnassignedAction = context.addAction(tr("Hide Unassigned"));
    hideUnassignedAction->setCheckable(true);
    hideUnassignedAction->setChecked(hideUnassigned);
    connect(hideUnassignedAction, SIGNAL(triggered()), this, SLOT(hideUnassignedTags()));

    context.addSeparator();
    propertiesAction = context.addAction(tr("Properties"));

    connect(addAction, SIGNAL(triggered()), this, SLOT(addRequested()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteRequested()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(renameRequested()));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(propertiesRequested()));
    connect(mergeAction, SIGNAL(triggered()), this, SLOT(mergeRequested()));
    connect(addShortcut, SIGNAL(activated()), this, SLOT(addRequested()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(deleteRequested()));
    connect(renameShortcut, SIGNAL(activated()), this, SLOT(renameRequested()));

    this->setItemDelegate(new NTagViewDelegate());
}


// Destructor
NTagView::~NTagView() {
    delete root;
}



// Called when the notebook has changed.  If this is a non-linked notebook we
// display all available tags owned by this user.  If it is a linked notebook
// we only show the tags that are connected to that notebook
void NTagView::notebookSelectionChanged(qint32 notebookLid) {
    LinkedNotebookTable table;
    if (table.exists(notebookLid)) {
        accountFilter = notebookLid;
        NotebookTable notebookTable;
        Notebook notebook;
        notebookTable.get(notebook, notebookLid);
        root->setData(NAME_POSITION, Qt::DisplayRole, tr("Tags from ")+QString::fromStdString(notebook.name));
    } else {
        root->setData(NAME_POSITION, Qt::DisplayRole, tr("Tags from Personal"));
        accountFilter = 0;
    }
    if (accountFilter > 0) {
        addAction->setEnabled(false);
        deleteAction->setEnabled(false);
    } else {
        addAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }
}


void NTagView::calculateHeight()
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

int NTagView::calculateHeightRec(QTreeWidgetItem * item)
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



// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NTagView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    if (!(event->buttons() & Qt::LeftButton))
            return;
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
void NTagView::loadData() {

    // Empty out the old data store
    QList<qint32> keys = dataStore.keys();
    for (int i=0; i<keys.size(); i++) {
        NTagViewItem *ptr = dataStore.take(keys[i]);
        delete ptr;
    }

    QSqlQuery query(*global.db);
    TagTable tagTable;
    query.exec("Select lid, name, parent_gid, account from TagModel order by name");
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString parentGid = query.value(2).toString();
        qint32 account = query.value(3).toInt();

        NTagViewItem *newWidget = new NTagViewItem();
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->account = account;
        if (account != accountFilter)
            newWidget->setHidden(true);
        else
            newWidget->setHidden(false);
        this->dataStore.insert(lid, newWidget);
        newWidget->parentGuid = parentGid;
        newWidget->parentLid = tagTable.getLid(parentGid);
        root->addChild(newWidget);
    }
    this->rebuildTree();
}


// Rebuild the GUI tree.
void NTagView::rebuildTree() {
    if (!this->rebuildTagTreeNeeded)
        return;

    QHashIterator<qint32, NTagViewItem *> i(dataStore);
    TagTable tagTable;

    while (i.hasNext()) {
        i.next();
        NTagViewItem *widget = i.value();
        if (widget != NULL && widget->parentGuid != "") {
            if (widget->parentLid == 0) {
                widget->parentLid = tagTable.getLid(widget->parentGuid);
            }
            NTagViewItem *parent = dataStore[widget->parentLid];
            widget->parent()->removeChild(widget);
            if (parent != NULL) {
                parent->childrenLids.append(i.key());
                parent->addChild(widget);
            }
        }
    }
    this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
    this->rebuildTagTreeNeeded = false;
    this->resetSize();
}


// A tag has been updated.   Things like a sync can cause this to be called
// because a tag's name may have changed.
void NTagView::tagUpdated(qint32 lid, QString name) {
    this->rebuildTagTreeNeeded = true;

    TagTable tagTable;
    qint32 account = tagTable.owningAccount(lid);
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        Tag tag;
        tagTable.get(tag, lid);
        NTagViewItem *newWidget = dataStore.value(lid);
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->parentGuid = QString::fromStdString(tag.parentGuid);
        newWidget->parentLid = tagTable.getLid(tag.parentGuid);
        newWidget->account = account;
        if (account != accountFilter)
            newWidget->setHidden(true);
        else
            newWidget->setHidden(false);
        root->addChild(newWidget);
    } else {
        Tag tag;
        tagTable.get(tag, lid);
        NTagViewItem *newWidget = new NTagViewItem();
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->parentGuid = QString::fromStdString(tag.parentGuid);
        newWidget->parentLid = tagTable.getLid(tag.parentGuid);
        newWidget->account = account;
        this->dataStore.insert(lid, newWidget);
        if (account != accountFilter)
            newWidget->setHidden(true);
        else
            newWidget->setHidden(false);
        root->addChild(newWidget);
        if (this->dataStore.count() == 1) {
            this->expandAll();
        }
    }
    resetSize();
    this->sortByColumn(NAME_POSITION);
}


// Force a reset of the size.  Useful when hiding tags so that the display looks
// correct.
void NTagView::resetSize() {
    calculateHeight();
}


// A tag was selected so a new FilterCriteria is created and the
// filtered table will display the results.
void NTagView::buildSelection() {
    QLOG_TRACE() << "Inside NTagView::buildSelection()";

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
        newFilter->setTags(selectedItems);
    }
    newFilter->resetAttribute = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetTags = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving NTagView::buildSelection()";
}


void NTagView::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetTags; i++) {
            selectedItems[i]->setSelected(false);
        }


        for (int i=0; i<criteria->getTags().size() && criteria->isTagsSet(); i++) {
            criteria->getTags()[i]->setSelected(true);
        }
    }
    root->setSelected(false);
    filterPosition = global.filterPosition;

    blockSignals(false);
}



// Add a new tag to the table
void NTagView::addNewTag(qint32 lid) {
    TagTable tagTable;
    Tag newTag;
    tagTable.get(newTag, lid);
    if (newTag.__isset.guid) {
        tagUpdated(lid, QString::fromStdString(newTag.name));
    }
}

// Accept the drag move event if possible
void NTagView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        if (event->answerRect().intersects(childrenRect()))
            event->acceptProposedAction();
        return;
    }
}


// Drag tag event.  Determine if dragging is even possible
void NTagView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        event->accept();
        return;
    }
    if (event->source() == this) {
        if (global.tagBehavior() == "HideInactiveCount") {
            event->ignore();
            return;
        }
        event->accept();
        return;
    }
    event->ignore();
}


// Handle what happens when something is dropped onto a tag item
bool NTagView::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) {

    // If this is a note-to-tag drop we are assigning tags to a note
    if (data->hasFormat("application/x-nixnote-note")) {
        QByteArray d = data->data("application/x-nixnote-note");
        QString data(d);

        // Find the tag lid we dropped onto
        qint32 tagLid = parent->data(NAME_POSITION, Qt::UserRole).toInt();

        // The string has a long list of note lids.  We parse them out & update the note
        QStringList stringLids = data.split(" ");
        for (int i=0; i<stringLids.size(); i++) {
            if (stringLids[i].trimmed() != "") {
                qint32 noteLid = stringLids.at(i).toInt();
                if (noteLid > 0) {
                    NoteTable noteTable;
                    if (!noteTable.hasTag(noteLid, tagLid)) {
                        noteTable.addTag(noteLid, tagLid, true);
                        QString tagString = noteTable.getNoteListTags(noteLid);
                        emit(updateNoteList(noteLid, NOTE_TABLE_TAGS_POSITION, tagString));
                        qint64 dt = QDateTime::currentMSecsSinceEpoch();
                        noteTable.updateDate(noteLid,  dt, NOTE_UPDATED_DATE, true);
                        emit(updateNoteList(noteLid, NOTE_TABLE_DATE_UPDATED_POSITION, dt));
                    }
                }
            }
        }
        if (stringLids.size() > 0)
            emit updateCounts();
        return true;
    }
    // If this is a tag-to-tag drop then we are modifying the hierarchy
    if (data->hasFormat("application/x-nixnote-tag")) {

        // If there is no parent, then they are trying to drop to the top level, which isn't permitted
        if (parent == NULL)
            return false;

//        // Get the lid we are dropping.
        QByteArray d = data->data("application/x-nixnote-tag");
        qint32 lid = d.toInt();
        if (lid == 0)
            return false;

        qint32 newParentLid = parent->data(NAME_POSITION, Qt::UserRole).toInt();
        qint32 oldParentLid = dataStore[lid]->parentLid;
        if (newParentLid == oldParentLid)
            return false;
        if (newParentLid == lid)
            return false;
        NTagViewItem *item = dataStore[lid];

        // If we had an old parent, remove the child from it.
        if (oldParentLid > 0)  {
            NTagViewItem *parent_ptr = dataStore[oldParentLid];
            for (int i=0; i<parent_ptr->childrenLids.size(); i++) {
                if (parent_ptr->childrenLids[i] == lid) {
                    parent_ptr->childrenLids.removeAt(i);
                    i=parent_ptr->childrenLids.size();
                }
            }
            parent_ptr->removeChild(item);
        } else {
            root->removeChild(item);
        }

        // Update the actual database
        Tag tag;
        TagTable tagTable;
        tagTable.get(tag, lid);
        QString guid;
        tagTable.getGuid(guid, newParentLid);
        tag.parentGuid = guid.toStdString();
        tag.__isset.parentGuid = true;
        tagTable.update(tag, true);

        if (newParentLid>0) {
            NTagViewItem *parent_ptr = dataStore[newParentLid];
            parent_ptr->addChild(item);
            parent_ptr->childrenLids.append(lid);
            item->parentLid = newParentLid;
            item->parentGuid = QString::fromStdString(tag.guid);
        } else {
            item->parentLid = 0;
            item->parentGuid = "";
            root->addChild(item);
        }

        // Resort the data
        this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
        sortItems(NAME_POSITION, Qt::AscendingOrder);
        return QTreeWidget::dropMimeData(parent, index, data, action);
    }


    return false;
}



// Implement of dropEvent so dropMimeData gets called
void NTagView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
}


void NTagView::mouseMoveEvent(QMouseEvent *event)
{
    if (currentItem() == NULL)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setData("application/x-nixnote-tag", currentItem()->data(NAME_POSITION, Qt::UserRole).toByteArray());
    drag->setMimeData(mimeData);

    drag->exec(Qt::MoveAction);
}


// Display the popup context menu
void NTagView::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem*> items = selectedItems();
    mergeAction->setEnabled(true);
    if (items.size() <=1)
        mergeAction->setEnabled(false);
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


// Add a new tag to the tree
void NTagView::addRequested() {
    TagProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    dialog.setLid(0);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    TagTable table;
    NTagViewItem *newWidget = new NTagViewItem();
    QString name = dialog.name.text().trimmed();
    qint32 lid = table.findByName(name, accountFilter);
    newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
    newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
    root->addChild(newWidget);
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);

    // Now add it to the datastore
    dataStore.insert(lid, newWidget);
    emit(tagAdded(lid));
}


// Display the properties dialog
void NTagView::propertiesRequested() {
    TagProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    QString oldName = items[0]->data(NAME_POSITION, Qt::DisplayRole).toString();
    dialog.setLid(lid);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    QString newName = dialog.name.text().trimmed();
    if (newName != oldName) {
        items[0]->setData(NAME_POSITION, Qt::DisplayRole, dialog.name.text().trimmed());

        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
        emit(tagRenamed(lid, oldName, newName));
    }
}



// Delete an item from the tree.  We really just hide it.
void NTagView::mergeRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Are you sure you want to merge these tags?"));
    msgBox.setWindowTitle(tr("Verify Merge"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret == QMessageBox::No)
        return;

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    NoteTable ntable;
    QList<qint32> notes;
    for (int j=1; j<items.size(); j++) {
        ntable.findNotesByTag(notes, items[j]->data(NAME_POSITION, Qt::UserRole).toInt());
        for (int i=0; i<notes.size(); i++) {
            if (!ntable.hasTag(notes[i], lid)) {
                ntable.addTag(notes[i], lid, true);
                QString tagString = ntable.getNoteListTags(notes[i]);
                emit(updateNoteList(notes[i], NOTE_TABLE_TAGS_POSITION, tagString));
                qint64 dt = QDateTime::currentMSecsSinceEpoch();
                ntable.updateDate(notes[i],  dt, NOTE_UPDATED_DATE, true);
                emit(updateNoteList(notes[i], NOTE_TABLE_DATE_UPDATED_POSITION, dt));
            }
        }
    }

    // Now delete the old tags.
    for (int i=1; i<items.size(); i++) {
        qint32 lid = items[i]->data(NAME_POSITION, Qt::UserRole).toInt();
        TagTable table;
        table.deleteTag(lid);

        // Now remove it in the datastore
        NTagViewItem *ptr = dataStore.take(items[i]->data(NAME_POSITION, Qt::UserRole).toInt());
        emit(tagDeleted(lid, ptr->data(NAME_POSITION, Qt::DisplayRole).toString()));
        delete ptr;
    }
}


// Delete an item from the tree.  We really just hide it.
void NTagView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    if (global.confirmDeletes()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Are you sure you want to delete this tag?"));
        msgBox.setWindowTitle(tr("Verify Delete"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
            return;
    }
    TagTable table;
    table.deleteTag(lid);
//    NTagViewItem *ptr = (NTagViewItem*)items[0];
//    ptr->setHidden(true);

    // Now remove it in the datastore
    NTagViewItem *ptr = dataStore.take(items[0]->data(NAME_POSITION, Qt::UserRole).toInt());
    emit(tagDeleted(lid, ptr->data(NAME_POSITION, Qt::DisplayRole).toString()));
    delete ptr;
}


// Rename the current tag.  This is just the setup for the edit.  When it is
// complete the editComplete() function is called so the edit can be validated.
void NTagView::renameRequested() {
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



// An edit is complete.  Validate it is an acceptable tag
void NTagView::editComplete() {
    QString text = editor->text().trimmed();
    qint32 lid = editor->lid;
    TagTable table;
    Tag tag;
    table.get(tag, lid);
    QString oldName = QString::fromStdString(tag.name);

    // Check that this tag doesn't already exist
    // if it exists, we go back to the original name
    qint32 check = table.findByName(text, accountFilter);
    if (check != 0) {
        NTagViewItem *item = dataStore[lid];
        item->setData(NAME_POSITION, Qt::DisplayRole, QString::fromStdString(tag.name));
    } else {
        tag.name = text.toStdString();
        table.update(tag, true);
    }
    delete editor;
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
    emit(tagRenamed(lid, oldName, text));
}



// A tag is purged.
void NTagView::tagExpunged(qint32 lid) {
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NTagViewItem *item = this->dataStore.value(lid);
        this->removeItemWidget(item, 0);
        delete item;
    }
    this->resetSize();
}



// Update the total counts for the tag.
void NTagView::updateTotals(qint32 lid, qint32 total) {
    if (dataStore.contains(lid)) {
        NTagViewItem *item = dataStore[lid];
        if (item != NULL)
            item->count = total;
    }
}



// If a tag has a zero count and if we should hide the tags, hide it.
// Make sure the tag's parents are also visible if the child has a non-zero
// count.
void NTagView::hideUnassignedTags() {
    NTagViewItem *item;
    if (hideUnassignedAction->isChecked())
        hideUnassigned = true;
    else
        hideUnassigned = false;

    // Save this option
    global.settings->beginGroup("SaveState");
    global.settings->setValue("hideUnassigned", hideUnassigned);
    global.settings->endGroup();
    QList<qint32> keys = dataStore.keys();

    // Unhide everything if they don't want items hidden.
    if (hideUnassigned != true) {
        for (int i=0; i<keys.size(); i++) {
            item = dataStore[keys[i]];
            if (item != NULL) {
                if (item->account == accountFilter)
                    item->setHidden(false);
                else
                    item->setHidden(true);
            }
        }
        resetSize();
        return;
    }

    // Start hiding unassigned tags
    for (int i=0; i<keys.size(); i++) {
        item = dataStore[keys[i]];
        if (item != NULL) {
            if (item->count == 0 || item->account != accountFilter)
                item->setHidden(true);
            else
                item->setHidden(false);
        }
    }

    for (int i=0; i<keys.size(); i++) {
        item = dataStore[keys[i]];
        if (item != NULL && !item->isHidden()) {
            while(item->parentLid > 0) {
                item->parent()->setHidden(false);
                item = (NTagViewItem*)item->parent();
            }
        }
    }
    resetSize();
}



// Return a pointer to the tag item
NTagViewItem* NTagView::getItem(qint32 lid) {
    return dataStore[lid];
}

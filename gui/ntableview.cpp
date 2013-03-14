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

#include "ntableview.h"
#include "global.h"
#include "datedelegate.h"
#include "numberdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <sql/resourcetable.h>
#include <QSqlQuery>
#include <QMessageBox>
#include <sql/notetable.h>
#include <QClipboard>
#include <sql/configstore.h>
#include "filters/filterengine.h"
#include "sql/usertable.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"

//*****************************************************************
//* This class overrides QTableView and is used to provide a
//* list of notes to the user
//******************************************************************

extern Global global;

using namespace boost;

//* Constructor
NTableView::NTableView(QWidget *parent) :
    QTableView(parent)
{
    QLOG_TRACE() << "Entering NTableView constructor";
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->verticalHeader()->setVisible(false);
    noteModel = new NoteModel(this);

    tableViewHeader = new NTableViewHeader(Qt::Horizontal, this);
    this->setHorizontalHeader(tableViewHeader);
    this->horizontalHeader()->setMovable(true);

    QLOG_TRACE() << "Setting up edit triggers";
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Set the default column height
    QLOG_TRACE() << "Setting up fort metrics";
    this->verticalHeader()->setDefaultSectionSize(QApplication::fontMetrics().height());

    QLOG_TRACE() << "Initializing proxy";
    this->proxy = new NoteSortFilterProxyModel();
    proxy->setSourceModel(model());
    global.settings->beginGroup("SaveState");
    Qt::SortOrder order = Qt::SortOrder(global.settings->value("sortOrder", 0).toInt());
    int col = global.settings->value("sortColumn", NOTE_TABLE_DATE_CREATED_POSITION).toInt();
    global.settings->endGroup();
    this->setSortingEnabled(true);
    proxy->setFilterKeyColumn(NOTE_TABLE_LID_POSITION);
    sortByColumn(col, order);
    noteModel->sort(col,order);

    //refreshData();
    setModel(proxy);

    // Set the date deligates
    QLOG_TRACE() << "Setting up table deligates";
    dateDelegate = new DateDelegate();
    blankNumber = new NumberDelegate(NumberDelegate::BlankNumber);
    kbNumber = new NumberDelegate(NumberDelegate::KBNumber);
    trueFalseDelegate = new TrueFalseDelegate();
    thumbnailDelegate = new ImageDelegate();
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_CREATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_SUBJECT_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_UPDATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_DELETED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_ALTITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LONGITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LATITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_SIZE_POSITION, kbNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_IS_DIRTY_POSITION, trueFalseDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_HAS_ENCRYPTION_POSITION, trueFalseDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_HAS_TODO_POSITION, trueFalseDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_THUMBNAIL_POSITION, thumbnailDelegate);

    QLOG_TRACE() << "Setting up column headers";
    this->setColumnHidden(NOTE_TABLE_LID_POSITION,true);
    this->setColumnHidden(NOTE_TABLE_NOTEBOOK_LID_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_DATE_DELETED_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_ALTITUDE_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_LATITUDE_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_LONGITUDE_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_SOURCE_APPLICATION_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_HAS_TODO_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_HAS_ENCRYPTION_POSITION, true);
    this->setColumnHidden(NOTE_TABLE_SOURCE_APPLICATION_POSITION, true);

    blockSignals(true);
    if (!isColumnHidden(NOTE_TABLE_DATE_CREATED_POSITION))
        tableViewHeader->createdDateAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_DATE_UPDATED_POSITION))
        tableViewHeader->changedDateAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_TITLE_POSITION))
        tableViewHeader->titleAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_NOTEBOOK_POSITION))
        tableViewHeader->notebookAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_AUTHOR_POSITION))
        tableViewHeader->authorAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_DATE_SUBJECT_POSITION))
        tableViewHeader->subjectDateAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_SOURCE_POSITION))
        tableViewHeader->sourceAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_SOURCE_URL_POSITION))
        tableViewHeader->urlAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_LATITUDE_POSITION))
        tableViewHeader->latitudeAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_LONGITUDE_POSITION))
        tableViewHeader->longitudeAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_ALTITUDE_POSITION))
        tableViewHeader->altitudeAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_HAS_ENCRYPTION_POSITION))
        tableViewHeader->hasEncryptionAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_HAS_TODO_POSITION))
        tableViewHeader->hasTodoAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_IS_DIRTY_POSITION))
        tableViewHeader->synchronizedAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_SIZE_POSITION))
        tableViewHeader->sizeAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_THUMBNAIL_POSITION))
        tableViewHeader->thumbnailAction->setChecked(true);
    if (!isColumnHidden(NOTE_TABLE_TAGS_POSITION))
        tableViewHeader->tagsAction->setChecked(true);

    connect(tableViewHeader, SIGNAL(setColumnVisible(int,bool)), this, SLOT(toggleColumnVisible(int,bool)));

    blockSignals(false);

    this->model()->setHeaderData(NOTE_TABLE_TITLE_POSITION, Qt::Horizontal, QObject::tr("Title"));
    this->model()->setHeaderData(NOTE_TABLE_AUTHOR_POSITION, Qt::Horizontal, QObject::tr("Author"));
    this->model()->setHeaderData(NOTE_TABLE_NOTEBOOK_POSITION, Qt::Horizontal, QObject::tr("Notebook"));
    this->model()->setHeaderData(NOTE_TABLE_TAGS_POSITION, Qt::Horizontal, QObject::tr("Tags"));
    this->model()->setHeaderData(NOTE_TABLE_DATE_CREATED_POSITION, Qt::Horizontal, QObject::tr("Date Created"));
    this->model()->setHeaderData(NOTE_TABLE_DATE_UPDATED_POSITION, Qt::Horizontal, QObject::tr("Date Updated"));
    this->model()->setHeaderData(NOTE_TABLE_DATE_SUBJECT_POSITION, Qt::Horizontal, QObject::tr("Subject Date"));
    this->model()->setHeaderData(NOTE_TABLE_DATE_DELETED_POSITION, Qt::Horizontal, QObject::tr("Deletion Date"));
    this->model()->setHeaderData(NOTE_TABLE_SOURCE_POSITION, Qt::Horizontal, QObject::tr("Source"));
    this->model()->setHeaderData(NOTE_TABLE_SOURCE_URL_POSITION, Qt::Horizontal, QObject::tr("Source Url"));
    this->model()->setHeaderData(NOTE_TABLE_SOURCE_APPLICATION_POSITION, Qt::Horizontal, QObject::tr("Source Application"));
    this->model()->setHeaderData(NOTE_TABLE_LONGITUDE_POSITION, Qt::Horizontal, QObject::tr("Longitude"));
    this->model()->setHeaderData(NOTE_TABLE_LATITUDE_POSITION, Qt::Horizontal, QObject::tr("Latitude"));
    this->model()->setHeaderData(NOTE_TABLE_ALTITUDE_POSITION, Qt::Horizontal, QObject::tr("Altitude"));
    this->model()->setHeaderData(NOTE_TABLE_HAS_ENCRYPTION_POSITION, Qt::Horizontal, QObject::tr("Has Encryption"));
    this->model()->setHeaderData(NOTE_TABLE_HAS_TODO_POSITION, Qt::Horizontal, QObject::tr("Has To-do"));
    this->model()->setHeaderData(NOTE_TABLE_IS_DIRTY_POSITION, Qt::Horizontal, QObject::tr("Sync"));
    this->model()->setHeaderData(NOTE_TABLE_SIZE_POSITION, Qt::Horizontal, QObject::tr("Size"));
    this->model()->setHeaderData(NOTE_TABLE_THUMBNAIL_POSITION, Qt::Horizontal, QObject::tr("Thumbnail"));

    contextMenu = new QMenu(this);
    QFont font;
    font.setPointSize(8);

    openNoteAction = new QAction(tr("Open Note"), this);
    contextMenu->addAction(openNoteAction);
    connect(openNoteAction, SIGNAL(triggered()), this, SLOT(openNoteContextMenuTriggered()));
    openNoteAction->setFont(font);

    deleteNoteAction = new QAction(tr("Delete Note"), this);
    contextMenu->addAction(deleteNoteAction);
    connect(deleteNoteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedNotes()));
    deleteNoteAction->setFont(font);

    restoreNoteAction = new QAction(tr("Restore Note"), this);
    contextMenu->addAction(restoreNoteAction);
    connect(restoreNoteAction, SIGNAL(triggered()), this, SLOT(restoreSelectedNotes()));
    restoreNoteAction->setFont(font);
    restoreNoteAction->setVisible(false);

    copyNoteLinkAction = new QAction(tr("Copy Note Link"), this);
    contextMenu->addAction(copyNoteLinkAction);
    copyNoteLinkAction->setFont(font);
    connect(copyNoteLinkAction, SIGNAL(triggered()), this, SLOT(copyNoteLink()));

    copyNoteAction = new QAction(tr("Duplicate Note"), this);
    contextMenu->addAction(copyNoteAction);
    copyNoteAction->setFont(font);
    connect(copyNoteAction, SIGNAL(triggered()), this, SLOT(copyNote()));

    mergeNotesAction = new QAction(tr("Merge Notes"), this);
    contextMenu->addAction(mergeNotesAction);
    mergeNotesAction->setFont(font);
    connect(mergeNotesAction, SIGNAL(triggered()), this, SLOT(mergeNotes()));

    repositionColumns();
    resizeColumns();
    setColumnsVisible();
    if (!isColumnHidden(NOTE_TABLE_THUMBNAIL_POSITION) && global.listView == Global::ListViewWide)
        verticalHeader()->setDefaultSectionSize(100);
    if (!isColumnHidden(NOTE_TABLE_THUMBNAIL_POSITION) && global.listView == Global::listViewNarrow)
        verticalHeader()->setDefaultSectionSize(100);
    QLOG_TRACE() << "Exiting NTableView constructor";

}


//* Destructor
NTableView::~NTableView() {
    delete dateDelegate;
    delete blankNumber;
    delete kbNumber;
    delete this->tableViewHeader;
    delete this->noteModel;
    delete this->proxy;
}


NoteModel* NTableView::model() {
    return noteModel;
}


void NTableView::contextMenuEvent(QContextMenuEvent *event) {
    openNoteAction->setEnabled(false);
    deleteNoteAction->setEnabled(false);
    QList<qint32> lids;
    getSelectedLids(lids);
    mergeNotesAction->setVisible(false);

    // Deterimne if the merge notes menu option is visible.
    // If it is a read-only notebook or if the number of selected notes
    // is < 1 then we can't merge.
    if (lids.size() > 1) {
        mergeNotesAction->setVisible(true);
        NoteTable nTable;
        NotebookTable bookTable;
        for (int i=0; i<lids.size(); i++) {
            qint32 notebookLid = nTable.getNotebookLid(lids[i]);
            if (bookTable.isReadOnly(notebookLid)) {
                mergeNotesAction->setVisible(false);
                i=lids.size();
            }
        }
    }
    if (lids.size() > 0) {
        bool readOnlySelected =  false;
        for (int i=0; i<lids.size(); i++) {
            Note n;
            NotebookTable bTable;
            NoteTable nTable;
            nTable.get(n, lids[i], false, false);
            qint32 notebookLid = bTable.getLid(n.notebookGuid);
            if (bTable.isReadOnly(notebookLid)) {
                readOnlySelected = true;
                i=lids.size();
            }
        }
        if (!readOnlySelected)
            deleteNoteAction->setEnabled(true);
        openNoteAction->setEnabled(true);
    }
    if (global.filterCriteria[global.filterPosition]->isDeletedOnlySet() &&
            global.filterCriteria[global.filterPosition]->getDeletedOnly())
        restoreNoteAction->setVisible(true);
    else
        restoreNoteAction->setVisible(false);
    contextMenu->popup(event->globalPos());
}



// Update the list of notes.
void NTableView::refreshData() {
    QLOG_TRACE() << "Getting valid lids in filter";
    QSqlQuery sql;
    sql.exec("select lid from filter");
    proxy->lidMap->clear();
    while(sql.next()) {
        proxy->lidMap->insert(sql.value(0).toInt(), 0);
    }
    QLOG_DEBUG() << "Valid LIDs retrieved.  Refreshing selection";
    model()->select();
    while(model()->canFetchMore())
        model()->fetchMore();
    refreshSelection();
    if (this->tableViewHeader->isThumbnailVisible())
        verticalHeader()->setDefaultSectionSize(100);
    else
        verticalHeader()->setDefaultSectionSize(QApplication::fontMetrics().height());
}


// The note list changed, so we need to reselect any valid notes.
void NTableView::refreshSelection() {

    this->blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    QList<qint32> historyList;
    criteria->getSelectedNotes(historyList);

    QLOG_TRACE() << "Highlighting selected rows after refresh";
    // Check the highlighted LIDs from the history selection.
    for (int i=0; i<historyList.size(); i++) {
        if (proxy->lidMap->contains(historyList[i])) {
            int rowLocation = proxy->lidMap->value(historyList[i]);
            if (rowLocation > 0) {
                QModelIndex modelIndex = model()->index(rowLocation,NOTE_TABLE_LID_POSITION);
                QModelIndex proxyIndex = proxy->mapFromSource(modelIndex);
                rowLocation = proxyIndex.row();
                selectRow(rowLocation);
            }
        }
    }
    QLOG_TRACE() << "Highlighting complete";

    // Make sure at least one thing is selected
    QLOG_TRACE() << "Selecting one item if nothing else is selected";
    QModelIndexList l = selectedIndexes();
    if (l.size() == 0) {
        qint32 rowLid = selectAnyNoteFromList();
        criteria->setLid(rowLid);
    }

    QLOG_TRACE() << "refleshSelection() complete";
    this->blockSignals(false);
}



// Listen for mouse press events.  This helps determine if we should
// open a note in a new window or the existing window
void NTableView::mouseReleaseEvent(QMouseEvent *e) {
    if ( e->button() == Qt::RightButton ) {
    } else if ( e->button() == Qt::LeftButton ) {
            this->openSelectedLids(false);
    } else if ( e->button() == Qt::MidButton ) {
            this->openSelectedLids(true);
    }
    QTableView::mouseReleaseEvent(e);
}



// Listen for up & down arrows
void NTableView::keyPressEvent(QKeyEvent *event) {
    QTableView::keyPressEvent(event);
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
            event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown)
        this->openSelectedLids(false);

}



// Open a selected note.  This is not done via the context menu.
void NTableView::openSelectedLids(bool newWindow) {

    QList<qint32> lids;
    getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            delete global.filterCriteria.takeAt(global.filterCriteria.size()-1);
    }

    FilterCriteria *newFilter = new FilterCriteria();
    global.filterCriteria.at(global.filterPosition)->duplicate(*newFilter);

    newFilter->setSelectedNotes(lids);
    if (lids.size() > 0)
        newFilter->setLid(lids.at(0));
    global.filterCriteria.push_back(newFilter);
    global.filterPosition++;


    if (lids.size() > 0) {
        emit openNote(newWindow);
    }
}

// Restore notes from the trash
void NTableView::restoreSelectedNotes() {
    QList<qint32> lids;
    this->getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    NoteTable ntable;
    QSqlQuery sql;
    QSqlQuery transaction;
    transaction.exec("begin");
    sql.prepare("Delete from filter where lid=:lid");
    for (int i=0; i<lids.size(); i++) {
        ntable.restoreNote(lids[i], true);
        sql.bindValue(":lid", lids[i]);
        sql.exec();
    }
    transaction.exec("commit");
    emit(notesRestored(lids));
}



// Delete the selected notes
void NTableView::deleteSelectedNotes() {
    QList<qint32> lids;
    this->getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    QString typeDelete;
    QString msg;
    FilterCriteria *f  = global.filterCriteria[global.filterPosition];
    bool expunged = false;
    typeDelete = tr("Delete ");

    if (f->isDeletedOnlySet() && f->getDeletedOnly()) {
        typeDelete = tr("Permanently delete ");
        expunged = true;
    }


    if (lids.size() == 1)
        msg = typeDelete + tr("selected note?");
    else
        msg = typeDelete +QString::number(lids.size()) + " notes?";

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Verify Delete"));
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int rc = msgBox.exec();
    if (rc != QMessageBox::Yes)
        return;

    NoteTable ntable;
    QSqlQuery sql;
    QSqlQuery transaction;
    transaction.exec("begin");
    sql.prepare("Delete from filter where lid=:lid");
    for (int i=0; i<lids.size(); i++) {
        ntable.deleteNote(lids[i], true);
        if (expunged)
            ntable.expunge(lids[i]);
        sql.bindValue(":lid", lids[i]);
        sql.exec();
        delete global.cache[lids[i]];
        global.cache.remove(lids[i]);
    }
    transaction.exec("commit");
    emit(notesDeleted(lids, expunged));
}


// Get a list of selected lids from the table
void NTableView::getSelectedLids(QList<qint32> &lids) {

    lids.empty();

    // This is a bit of a hack.  Basically we loop through
    // everything selected.  For each item selected we look at
    // the lid position and pull the lid from the table.
    // Since multiple items in a row are selected, we end up
    // getting the same lid multiple times, but it is the best
    // way since I can't determine exactly how many rows are
    // selected.
    QModelIndexList l = selectedIndexes();
    for (int i=0; i<l.size(); i++) {
        qint32 currentLid =  l.at(i).sibling(l.at(i).row(),NOTE_TABLE_LID_POSITION).data().toInt();
        if (!lids.contains(currentLid)) {
            lids.append(currentLid);
        }
    }
}



// Check if any specific lid is selected
bool NTableView::isLidSelected(qint32 lid) {
    QModelIndexList l = selectedIndexes();
    for (int i=0; i<l.size(); i++) {
        qint32 currentLid =  l.at(i).sibling(l.at(i).row(),NOTE_TABLE_LID_POSITION).data().toInt();
        if (currentLid == lid)
            return true;
    }
    return false;
}



// Pick a note, any note, to open.  This is normally done to force any note to be opened if nothing is currently
// selected.
qint32 NTableView::selectAnyNoteFromList() {
    QModelIndexList l = selectedIndexes();
    int rowCount = proxy->rowCount(QModelIndex());
    for (int j=rowCount-1; j>=0; j--) {
        QModelIndex idx = proxy->index(j,NOTE_TABLE_LID_POSITION);
        qint32 rowLid = idx.data().toInt();
        if (rowLid > 0) {
            QLOG_DEBUG() << ""  << "Selecting row " << j << "lid: " << rowLid;
            selectRow(j);
            this->blockSignals(true);
            emit openNote(false);
            this->blockSignals(false);
            return rowLid;
        }
    }
    return -1;
}



// A user asked to open new notes via the context menu.
void NTableView::openNoteContextMenuTriggered() {
    QList<qint32> lids;
    getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            delete global.filterCriteria.takeAt(global.filterCriteria.size()-1);
    }

    for (int i=0; i<lids.size(); i++) {
        FilterCriteria *newFilter = new FilterCriteria();
        global.filterCriteria.at(global.filterPosition)->duplicate(*newFilter);

        newFilter->setSelectedNotes(lids);
        newFilter->setLid(lids.at(i));
        global.filterCriteria.push_back(newFilter);
        global.filterPosition++;
        emit openNote(true);
    }
}


// Copy (duplicate) a note
void NTableView::copyNote() {
    QList<qint32> lids;
    ConfigStore cs;
    getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    NoteTable noteTable;
    for (int i=0; i<lids.size(); i++) {
        noteTable.duplicateNote(lids[i]);
    }
    FilterEngine engine;
    engine.filter();
    refreshData();
}



// Copy a note link into the clipboard
void NTableView::copyNoteLink() {
    QList<qint32> lids;
    getSelectedLids(lids);
    if (lids.size() == 0)
        return;


    UserTable userTable;
    User user;
    userTable.getUser(user);
    bool syncneeded = false;
    QString userid;

    if (user.__isset.id)
        userid = QVariant(user.id).toString();
    else {
        syncneeded = true;
        userid = "0000";
    }

    QString shard;
    if (user.__isset.shardId)
        shard = QString::fromStdString(user.shardId);
    else {
        syncneeded = true;
        shard = "s0";
    }

    Note note;
    NoteTable ntable;
    ntable.get(note, lids[0], false,false);

    QString guid = QString::fromStdString(note.guid);
    QString localid;
    if (!note.__isset.updateSequenceNum || note.updateSequenceNum == 0) {
        syncneeded = true;
        localid = QString::number(lids[0]);
    } else
        localid = guid;

    QString lidUrl = "evernote:///view/" + userid +"/" +shard + "/" +guid + "/" +localid;
    if (syncneeded) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Unsynchronized Note"));
        msgBox.setText(tr("This note has never been synchronized.\nUsing this in a note link can cause problems unless you synchronize it first."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
    global.clipboard->setText(lidUrl);
}



// Toggle columns hidden or visible
void NTableView::toggleColumnVisible(int position, bool visible) {
    setColumnHidden(position, !visible);
    if (this->tableViewHeader->isThumbnailVisible())
        verticalHeader()->setDefaultSectionSize(100);
    else
        verticalHeader()->setDefaultSectionSize(QApplication::fontMetrics().height());
}



// Save which columns are visible so it can be restored on the next stat
void NTableView::saveColumnsVisible() {
    if (global.listView == Global::ListViewWide)
        global.settings->beginGroup("ColumnHidden-Wide");
    else
        global.settings->beginGroup("ColumnHidden-Narrow");

    bool value = isColumnHidden(NOTE_TABLE_ALTITUDE_POSITION);
    global.settings->setValue("altitude", value);

    value = isColumnHidden(NOTE_TABLE_AUTHOR_POSITION);
    global.settings->setValue("author", value);

    value = isColumnHidden(NOTE_TABLE_DATE_CREATED_POSITION);
    global.settings->setValue("dateCreated", value);

    value = isColumnHidden(NOTE_TABLE_DATE_DELETED_POSITION);
    global.settings->setValue("dateDeleted", value);

    value = isColumnHidden(NOTE_TABLE_DATE_SUBJECT_POSITION);
    global.settings->setValue("dateSubject", value);

    value = isColumnHidden(NOTE_TABLE_DATE_UPDATED_POSITION);
    global.settings->setValue("dateUpdated", value);

    value = isColumnHidden(NOTE_TABLE_HAS_ENCRYPTION_POSITION);
    global.settings->setValue("hasEncryption", value);

    value = isColumnHidden(NOTE_TABLE_AUTHOR_POSITION);
    global.settings->setValue("author", value);

    value = isColumnHidden(NOTE_TABLE_HAS_TODO_POSITION);
    global.settings->setValue("hasTodo", value);

    value = isColumnHidden(NOTE_TABLE_IS_DIRTY_POSITION);
    global.settings->setValue("isDirty", value);

    value = isColumnHidden(NOTE_TABLE_LATITUDE_POSITION);
    global.settings->setValue("latitude", value);

    value = isColumnHidden(NOTE_TABLE_LID_POSITION);
    global.settings->setValue("lid", value);

    value = isColumnHidden(NOTE_TABLE_LONGITUDE_POSITION);
    global.settings->setValue("longitude", value);

    value = isColumnHidden(NOTE_TABLE_NOTEBOOK_LID_POSITION);
    global.settings->setValue("notebookLid", value);

    value = isColumnHidden(NOTE_TABLE_NOTEBOOK_POSITION);
    global.settings->setValue("notebook", value);

    value = isColumnHidden(NOTE_TABLE_SIZE_POSITION);
    global.settings->setValue("size", value);

    value = isColumnHidden(NOTE_TABLE_THUMBNAIL_POSITION);
    global.settings->setValue("thumbnail", value);

    value = isColumnHidden(NOTE_TABLE_SOURCE_APPLICATION_POSITION);
    global.settings->setValue("sourceApplication", value);

    value = isColumnHidden(NOTE_TABLE_SOURCE_POSITION);
    global.settings->setValue("source", value);

    value = isColumnHidden(NOTE_TABLE_SOURCE_URL_POSITION);
    global.settings->setValue("sourceUrl", value);

    value = isColumnHidden(NOTE_TABLE_TAGS_POSITION);
    global.settings->setValue("tags", value);

    value = isColumnHidden(NOTE_TABLE_TITLE_POSITION);
    global.settings->setValue("title", value);

    global.settings->endGroup();
}



// Set which columns are visible (used after restarting)
void NTableView::setColumnsVisible() {
    if (global.listView == Global::ListViewWide)
        global.settings->beginGroup("ColumnHidden-Wide");
    else
        global.settings->beginGroup("ColumnHidden-Narrow");

    bool value = global.settings->value("dateCreated", false).toBool();
    tableViewHeader->createdDateAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_DATE_CREATED_POSITION, value);

    value = global.settings->value("dateUpdated", false).toBool();
    tableViewHeader->changedDateAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_DATE_UPDATED_POSITION, value);

    value = global.settings->value("dateSubject", true).toBool();
    tableViewHeader->subjectDateAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_DATE_SUBJECT_POSITION, value);

    value = global.settings->value("tags", false).toBool();
    tableViewHeader->tagsAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_TAGS_POSITION, value);

    value = global.settings->value("title", false).toBool();
    tableViewHeader->titleAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_TITLE_POSITION, value);

    value = global.settings->value("notebook", false).toBool();
    tableViewHeader->notebookAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_NOTEBOOK_POSITION, value);

    value = global.settings->value("isDirty", false).toBool();
    tableViewHeader->synchronizedAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_IS_DIRTY_POSITION, value);

    value = global.settings->value("source", true).toBool();
    tableViewHeader->sourceAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_SOURCE_POSITION, value);

    value = global.settings->value("author", false).toBool();
    tableViewHeader->authorAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_AUTHOR_POSITION, value);

    value = global.settings->value("sourceUrl", true).toBool();
    tableViewHeader->urlAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_SOURCE_URL_POSITION, value);

    value = global.settings->value("altitude", true).toBool();
    tableViewHeader->altitudeAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_ALTITUDE_POSITION, value);

    value = global.settings->value("longitude", true).toBool();
    tableViewHeader->longitudeAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_LONGITUDE_POSITION, value);

    value = global.settings->value("latitude", true).toBool();
    tableViewHeader->latitudeAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_LATITUDE_POSITION, value);

    value = global.settings->value("hasTodo", false).toBool();
    tableViewHeader->hasTodoAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_HAS_TODO_POSITION, value);

    value = global.settings->value("hasEncryption", false).toBool();
    tableViewHeader->hasEncryptionAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_HAS_ENCRYPTION_POSITION, value);

    value = global.settings->value("size", false).toBool();
    tableViewHeader->sizeAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_SIZE_POSITION, value);

    value = global.settings->value("thumbnail", true).toBool();
    tableViewHeader->thumbnailAction->setChecked(!value);
    setColumnHidden(NOTE_TABLE_THUMBNAIL_POSITION, value);

    global.settings->endGroup();
}


// Change the order of the columns (used after restarting)
void NTableView::repositionColumns() {
    int from = horizontalHeader()->visualIndex(NOTE_TABLE_AUTHOR_POSITION);
    int to = global.getColumnPosition("noteTableAuthorPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_ALTITUDE_POSITION);
    to = global.getColumnPosition("noteTableAltitudePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_DATE_CREATED_POSITION);
    to = global.getColumnPosition("noteTableDateCreatedPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_DATE_DELETED_POSITION);
    to = global.getColumnPosition("noteTableDateDeletedPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_DATE_SUBJECT_POSITION);
    to = global.getColumnPosition("noteTableDateSubjectPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_DATE_UPDATED_POSITION);
    to = global.getColumnPosition("noteTableDateUpdatedPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_HAS_ENCRYPTION_POSITION);
    to = global.getColumnPosition("noteTableHasEncryptionPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_HAS_TODO_POSITION);
    to = global.getColumnPosition("noteTableHasTodoPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_IS_DIRTY_POSITION);
    to = global.getColumnPosition("noteTableIsDirtyPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_LATITUDE_POSITION);
    to = global.getColumnPosition("noteTableLatitudePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_LID_POSITION);
    to = global.getColumnPosition("noteTableLidPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_LONGITUDE_POSITION);
    to = global.getColumnPosition("noteTableLongitudePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_NOTEBOOK_LID_POSITION);
    to = global.getColumnPosition("noteTableNotebookLidPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_NOTEBOOK_POSITION);
    to = global.getColumnPosition("noteTableNotebookPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_SIZE_POSITION);
    to = global.getColumnPosition("noteTableSizePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_THUMBNAIL_POSITION);
    to = global.getColumnPosition("noteTableThumbnailPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_APPLICATION_POSITION);
    to = global.getColumnPosition("noteTableSourceApplicationPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_POSITION);
    to = global.getColumnPosition("noteTableSourcePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_URL_POSITION);
    to = global.getColumnPosition("noteTableSourceUrlPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_TAGS_POSITION);
    to = global.getColumnPosition("noteTableTagsPosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);

    from = horizontalHeader()->visualIndex(NOTE_TABLE_TITLE_POSITION);
    to = global.getColumnPosition("noteTableTitlePosition");
    if (to>=0) horizontalHeader()->moveSection(from, to);
}


// Change the size of the columns (used after restarting)
void NTableView::resizeColumns() {
    int width = global.getColumnWidth("noteTableAltitudePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_ALTITUDE_POSITION, width);
    width = global.getColumnWidth("noteTableAuthorPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_AUTHOR_POSITION, width);
    width = global.getColumnWidth("noteTableDateCreatedPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_DATE_CREATED_POSITION, width);
    width = global.getColumnWidth("noteTableDateDeletedPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_DATE_DELETED_POSITION, width);
    width = global.getColumnWidth("noteTableDateSubjectPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_DATE_SUBJECT_POSITION, width);
    width = global.getColumnWidth("noteTableDateUpdatedPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_DATE_UPDATED_POSITION, width);
    width = global.getColumnWidth("noteTableHasEncryptionPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_HAS_ENCRYPTION_POSITION, width);
    width = global.getColumnWidth("noteTableTodoPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_HAS_TODO_POSITION, width);
    width = global.getColumnWidth("noteTableIsDirtyPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_IS_DIRTY_POSITION, width);
    width = global.getColumnWidth("noteTableLatitudePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_LATITUDE_POSITION, width);
    width = global.getColumnWidth("noteTableLidPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_LID_POSITION, width);
    width = global.getColumnWidth("noteTableLongitudePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_LONGITUDE_POSITION, width);
    width = global.getColumnWidth("noteTableNotebookLidPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_NOTEBOOK_LID_POSITION, width);
    width = global.getColumnWidth("noteTableNotebookPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_NOTEBOOK_POSITION, width);
    width = global.getColumnWidth("noteTableSizePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_SIZE_POSITION, width);
    width = global.getColumnWidth("noteTableSourceApplicationPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_SOURCE_APPLICATION_POSITION, width);
    width = global.getColumnWidth("noteTableSourcePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_SOURCE_POSITION, width);
    width = global.getColumnWidth("noteTableSourceUrlPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_SOURCE_URL_POSITION, width);
    width = global.getColumnWidth("noteTableTagsPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_TAGS_POSITION, width);
    width = global.getColumnWidth("noteTableTitlePosition");
    if (width>0) setColumnWidth(NOTE_TABLE_TITLE_POSITION, width);
    width = global.getColumnWidth("noteTableThumbnailPosition");
    if (width>0) setColumnWidth(NOTE_TABLE_THUMBNAIL_POSITION, width);
}


// Combine multiple notes
void NTableView::mergeNotes() {
    QList<qint32> lids;
    getSelectedLids(lids);
    if (lids.size() == 0)
        return;

    NoteTable nTable;
    ResourceTable rTable;

    Note note;
    qint32 lid = lids[0];
    nTable.get(note, lid, false,false);
    QString content = QString::fromStdString(note.content);
    content = content.replace("</en-note>","<p/>");

    // Duplicate the source notes so we can undelete them later if something
    // goes horribly wrong
    for (int i=1; i<lids.size(); i++) {
        qint32 newLid = nTable.duplicateNote(lids[i]);
        QList<qint32> resLids;
        rTable.getResourceList(resLids, newLid);
        for (int j=0; j<resLids.size(); j++) {
            rTable.updateNoteLid(resLids[j], lid);
        }

        Note oldNote;
        nTable.get(oldNote, lids[i], false,false);
        QString oldContent = QString::fromStdString(oldNote.content);
        oldContent = oldContent.replace("</en-note>", "<p/>");
        int startPos = oldContent.indexOf("<en-note");
        startPos = oldContent.indexOf(">", startPos)+1;
        content = content+oldContent.mid(startPos);
        QLOG_DEBUG() << content;

        nTable.deleteNote(lids[i], true);
        nTable.expunge(newLid);
    }
    content = content+QString("</en-note>");
    QLOG_DEBUG() << content;
    nTable.updateNoteContent(lid, content, true);
    global.cache.remove(lid);

    FilterEngine engine;
    engine.filter();
    refreshData();
    emit(refreshNoteContent(lid));
}

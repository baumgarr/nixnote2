#include "ntableview.h"
#include "global.h"
#include "datedelegate.h"
#include "numberdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QSqlQuery>
#include <QMessageBox>
#include <sql/notetable.h>
#include <QClipboard>
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
    header = new NTableViewHeader(Qt::Horizontal, this);
    this->verticalHeader()->setVisible(false);
    noteModel = new NoteModel(this);

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
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_CREATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_SUBJECT_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_UPDATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_DELETED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_ALTITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LONGITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LATITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_SIZE_POSITION, kbNumber);

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
    this->model()->setHeaderData(NOTE_TABLE_HAS_TODO_POSITION, Qt::Horizontal, QObject::tr("Has Todo"));
    this->model()->setHeaderData(NOTE_TABLE_IS_DIRTY_POSITION, Qt::Horizontal, QObject::tr("Synchronized"));
    this->model()->setHeaderData(NOTE_TABLE_SIZE_POSITION, Qt::Horizontal, QObject::tr("Size"));

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

    copyNoteAction = new QAction(tr("Copy Note"), this);
    contextMenu->addAction(copyNoteAction);
    copyNoteAction->setFont(font);
    connect(copyNoteAction, SIGNAL(triggered()), this, SLOT(copyNote()));

    QLOG_TRACE() << "Exiting NTableView constructor";
}


//* Destructor
NTableView::~NTableView() {
    delete dateDelegate;
    delete blankNumber;
    delete kbNumber;
    delete this->header;
    delete this->noteModel;
    delete this->proxy;
}


// On a focus out event, save the sort order.
void NTableView::focusOutEvent(QFocusEvent *event) {
    int order = proxy->sortOrder();
    int col = proxy->sortColumn();
    global.settings->beginGroup("SaveState");
    global.settings->setValue("sortOrder", order);
    global.settings->setValue("sortColumn", col);
    global.settings->endGroup();
    QTableView::focusOutEvent(event);
}


NoteModel* NTableView::model() {
    return noteModel;
}


void NTableView::contextMenuEvent(QContextMenuEvent *event) {
    openNoteAction->setEnabled(false);
    deleteNoteAction->setEnabled(false);
    QList<qint32> lids;
    getSelectedLids(lids);
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
            this->blockSignals(false);
            emit openNote(false);
            this->blockSignals(true);
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



void NTableView::copyNote() {

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

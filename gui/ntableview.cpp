#include "ntableview.h"
#include "global.h"
#include "datedelegate.h"
#include "numberdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QSqlQuery>


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
    proxy->setFilterKeyColumn(NOTE_TABLE_LID_POSITION);
    //refreshData();
    setModel(proxy);
    this->setSortingEnabled(true);

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


NoteModel* NTableView::model() {
    return noteModel;
}



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



void NTableView::refreshSelection() {

    this->blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    QList<qint32> historyList;
    criteria->getSelectedNotes(historyList);

//    if (global.filterPosition > 0 && historyList.size() == 0) {
//        FilterCriteria *priorCriteria = global.filterCriteria[global.filterPosition-1];

//        if (priorCriteria->isSelectedNotesSet() && !criteria->isSelectedNotesSet()) {
//            QList<qint32> oldList;
//            priorCriteria->getSelectedNotes(oldList);
//            criteria->setSelectedNotes(oldList);
//            criteria->getSelectedNotes(historyList);
//        }
//        if (priorCriteria->isLidSet() && !criteria->isLidSet()) {
//            qint32 priorLid = priorCriteria->getLid();
//            qint32 currentLid = criteria->getLid();
//            if (historyList.contains(priorLid))
//                criteria->setLid(priorCriteria->getLid());
//        }
//    }

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
        int rowCount = proxy->rowCount(QModelIndex());
        for (int j=rowCount-1; j>=0; j--) {
            QModelIndex idx = proxy->index(j,NOTE_TABLE_LID_POSITION);
            qint32 rowLid = idx.data().toInt();
            if (rowLid > 0) {
                QLOG_DEBUG() << ""  << "Selecting row " << j << "lid: " << rowLid;
                criteria->setLid(rowLid);
                selectRow(j);
                this->blockSignals(false);
                emit openNote(false);
                this->blockSignals(true);
                j=-1;
            }
        }
    }

    QLOG_TRACE() << "refleshSelection() complete";
    this->blockSignals(false);
}




void NTableView::mouseReleaseEvent(QMouseEvent *e) {
    if ( e->button() == Qt::RightButton ) {
    } else if ( e->button() == Qt::LeftButton ) {
            this->getSelectedLids(false);
    } else if ( e->button() == Qt::MidButton ) {
            this->getSelectedLids(true);
    }
    QTableView::mouseReleaseEvent(e);
}


void NTableView::keyPressEvent(QKeyEvent *event) {
    QTableView::keyPressEvent(event);
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
            event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown)
        this->getSelectedLids(false);

}

void NTableView::getSelectedLids(bool newWindow) {

    QList<qint32> lids;
    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            delete global.filterCriteria.takeAt(global.filterCriteria.size()-1);
    }
    FilterCriteria *newFilter = new FilterCriteria();
    global.filterCriteria.at(global.filterPosition)->duplicate(*newFilter);
    filterPosition++;
    global.filterCriteria.push_back(newFilter);
    global.filterPosition++;



    // This is a bit of a hack.  Basically we loop through
    // everything selected.  For each item selected we look at
    // the lid position and pull the lid from the table.
    // Since multiple items in a row are selected, we end up
    // getting the same lid multiple times, but it is the best
    // way since I can't determine exactly how many rows are
    // selected.
    qint32 priorLid = -1;
    QModelIndexList l = selectedIndexes();
    for (int i=0; i<l.size(); i++) {
        qint32 currentLid =  l.at(i).sibling(l.at(i).row(),NOTE_TABLE_LID_POSITION).data().toInt();
        if (priorLid != currentLid) {
            lids.append(currentLid);
            priorLid = currentLid;
        }
    }

    newFilter->setSelectedNotes(lids);
    if (lids.size() > 0) {
        newFilter->setLid(lids.at(0));
        emit openNote(newWindow);
    }
}



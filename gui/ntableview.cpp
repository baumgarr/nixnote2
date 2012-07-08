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
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    header = new NTableViewHeader(Qt::Horizontal, this);
    this->verticalHeader()->setVisible(false);
    noteModel = new NoteModel();
    this->setModel( noteModel );

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

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->proxy = new NoteSortFilterProxyModel();
    this->setSortingEnabled(true);

    // Set the default column height
    this->verticalHeader()->setDefaultSectionSize(QApplication::fontMetrics().height());

    // Set the date deligates
    DateDelegate *dateDelegate = new DateDelegate();
    NumberDelegate *blankNumber = new NumberDelegate(NumberDelegate::BlankNumber);
    NumberDelegate *kbNumber = new NumberDelegate(NumberDelegate::KBNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_CREATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_SUBJECT_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_UPDATED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_DATE_DELETED_POSITION, dateDelegate);
    this->setItemDelegateForColumn(NOTE_TABLE_ALTITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LONGITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_LATITUDE_POSITION, blankNumber);
    this->setItemDelegateForColumn(NOTE_TABLE_SIZE_POSITION, kbNumber);
}


//* Destructor
NTableView::~NTableView() {
    delete this->header;
    delete this->noteModel;
}


NoteModel* NTableView::model() {
    return noteModel;
}



void NTableView::refreshData() {
    model()->select();

    QSqlQuery sql;
    sql.exec("select lid from filter");
    validLids.empty();
    while(sql.next()) {
        validLids.append(sql.value(0).toInt());
    }
    refreshSelection();
}



void NTableView::refreshSelection() {

    this->blockSignals(true);
    this->clearSelection();

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    QList<int> historyList;
    criteria->getSelectedNotes(historyList);

    // Check the highlighted LIDs from the history selection.
    for (int i=0; i<historyList.size() && criteria->isSelectedNotesSet(); i++) {

        // If the list of vaild lids (lids we can see in the list)
        // contains a note in the history list, we need to highlight it.
        if (validLids.contains(historyList.at(i))) {

            // Go through each row in the list.  If it is in the history, then
            // we highlight it.
            for (int j=0; j<model()->rowCount(QModelIndex()); j++) {
                QModelIndex idx = model()->index(j,NOTE_TABLE_LID_POSITION);
                int rowLid = idx.data().toInt();
                if (historyList.contains(rowLid)) {
                    this->selectRow(j);
                }
            }
        }
    }

    // ugly hack to hide stupid blank row that somehow always seems to appear
    for (int i=0; i < model()->rowCount(QModelIndex()); i++) {
        QModelIndex idx = model()->index(i,NOTE_TABLE_LID_POSITION);
        QString rowLid = idx.data().toString();
        if (rowLid.trimmed() == "")
            hideRow(i);
    }

    // Make sure at least one thing is selected
    QModelIndexList l = selectedIndexes();
    if (l.size() == 0) {
        for (int j=model()->rowCount(QModelIndex())-1; j>=0; j--) {
            QModelIndex idx = model()->index(j,NOTE_TABLE_LID_POSITION);
            int rowLid = idx.data().toInt();
            if (rowLid > 0) {
                criteria->setContent(rowLid);
                selectRow(j);
                this->blockSignals(false);
                emit openNote(false);
                this->blockSignals(true);
                j=-1;
            }
        }

    }

    this->blockSignals(false);
}




void NTableView::mouseReleaseEvent(QMouseEvent *e) {
    if ( e->button() == Qt::RightButton ) {
            QLOG_DEBUG() << "Right Button";
    } else if ( e->button() == Qt::LeftButton ) {
            QLOG_DEBUG() << "Left Button";
            this->getSelectedLids(false);
    } else if ( e->button() == Qt::MidButton ) {
            QLOG_DEBUG() << "Middle Button";
            this->getSelectedLids(true);
    }
    QTableView::mouseReleaseEvent(e);
}

void NTableView::getSelectedLids(bool newWindow) {

    QList<int> lids;
    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            global.filterCriteria.removeLast();
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
    int priorLid = -1;
    QModelIndexList l = selectedIndexes();
    for (int i=0; i<l.size(); i++) {
        int currentLid =  l.at(i).sibling(l.at(i).row(),NOTE_TABLE_LID_POSITION).data().toInt();
        if (priorLid != currentLid) {
            lids.append(currentLid);
            priorLid = currentLid;
        }
    }

    newFilter->setSelectedNotes(lids);
    if (lids.size() > 0) {
        newFilter->setContent(lids.at(0));
        emit openNote(newWindow);
    }
}


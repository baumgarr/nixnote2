/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "lineedit.h"
#include <QToolButton>
#include <QStyle>
#include <QPainter>
#include "global.h"

extern Global global;


 LineEdit::LineEdit(QWidget *parent)
     : QLineEdit(parent)
 {

     filterPosition = -1;

     QString css = global.getThemeCss("searchInputCss");
     if (css == "")
         css = "QLineEdit { padding-right: %1px; }";
     setStyleSheet(css);
#if QT_VERSION > 0x050000
     this->setClearButtonEnabled(true);
     this->setStyleSheet(css);
#endif
     defaultText = QString(tr("Search"));
     this->setPlaceholderText(defaultText);


     connect(this, SIGNAL(returnPressed()), this, SLOT(buildSelection()));
     connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
 }




 //*************************************************************
 // This function is called when a user selects something
 // within this tree.  It also emits a signal that will
 // be picked up by the main nixnote program.
 //*************************************************************
 void LineEdit::buildSelection() {
     QLOG_TRACE() << "Inside LineEdit::buildSelection()";
     savedText = text().trimmed();

     // First, find out if we're already viewing history.  If we are we
     // chop off the end of the history & start a new one
     if (global.filterPosition+1 < global.filterCriteria.size()) {
         while (global.filterPosition+1 < global.filterCriteria.size())
             global.filterCriteria.removeLast();
     }

     filterPosition++;
     FilterCriteria *newFilter = new FilterCriteria();
     global.filterCriteria.push_back(newFilter);
     FilterCriteria *oldFilter = global.filterCriteria[global.filterPosition];
     global.filterPosition++;

     newFilter->setSearchString(text());
     if (!global.getClearNotebookOnSearch() && oldFilter->isNotebookSet())
         newFilter->setNotebook(*oldFilter->getNotebook());
     if (!global.getClearTagsOnSearch() && oldFilter->isTagsSet()) {
         QList<QTreeWidgetItem*> items = oldFilter->getTags();
         newFilter->setTags(items);
     }
     newFilter->resetTags=true;
     newFilter->resetNotebook=true;
     newFilter->resetAttribute = true;
     newFilter->resetFavorite = true;
     newFilter->resetDeletedOnly = true;
     newFilter->resetSavedSearch = true;
     QList<qint32> oldLids;
     oldFilter->getSelectedNotes(oldLids);
     newFilter->setSelectedNotes(oldLids);
     newFilter->setLid(oldFilter->getLid());

     emit updateSelectionRequested();

     QLOG_TRACE() << "Leaving LineEdit::buildSelection()";
 }


 //*************************************************************
 // This function is called from the main NixNote class.
 // it will reset the items which are selected based upon
 // what the user did somewhere else (outside this widget).
 //*************************************************************
 void LineEdit::updateSelection() {
     blockSignals(true);
     FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
     if (global.filterPosition != filterPosition) {
         if (criteria->isSearchStringSet()) {
             setText(criteria->getSearchString());
         }
     }
     filterPosition = global.filterPosition;
     blockSignals(false);
 }



void LineEdit::textChanged(QString text) {
    if ((text == defaultText || text == "") && savedText != "") {
        buildSelection();
    }
}


// Check if any value is set
bool LineEdit::isSet() {
    if (this->text().trimmed() != "")
        return true;
    else
        return false;
}

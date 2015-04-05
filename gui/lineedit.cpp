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
     clearButton = new QToolButton(this);
     QPixmap pixmap(global.getPixmapResource(":fileclose.png"));
     clearButton->setIcon(QIcon(pixmap));
     clearButton->setIconSize(pixmap.size());
     clearButton->setCursor(Qt::ArrowCursor);
     clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
     clearButton->hide();
     connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
     connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));
     int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
     setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
     QSize msz = minimumSizeHint();
     setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                    qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));

     defaultText = QString(tr("Search"));
     this->setText(defaultText);

     inactiveColor = "QLineEdit {color: gray; font:italic;} ";
     activeColor = "QLineEdit {color: black; font:normal;} ";

     connect(this, SIGNAL(returnPressed()), this, SLOT(buildSelection()));
     connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
     connect(clearButton, SIGNAL(clicked()), this, SLOT(buildSelection()));
     setStyleSheet(inactiveColor);
 }

 void LineEdit::resizeEvent(QResizeEvent *)
 {
     QSize sz = clearButton->sizeHint();
     int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
     clearButton->move(rect().right() - frameWidth - sz.width(),
                       (rect().bottom() + 1 - sz.height())/2);
 }

 void LineEdit::updateCloseButton(const QString& text)
 {
     clearButton->setVisible(!text.isEmpty());
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
     newFilter->resetNotebook = true;
     newFilter->resetTags = true;
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
//     if (criteria->resetSearchString) {
//         this->blockSignals(true);
//         this->setText(defaultText);
//         setStyleSheet(inactiveColor);
//         this->blockSignals(false);
//     }
     if (global.filterPosition != filterPosition) {
         if (criteria->resetSearchString) {
             setText(defaultText);
             setStyleSheet(inactiveColor);
         }

         if (criteria->isSearchStringSet()) {
             setText(criteria->getSearchString());
             setStyleSheet(activeColor);
         }
     }
     filterPosition = global.filterPosition;

     blockSignals(false);
 }

void LineEdit::textChanged(QString text) {
    this->blockSignals(true);
    if (text.trimmed() == "" && !hasFocus())
        this->setText(defaultText);
    this->blockSignals(false);
    if ((text == defaultText || text == "") && savedText != "") {
        buildSelection();
    }
}

void LineEdit::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  if (this->text() == defaultText) {
    blockSignals(true);
    setText("");
    blockSignals(false);
  }
  setStyleSheet(activeColor);
//  this->setCursor(Qt::ArrowCursor);
}

void LineEdit::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  if (this->text().trimmed() == "") {
    blockSignals(true);
    setText(defaultText);
    blockSignals(false);
    setStyleSheet(inactiveColor);
  }
//  this->setCursor(Qt::PointingHandCursor);

}


// Check if any value is set
bool LineEdit::isSet() {
    if (this->text().trimmed() != defaultText && this->text().trimmed() != "")
        return true;
    else
        return false;
}




void LineEdit::reloadIcons() {
    clearButton->setIcon(global.getIconResource(":filecloseIcon"));
}

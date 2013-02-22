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

#include "datetimeeditor.h"
#include <QDateTimeEdit>
#include <QVBoxLayout>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

#include "sql/notetable.h"

using namespace evernote::edam  ;
using namespace std;

DateTimeEditor::DateTimeEditor(QWidget *parent) :
    QDateTimeEdit(parent)
{
    QFont f = font();
    f.setPointSize(8);
    this->setFont(f);

    QPalette pal;
    pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    inactiveColor = "QDateTimeEdit {background-color: transparent; border-radius: 1px;} QDateTimeEdit:hover {border: 1px solid #808080; background-color: white; border-radius: 4px;} QDateTimeEdit::up-button {width: 0px; image:none;} QDateTimeEdit::down-button{width: 0px; image: none;}";
    activeColor = "QDateTimeEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;}  QDateTimeEdit::up-button {width: 14px;} QDateTimeEdit::down-button{width: 14px;}";

    //editor.setCalendarPopup(true);
    //display.setStyleSheet(inactiveColor);
    setStyleSheet(inactiveColor);
    setCalendarPopup(true);

    setCursor(Qt::PointingHandCursor);

    connect(this, SIGNAL(dateChanged(QDate)), this, SLOT(valueChanged()));
    connect(this, SIGNAL(timeChanged(QTime)), this, SLOT(valueChanged()));

    hide();
}



void DateTimeEditor::focusOutEvent(QFocusEvent *e)
{
  QDateTimeEdit::focusOutEvent(e);
  this->setCursor(Qt::PointingHandCursor);
  setStyleSheet(inactiveColor);
}

void DateTimeEditor::focusInEvent(QFocusEvent *e)
{
  QDateTimeEdit::focusInEvent(e);
  this->setCursor(Qt::ArrowCursor);
  setStyleSheet(activeColor);
}


void DateTimeEditor::setNote(qint32 lid, Timestamp value, qint32 key) {
    this->lid = lid;
    this->value = value;
    this->key = key;
    QDateTime dt;
    dt.setMSecsSinceEpoch(value);
    blockSignals(true);
    setDateTime(dt);
    blockSignals(false);
}



void DateTimeEditor::valueChanged() {
    NoteTable noteTable;

    QDateTime dt = this->dateTime();
    noteTable.updateDate(lid, dt.toMSecsSinceEpoch(), key, true);
}


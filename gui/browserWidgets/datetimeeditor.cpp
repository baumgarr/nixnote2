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

#include "global.h"

#include "sql/notetable.h"

using namespace std;

extern Global global;

DateTimeEditor::DateTimeEditor(QWidget *parent) :
    QDateTimeEdit(parent)
{
    this->setFont(global.getGuiFont(font()));

    QPalette pal;
    QString test = global.getDateTimeEditorColor();
    pal.setColor(QPalette::Text, QColor(test));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    inactiveColor = global.getThemeCss("dateMenuInactiveCss");
    if (inactiveColor=="")
        inactiveColor = global.getDateTimeEditorInactiveStyle();
    activeColor = global.getThemeCss("dateMenuActiveCss");
    if (activeColor=="")
        activeColor = global.getDateTimeEditorActiveStyle();

    setStyleSheet(inactiveColor);
    setCalendarPopup(true);

    setCursor(Qt::PointingHandCursor);

    connect(this, SIGNAL(dateChanged(QDate)), this, SLOT(valueChanged()));
    connect(this, SIGNAL(timeChanged(QTime)), this, SLOT(valueChanged()));
    this->setDisplayFormat(global.dateFormat + " " +global.timeFormat);

    hide();

    QString css = global.getThemeCss("dateMenuCss");
    if (css!="")
        this->setStyleSheet(css);

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

void DateTimeEditor::setDate(qint64 value) {
    this->blockSignals(true);
    QDateTime dt;
    dt.setMSecsSinceEpoch(value);
    setDateTime(dt);
    this->blockSignals(false);
}



void DateTimeEditor::valueChanged() {
    NoteTable noteTable(global.db);

    QDateTime dt = this->dateTime();
    noteTable.updateDate(lid, dt.toMSecsSinceEpoch(), key, true);
}


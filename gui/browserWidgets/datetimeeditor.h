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

#ifndef DATETIMEEDITOR_H
#define DATETIMEEDITOR_H

#include <QDateTimeEdit>
#include <QDateTime>
#include <QWidget>
#include <QPushButton>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

using namespace evernote::edam  ;
using namespace std;


class DateTimeEditor : public QDateTimeEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    Timestamp value;
    qint32 key;
    qint32 lid;

public:
    explicit DateTimeEditor(QWidget *parent = 0);
    void setNote(qint32 lid, Timestamp value, qint32 key);
    void setDate(qint64 value);
    
signals:
    
protected:
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);

public slots:

private slots:
    void valueChanged();

    
};

#endif // DATETIMEEDITOR_H

/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#ifndef REMOTEQUERY_H
#define REMOTEQUERY_H

#include <QObject>
#if QT_VERSION < 0x050000
#include <QtDBus/QtDBus>
#else
#include <QtDBus>
#endif
#include <QList>
#include <QString>
#include "sql/notetable.h"


#define DBUS_SERVICE_NAME "com.canonical.Unity.Scope.Notes.NixNote"

class RemoteQuery : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Scope.Notes.NixNote.RemoteQuery")
private:
    Note *note;
    qint32 lid;

public:
    explicit RemoteQuery(QObject *parent = 0);
    void initDbus();

signals:

public slots:
    Q_SCRIPTABLE QString query(const QString &arg);
    Q_SCRIPTABLE QString getNoteTitle();
    Q_SCRIPTABLE bool setNote(qint32 lid);
    Q_SCRIPTABLE QString getNotePreview();
    Q_SCRIPTABLE QString getNoteDateUpdated();
    Q_SCRIPTABLE QString getNoteDateCreated();
    Q_SCRIPTABLE QString getNoteTags();
};


#endif // REMOTEQUERY_H

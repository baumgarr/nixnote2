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



#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QtXml>
#include <QProgressDialog>
#include "evernote/UserStore.h"

using namespace std;
using namespace evernote::edam  ;



class ExportData : public QObject
{
    Q_OBJECT
private:
    bool quitNow;
    void createNode(QString nodeName, QString value);
    void createNode(QString nodeName, string value);
    void createLongLongNode(QString nodeName, qlonglong value);
    void createNode(QString nodeName, qint32 value);
    void createBinaryNode(QString nodeName, string value);
    void createTimestampNode(QString nodeName, Timestamp value);
    void createNode(QString nodeName, bool value);
    void createNode(QString nodeName, QBool value);
    void createNode(QString nodeName, double value);
    void createNode(QString nodename, int64_t value);
    void writeTags();
    void writeNotebooks();
    void writeSavedSearches();
    void writeLinkedNotebooks();
    void writeSharedNotebooks();
    void writeNotes();
    void writeUser(User user);
    void writeData(QString name, Data data);
    void writeResource(Resource r);
    QProgressDialog *progress;


public:
    bool backup;
    explicit ExportData(bool backup, QObject *parent = 0);
    void backupData(QString filename);
    int lastError;
    QString errorMessage;
    QXmlStreamWriter *writer;
    QList<qint32> lids;

signals:

public slots:
    void abortBackup();

};

#endif // EXPORTDATA_H

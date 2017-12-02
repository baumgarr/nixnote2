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

#ifndef INDEXRUNNER_H
#define INDEXRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include "sql/databaseconnection.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <QFileInfo>
#include <QTimer>
#include <QTextDocument>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

using namespace std;

// Forward declare classes used later
class DatabaseConnection;

class IndexRecord : public QObject
{
    Q_OBJECT
public:
    qint32 lid;
    qint32 weight;
    QString source;
    QString content;
};




class IndexRunner : public QObject
{
    Q_OBJECT
private:
    QTimer *indexTimer;
    QHash<qint32, IndexRecord*> *indexHash;
    bool init;
    void indexRecognition(qint32 lid, Resource &r);
    void indexNote(qint32 lid, Note &n);
    void indexPdf(qint32 lid, Resource &r);
    void indexAttachment(qint32 lid, Resource &r);
    QTextDocument *textDocument;
    DatabaseConnection *db;
    void flushCache();
    void busy(bool value, bool finished);
    bool iAmBusy;

public:
    bool enableIndexing;
    bool keepRunning;
    bool pauseIndexing;
    void initialize();
    bool officeFound;
    IndexRunner();
    ~IndexRunner();

signals:
    void thumbnailNeeded(qint32);
    void indexDone(bool finished);

 public slots:
    void index();

};

#endif // INDEXRUNNER_H

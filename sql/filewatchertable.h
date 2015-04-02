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

#ifndef FILEWATCHERTABLE_H
#define FILEWATCHERTABLE_H

#include <QObject>
#include "watcher/filewatcher.h"
#include "global.h"

extern Global global;

//*************************************
//* This table is used to keep track
//* of what directories to "watch".
//* Files found in these directories are
//* imparted automatically.
//*************************************

// Define key types
#define FILE_WATCHER_DIR 100   // The base directory
#define FILE_WATCHER_TYPE 101  // ImportDelete or ImportKeep
#define FILE_WATCHER_NOTEBOOK 102 // The notebook to import to
#define FILE_WATCHER_SUBDIRS 103  // Include subdirectories?

class FileWatcherTable : public QObject
{
    Q_OBJECT
public:
    explicit FileWatcherTable(QSqlDatabase *db);
    QSqlDatabase *db;

    // DB Read Functions
    void get(qint32 lid, QString &baseDir, FileWatcher::ScanType &type, qint32 &notebookLid, bool &includeSubdirs);  // Get a record
    qint32 findLidByDir(QString baseDir);   // Find a LID by the directory name
    qint32 getAll(QList<qint32> &lids);     // Get all watcher LIDs

    // DB Write Functions
    qint32 addEntry(qint32 lid, QString baseDir, FileWatcher::ScanType type, qint32 notebookLid, bool includeSubdirs);  // Add a record
    void expunge(qint32 lid);               // Delete a record
    
signals:
    
public slots:
    
};

#endif // FILEWATCHERTABLE_H

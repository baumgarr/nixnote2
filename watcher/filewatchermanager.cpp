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

#include "filewatchermanager.h"
#include "sql/filewatchertable.h"

#include "global.h"

extern Global global;

FileWatcherManager::FileWatcherManager(QObject *parent) :
    QObject(parent)
{
}


void FileWatcherManager::reset() {
    for (int i=importDelete.size()-1; i>=0; i--) {
        FileWatcher *fw = importDelete.takeAt(i);
        delete fw;
    }
    for (int i=importKeep.size()-1; i>=0; i--) {
        FileWatcher *fw = importKeep.takeAt(i);
        delete fw;
    }
}

void FileWatcherManager::signalImported(qint32 noteLid, qint32 resourceLid) {
    emit fileImported(noteLid, resourceLid);
}


void FileWatcherManager::signalImported() {
    emit fileImported();
}

void FileWatcherManager::setup() {
    this->reset();

    // Setup the dbi file for batch creation of notes
    FileWatcher *dbi = new FileWatcher(global.fileManager.getDbiDirPath(), FileWatcher::ImportDelete, 0, false, 0);
    connect(dbi, SIGNAL(nnexImported()), this, SLOT(signalImported()));
    importDelete.append(dbi);

    QList<qint32> lids;
    FileWatcherTable ft(global.db);
    ft.getAll(lids);

    for (int i=0; i<lids.size(); i++) {
        FileWatcher::ScanType type;
        QString dir;
        qint32 notebookLid;
        bool includeSubdirs;
        ft.get(lids[i], dir, type, notebookLid, includeSubdirs);
        FileWatcher *fw = new FileWatcher(dir, type, notebookLid, includeSubdirs);
        if (type == FileWatcher::ImportDelete)
           importDelete.append(fw);
        else
            importKeep.append(fw);
        connect(fw, SIGNAL(fileImported(qint32,qint32)), this, SLOT(signalImported(qint32,qint32)));
    }
}


void FileWatcherManager::dump() {
    for (int i=0; i<importKeep.size(); i++) {
        FileWatcher *fw = importKeep[i];
        for (int j=0; j<fw->directories().size(); j++) {
            QLOG_DEBUG() << fw->directories().at(j);
        }
    }
}

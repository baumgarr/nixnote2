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

#include "filewatcher.h"
#include "global.h"
#include "sql/configstore.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "sql/resourcetable.h"
#include "utilities/mimereference.h"
#include "sql/filewatchertable.h"
#include "xml/batchimport.h"

#include <QDirIterator>

extern Global global;

FileWatcher::FileWatcher(QString dir, ScanType type, qint32 notebookLid, bool subdirs, QObject *parent) :
    QFileSystemWatcher(parent)
{
    this->notebookLid = notebookLid;
    this->dir = dir;
    this->scanType = type;
    this->includeSubdirectories = subdirs;
    addDirectory(dir);

    connect(this, SIGNAL(directoryChanged(QString)), this, SLOT(saveDirectory(QString)));
    connect(this, SIGNAL(fileChanged(QString)), this, SLOT(saveFile(QString)));
}


void FileWatcher::saveDirectory(QString dir){
    QStringList files;
    QStringList dirs;
    setupSubDirectories(dirs, files, dir);
    for (int i=0; i<files.size(); i++) {
        if (!saveFiles.contains(files[i]) || scanType == ImportDelete) {
            saveFiles.append(files[i]);
            saveFile(files[i]);
        }
    }
}

void FileWatcher::saveFile(QString file) {
    QFileInfo fileInfo(file);

    // If we have a dbi import file
    QLOG_DEBUG() << fileInfo.dir().absolutePath() + QDir::separator();
    QLOG_DEBUG() << global.fileManager.getDbiDirPath();
    if ((fileInfo.dir().absolutePath() + QDir::separator()) == global.fileManager.getDbiDirPath()) {
        BatchImport importer;
        importer.import(file);
        emit(nnexImported());
        QFile f(file);
        f.remove();
        return;
    }


    // If we have a user-import file
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();
    if (f.size() == 0)
        return;

    Note newNote;
    NoteTable ntable(global.db);
    ConfigStore cs(global.db);
    qint32 lid = cs.incrementLidCounter();

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(data, QCryptographicHash::Md5);

    // * Start setting up the new note
    newNote.guid = QString::number(lid);
    newNote.title = file;

    NotebookTable bookTable(global.db);
    QString notebook;
    bookTable.getGuid(notebook, notebookLid);
    newNote.notebookGuid = notebook;

    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\">");

    MimeReference mimeRef;
    QString mime = mimeRef.getMimeFromFileName(file);
    QString enMedia =QString("<en-media hash=\"") +hash.toHex() +QString("\" border=\"0\"")
            +QString(" type=\"" +mime +"\" ")
            +QString("/>");
    newNoteBody.append(enMedia + QString("</en-note>"));
    newNote.content = newNoteBody;
    newNote.active = true;
    newNote.created = QDateTime::currentMSecsSinceEpoch();;
    newNote.updated = newNote.created;
    newNote.updateSequenceNum = 0;
    NoteAttributes na;
    na.sourceURL = "file://" + file;
    newNote.attributes = na;

    qint32 noteLid = lid;
    ntable.add(lid, newNote, true);
    QString noteGuid = ntable.getGuid(lid);
    lid = cs.incrementLidCounter();


    // Start creating the new resource
    Resource newRes;
    Data d;
    d.body = data;
    d.bodyHash = hash;
    d.size = data.size();
    newRes.data = d;
    newRes.mime = mime;
    ResourceAttributes ra;
    ra.fileName = file;
    if (mime.startsWith("image", Qt::CaseInsensitive) || mime.endsWith("pdf", Qt::CaseInsensitive))
        ra.attachment = false;
    else
        ra.attachment = true;
    newRes.active = true;
    newRes.guid = QString::number(lid);
    newRes.noteGuid = noteGuid;
    newRes.updateSequenceNum = 0;
    ResourceTable restable(global.db);
    restable.add(lid, newRes, true, noteLid);

    emit(fileImported(noteLid, lid));

    if (scanType == FileWatcher::ImportDelete) {
        QLOG_DEBUG() << f.remove();
    }
}


void FileWatcher::setupSubDirectories(QStringList &directories, QStringList &files, QString root) {
    QDirIterator it(root, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString entry = it.next();
        QFileInfo fileInfo;
        fileInfo.setFile(entry);
        if (fileInfo.isFile() && !files.contains(fileInfo.absoluteFilePath())) {
            files.append(fileInfo.absoluteFilePath());
        }
        if (fileInfo.isDir() && !entry.endsWith("..") && !directories.contains(fileInfo.absoluteFilePath())) {
            directories.append(fileInfo.absoluteFilePath());
        }
    }
}



void FileWatcher::setupDirectory(QStringList &files, QString root) {
    QDirIterator it(root);
    files.clear();
    while (it.hasNext()) {
        QString entry = it.next();
        QFileInfo fileInfo;
        fileInfo.setFile(entry);
        if (fileInfo.isFile() && !files.contains(fileInfo.absoluteFilePath())) {
            files.append(fileInfo.absoluteFilePath());
        }
    }
}




void FileWatcher::addDirectory(QString root) {
    if (directories().contains(root))
        return;

    QStringList dirs;
    QStringList files;
    if (includeSubdirectories) {
        setupSubDirectories(dirs, files, root);
        if (!dirs.isEmpty())
           addPaths(dirs);
        if (!files.isEmpty())
            addPaths(files);
    } else {
        setupDirectory(files, root);
        if (!root.isEmpty())
            addPath(root);
        if (!files.isEmpty())
            addPaths(files);
    }
    if (scanType == ImportDelete) {
        for (int i=0; i<files.size(); i++) {
            saveFile(files[i]);
        }
    }
    if (scanType == ImportKeep)
        saveFiles = files;

}

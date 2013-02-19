#include "filewatcher.h"
#include "global.h"
#include "sql/configstore.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "sql/resourcetable.h"
#include "utilities/mimereference.h"
#include "sql/filewatchertable.h"

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
    Note newNote;
    NoteTable ntable;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();
    if (f.size() == 0)
        return;

    QLOG_DEBUG() << data;
    ConfigStore cs;
    qint32 lid = cs.incrementLidCounter();

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(data, QCryptographicHash::Md5);

    // * Start setting up the new note
    newNote.guid = QString::number(lid).toStdString();
    newNote.__isset.guid = true;
    newNote.title = file.toStdString();
    newNote.__isset.title = true;

    NotebookTable bookTable;
    QString notebook;
    bookTable.getGuid(notebook, notebookLid);
    newNote.notebookGuid = notebook.toStdString();
    newNote.__isset.notebookGuid = true;

    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\">");

    MimeReference mimeRef;
    QString mime = mimeRef.getMimeFromFileName(file);
    QString enMedia =QString("<en-media hash=\"") +hash.toHex() +QString("\" border=\"0\"")
            +QString(" type=\"" +mime +"\" ")
            +QString("/>");
    newNoteBody.append(enMedia) + QString("</en-note>");
    newNote.content = newNoteBody.toStdString();
    newNote.__isset.content = true;
    newNote.active = true;
    newNote.__isset.active = true;
    newNote.created = QDateTime::currentMSecsSinceEpoch();;
    newNote.__isset.created = true;
    newNote.updated = newNote.created;
    newNote.__isset.updated = true;
    newNote.updateSequenceNum = 0;
    newNote.__isset.updateSequenceNum = true;
    newNote.attributes.sourceURL = "file://" + file.toStdString();
    newNote.attributes.__isset.sourceURL = true;
    newNote.__isset.attributes = true;


    qint32 noteLid = lid;
    ntable.add(lid, newNote, true);
    QString noteGuid = ntable.getGuid(lid);
    lid = cs.incrementLidCounter();


    // Start creating the new resource
    Resource newRes;
    string bodystring(data.constData(), data.size());
    newRes.data.body = bodystring;
    string hashstring(hash.constData(), hash.size());
    newRes.data.bodyHash = hashstring;
    newRes.data.size = data.size();
    newRes.data.__isset.body = true;
    newRes.data.__isset.bodyHash = true;
    newRes.data.__isset.size = true;
    newRes.__isset.data = true;
    newRes.mime = mime.toStdString();
    newRes.__isset.mime = true;
    newRes.attributes.fileName = file.toStdString();
    newRes.attributes.__isset.fileName = true;
    newRes.__isset.attributes = true;
    if (mime.startsWith("image", Qt::CaseInsensitive) || mime.endsWith("pdf", Qt::CaseInsensitive))
        newRes.attributes.attachment = false;
    else
        newRes.attributes.attachment = true;
    newRes.attributes.__isset.attachment = true;
    newRes.active = true;
    newRes.__isset.active = true;
    newRes.guid = QString::number(lid).toStdString();
    newRes.__isset.guid = true;
    newRes.noteGuid = noteGuid.toStdString();
    newRes.__isset.noteGuid = true;
    newRes.updateSequenceNum = 0;
    newRes.__isset.updateSequenceNum = 0;
    ResourceTable restable;
    restable.add(lid, newRes, true);

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
        addPaths(dirs);
        addPaths(files);
    } else {
        setupDirectory(files, root);
        addPath(root);
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

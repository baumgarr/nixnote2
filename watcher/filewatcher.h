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

#ifndef FILEWATCHER_H
#define FILEWATCHER_H


#include <QFileSystemWatcher>
#include <QStringList>
#include <QFileInfo>

class FileWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    enum ScanType {
        ImportDelete = 0,
        ImportKeep = 1
    };
    explicit FileWatcher(QString dir, ScanType type, qint32 notebookLid, bool subdirs=true, QObject *parent = 0);

private:
    //void getDirectoryEntries(QStringList fileList, QString dir);
    QString dir;
    ScanType scanType;
    qint32 notebookLid;
    QList<QString> saveFiles;
    bool includeSubdirectories;
    void setupSubDirectories(QStringList &directories, QStringList &files, QString directory);
    void setupDirectory(QStringList &files, QString directory);
    void addDirectory(QString root);


signals:
    void fileImported(qint32 noteLid, qint32 ressourceLid);
    
public slots:
    void saveFile(QString file);
    void saveDirectory(QString dir);
    
};

#endif // FILEWATCHER_H

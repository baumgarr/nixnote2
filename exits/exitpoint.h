/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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


#ifndef EXITPOINT_H
#define EXITPOINT_H

#include <QObject>
#include <QDateTime>

class ExitPoint : public QObject
{
    Q_OBJECT
private:
    QString     exitName;
    QString     fileName;
    QString     engine;
    bool        enabled;
    int         version;
    QString     script;

public:
    explicit ExitPoint(QObject *parent = 0);

    void setExitName(QString);
    QString getExitName();

    void setFileName(QString);
    QString getFileName();

    void setEngine(QString);
    QString getEngine();

    void setEnabled(bool);
    bool getEnabled();

    void setVersion(int);
    int getVersion();

    void setScript(QString);
    QString getScript();

signals:

public slots:
    void log(QString vaule);
};



// Exit parameters used when saving a note in the editor
class ExitPoint_NoteEdit : public ExitPoint
{
    Q_OBJECT
private:
    bool    exitReady;

    QString title;
    bool    title_isSet;
    bool    title_isModified;

    QString contents;
    bool    contents_isSet;
    bool    contents_isModified;
    bool    contents_reload_requested;
    bool    contents_is_dirty;

    QString *notebook;
    bool    notebook_isSet;
    bool    notebook_isModified;

    QStringList  tags;
    bool    tags_isSet;
    bool    tags_isModified;

    QDateTime subjectDate;
    bool      subjectDate_isSet;
    bool      subjectDate_isModified;

    QDateTime updateDate;
    bool      updateDate_isSet;
    bool      updateDate_isModified;

    QDateTime creationDate;
    bool      creationDate_isSet;
    bool      creationDate_isModified;

public:
    explicit ExitPoint_NoteEdit(ExitPoint *parent = 0);
    void setExitReady();

public slots:
    void setTitle(QString);
    QString getTitle();
    bool isTitleModified();

    void setContents(QString);
    QString getContents();
    QString getContentsPlainText();
    bool isContentsModified();
    bool reloadContents();
    void setContentsReload(bool);
    bool isContentsDirty();
    void setContentsDirty(bool);

    void setNotebook(QString);
    QString getNotebook();
    bool isNotebookModified();

    void setTags(QStringList);
    QStringList getTags();
    bool isTagsModified();
    int getTagsSize();
    void insertTag(QString);
    void removeTag(QString);
    QString getTagAt(int);

    QString getCreatedDate(QString format);
    QString getSubjectDate(QString format);
    QString getUpdatedDate(QString format);

    void setUpdatedDate(qint64 msecs);
    void setCreationDate(qint64 msecs);
    void setSubjectDate(qint64 msecs);
};



#endif // EXITPOINT_H

/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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


#ifndef IMPORTENEX_H
#define IMPORTENEX_H

#include <QString>
#include <QXmlStreamReader>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QtXml>
#include <QProgressDialog>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "sql/notemetadata.h"
#include "global.h"

using namespace evernote::edam;
using namespace std;

class ImportEnex : public QObject
{
    Q_OBJECT

private:
    void                        processNoteNode();
    void                        processResource(Resource &resource);
    void                        setNotebookGuid(QString g);
    void                        processData(QString nodeName, Data &data);
    void                        processResourceAttributes(ResourceAttributes &attributes);
    void                        processNoteAttributes(NoteAttributes &attributes);
    QString                     fileName;
    QXmlStreamReader            *reader;
    QString                     notebookGuid;
    QProgressDialog             *progress;

    QHash<QString,QString>          noteMap;
    QHash<QString, NoteMetadata>    metaData;
    QList<QString>                  noteList;
    QHash<QString, QString>         tagList;
    bool                            backup;
    bool                            stopNow;

    QString                         textValue();
    qint32                          intValue();
    long                            longValue();
    qlonglong                       longLongValue();
    qlonglong                       datetimeValue();
    double                          doubleValue();
    bool                            booleanValue();
    short                           shortValue();

public:
    ImportEnex(QObject *parent=0);
    qint32                     lastError;
    QString                     errorMessage;
    bool                        importTags;
    bool                        importNotebooks;
    bool                        createTags;
    void                        import(QString file);
    QString                     getErrorMessage();

public slots:
    void                        canceled();

};

#endif // IMPORTENEX_H

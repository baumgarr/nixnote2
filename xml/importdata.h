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


#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QtXml>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "sql/notemetadata.h"
#include "global.h"

using namespace evernote::edam;
using namespace std;

//***********************************************************
//***********************************************************
//* This class is used to import or restore data from an
//* export file.  Export files typically end in nnex.
//***********************************************************
//***********************************************************

class ImportData
{

private:
    QString                 fileName;
    QXmlStreamReader        *reader;
    bool                    backup;
    QString                 notebookGuid;

    QHash<QString,QString>		noteMap;
    QHash<QString, NoteMetadata> metaData;
    QList<QString>              noteList;

    void processNoteNode();
    void processResource(Resource &resource);
    void processData(QString nodeName, Data &data);
    void processResourceAttributes(ResourceAttributes &attributes);
    void processNoteTagList(QStringList &guids, QStringList &names);
    void processNoteAttributes(NoteAttributes &attributes);
    void processSynchronizationNode();
    void processSavedSearchNode();
    void processLinkedNotebookNode();
    void processSharedNotebookNode();
    void processNotebookNode();
    void processTagNode();
    QString textValue();
    qint32 intValue();
    long longValue();
    qlonglong longLongValue();
    double doubleValue();
    bool booleanValue();
    short shortValue();

public:
    qint32                     lastError;
    QString                 errorMessage;
    bool                    importTags;
    bool                    importNotebooks;
    bool                    createTags;
    ImportData(bool full);
    void import(QString file);
    void setNotebookGuid(QString g);
    QString getErrorMessage();
};

#endif // IMPORTDATA_H

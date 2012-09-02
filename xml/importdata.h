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

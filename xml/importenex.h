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

#ifndef NOTEFORMATTER_H
#define NOTEFORMATTER_H

#include <QtWebKit>
#include <QObject>
#include <QTemporaryFile>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QtXml>


#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "filters/ensearch.h"

using namespace apache::thrift;
using namespace evernote::edam;
using namespace std;


class NoteFormatter : public QObject
{
    Q_OBJECT
private:
    Note note;
    QByteArray content;
    bool pdfPreview;
    QList< QTemporaryFile* > tempFiles;
    EnSearch enSearch;
    bool noteHistory;
    bool formatError;
    void addImageHighlight(qint32 resLid, QFile &f);
    void modifyImageTags(QDomDocument &doc, QDomElement &docElement, QDomElement &enMedia, QDomAttr &hash);
    void modifyApplicationTags(QDomDocument &doc, QDomElement &docElem, QDomElement &enmedia, QDomAttr &hash, QString appl);
    void modifyTodoTags(QDomElement &todo);
    void modifyTags(QDomDocument &doc);
    QString findIcon(qint32 lid, Resource r, QString appl);

public:
    bool resourceError;
    bool readOnly;
    bool inkNote;
    bool enableHighlight;

    explicit NoteFormatter(QObject *parent = 0);
    void setNote(Note n, bool pdfPreview);
    QString getPage();
    void setHighlight(EnSearch search);
    void setNoteHistory(bool value);
    QByteArray rebuildNoteHTML();
    QDomDocument addHighlight(QDomDocument &doc);
    bool  buildInkNote(QDomDocument &doc, QDomElement &docElem, QDomElement &enmedia, QDomAttr &hash, QString appl);


signals:

public slots:

};

#endif // NOTEFORMATTER_H

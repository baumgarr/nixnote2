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
    QStringList highlightWords;
    bool noteHistory;
    bool formatError;
    QString addImageHighlight(qint32 resLid, QString imgfile);
    void modifyImageTags(QWebElement &enMedia, QString &hash);
    void modifyApplicationTags(QWebElement &enmedia, QString &hash, QString appl);
    void modifyPdfTags(qint32 resLid, QWebElement &enmedia);
    void modifyTodoTags(QWebElement &todo);
    void modifyTags(QWebPage &doc);
    QString findIcon(qint32 lid, Resource r, QString appl);
    QString preHtmlFormat(QString content);
    QHash<QString, qint32> hashMap;
    QHash<qint32, Resource> resourceMap;

public:
    bool resourceError;
    bool readOnly;
    bool inkNote;
    bool thumbnail;
    bool enableHighlight;

    explicit NoteFormatter(QObject *parent = 0);
    void setNote(Note n, bool pdfPreview);
    QEventLoop eventLoop;
    QString getPage();
    void setHighlight();
    void setNoteHistory(bool value);
    QByteArray rebuildNoteHTML();
    bool  buildInkNote(QWebElement &docElem, QString &hash);
    void setHighlightText(QString text);


signals:
    void fileIconProviderRequested(QString fileName);

public slots:

};

#endif // NOTEFORMATTER_H

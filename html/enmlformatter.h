#ifndef ENMLFORMATTER_H
#define ENMLFORMATTER_H

#include <QObject>

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


class EnmlFormatter : public QObject
{
    Q_OBJECT
private:
    QByteArray content;
    QDomDocument doc;
    bool isAttributeValid(QString attribute);
    bool isElementValid(QString element);
    void cleanupElementAttributes(QDomElement &e);


public:
    bool formattingError;
    QList<int> resources;

    explicit EnmlFormatter(QObject *parent = 0);
    void setHtml(QString html);
    QString getEnml();
    QByteArray rebuildNoteEnml();
    void parseNodes(const QDomNodeList &nodes);
    void scanTags();
    void fixNode(const QDomNode &node);
    QDomNode fixLinkNode(const QDomNode &node);

signals:

public slots:

};


#endif // ENMLFORMATTER_H

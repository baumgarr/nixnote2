/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#ifndef NOTEFORMATTER_H
#define NOTEFORMATTER_H

#include <QtWebKit>
#include <QWebPage>
#include <QWebFrame>
#include <QObject>
#include <QTemporaryFile>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QtXml>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

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
    bool resourceHighlight;
    const char* findImageFormat(QString file);

public:
    bool resourceError;
    bool readOnly;
    bool inkNote;
    bool thumbnail;
    //bool enableHighlight;

    explicit NoteFormatter(QObject *parent = 0);
    void setNote(Note n, bool pdfPreview);
    QEventLoop eventLoop;
    QString getPage();
    //void setHighlight();
    void setNoteHistory(bool value);
    QByteArray rebuildNoteHTML();
    bool  buildInkNote(QWebElement &docElem, QString &hash);
    void setHighlightText(QString text);


signals:
    void fileIconProviderRequested(QString fileName);

public slots:

};

#endif // NOTEFORMATTER_H

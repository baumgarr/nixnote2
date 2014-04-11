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

using namespace std;


class EnmlFormatter : public QObject
{
    Q_OBJECT
private:
    QByteArray content;
    QDomDocument doc;
    bool isAttributeValid(QString attribute);
    bool isElementValid(QString element);
    void scanTags(QWebElement &element);
    void fixImgNode(QWebElement &element);
    void fixEnCryptNode(QWebElement &element);
    void processTodo(QWebElement &element);
    QStringList findAllTags(QWebElement &element);
    void removeInvalidAttributes(QWebElement &node);
    void fixLinkNode(QWebElement e);
    void fixObjectNode(QWebElement &e);
    void scanTags();
    void fixNode(const QDomNode &node);
    void postXmlFix();
    QByteArray fixEncryptionTags(QByteArray newContent);

public:
    bool formattingError;
    QList<qint32> resources;

    explicit EnmlFormatter(QObject *parent = 0);
    void setHtml(QString html);
    QString getEnml();
    QByteArray rebuildNoteEnml();


signals:

public slots:

};


#endif // ENMLFORMATTER_H

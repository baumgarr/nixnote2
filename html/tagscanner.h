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

#ifndef TAGSCANNER_H
#define TAGSCANNER_H

#include <QObject>
#include <QHash>
#include <stdint.h>


class TagScannerRecord {

public:
    TagScannerRecord(QObject *parent = 0);
    qint32 startPosition;
    qint32 endPosition;
};







class TagScanner : public QObject
{
    Q_OBJECT
private:
    QString content;
    QHash<QString, TagScannerRecord> recTable;

public:
    explicit TagScanner(QObject *parent = 0);
    void setData(QString data);
    qint32 findAll(QList<TagScannerRecord> &recs, QString tagName);

signals:

public slots:

};







#endif // TAGSCANNER_H

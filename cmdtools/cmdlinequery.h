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


#ifndef CMDLINEQUERY_H
#define CMDLINEQUERY_H

#include <QObject>
#include <QTextStream>

class CmdLineQuery : public QObject
{
    Q_OBJECT
private:
    QTextStream *out;
    bool stdout;
    void writeLine(QString line);
    QString lineBuilder(QString value, QString format, int defaultPadding=0, QChar padChar=' ');

public:
    explicit CmdLineQuery(QObject *parent = 0);
    int account;
    QString query;
    QString delimiter;
    QString outputFormat;
    bool printHeaders;
    void write(QList<qint32> lids, QString filename);
    QString wrap();
    void unwrap(QString data);
    int lastError;
    QString errorMessage;
    QString returnUuid;

signals:

public slots:

};

#endif // CMDLINEQUERY_H

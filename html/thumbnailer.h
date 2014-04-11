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


#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtWebKit>
#include <QObject>
#include <QSqlDatabase>

#include "html/noteformatter.h"


#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

using namespace std;


class Thumbnailer : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase *db;
    QTimer timer;
    int minTime;
    int maxTime;


public:
    QWebPage *page;
    Thumbnailer(QSqlDatabase *db);
    ~Thumbnailer();
    void render(qint32 lid);
    qint32 lid;
    bool idle;
    void capturePage(QWebPage *page);
    void startTimer(int minSeconds, int maxSeconds);


signals:

private slots:

public slots:
    void pageReady(bool ok);
    void generateNextThumbnail();

};

#endif // THUMBNAILER_H

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

#include "thumbnailer.h"
#include <QtWebKit>
#include <QtSql>
#include <QTextDocument>
#include "global.h"
#include "sql/notetable.h"

extern Global global;

/* Generic constructor. */
Thumbnailer::Thumbnailer(QSqlDatabase *db)
{
    this->db = db;
    connect(&page, SIGNAL(loadFinished(bool)), this, SLOT(pageReady(bool)));
}


void Thumbnailer::render(qint32 lid, QString contents) {
    this->lid = lid;

    // Start removing object tags
    int pos = contents.indexOf("<object");
    while (pos>=0) {
        int endPos = contents.indexOf(">", pos);
        QString lidString = contents.mid(contents.indexOf("lid=", pos)+5);
        lidString = lidString.mid(0,lidString.indexOf("\" "));
        contents = contents.mid(0,pos) + "<img src=\"file://" +
                global.fileManager.getTmpDirPath() + lidString +
                QString("-print.png\">")+contents.mid(endPos+1);

        pos = contents.indexOf("<object", endPos);
    }
    // Start removing <en-media tags
    pos = contents.indexOf("<en-media");
    while (pos>=0) {
        int endPos = contents.indexOf(">", pos)+1;
        contents = contents.mid(0,pos) + contents.mid(endPos);
        pos = contents.indexOf("<en-media", endPos);
    }

    contents = contents.replace("</object>","");
    contents = contents.replace("</en-media>","");
    QLOG_DEBUG() << contents;

    page.mainFrame()->setContent(contents.toUtf8());
}


void Thumbnailer::pageReady(bool ok) {
//    int minWidth = 600;
//    if (textLen < 600)
//        minWidth = textLen*10;
//    if (contents.indexOf("<img") > 0)
//        minWidth = 10;

    page.mainFrame()->setZoomFactor(3);
    page.setViewportSize(QSize(300,300));
    page.mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    QImage pix(QSize(300,300), QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&pix);
    page.mainFrame()->render(&painter);
    painter.end();
    QString filename = global.fileManager.getThumbnailDirPath()+QString::number(lid) +".png";
    pix.save(filename);
    NoteTable ntable(db);
    ntable.setThumbnail(lid, filename);
}

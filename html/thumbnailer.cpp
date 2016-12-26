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
#include <QWebPage>
#include <QWebFrame>
#include <QtSql>
#include <QTextDocument>
#include "global.h"
#include "sql/notetable.h"

extern Global global;

/* Generic constructor. */
Thumbnailer::Thumbnailer(DatabaseConnection *db)
{
    this->db = db;
    page = new QWebPage();
    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(pageReady(bool)));
    idle = true;
    connect(&timer, SIGNAL(timeout()), this, SLOT(generateNextThumbnail()));
}

Thumbnailer::~Thumbnailer() {
    delete page;
}

void Thumbnailer::render(qint32 lid) {
    idle = false;
    this->lid = lid;

    NoteFormatter formatter;
    formatter.thumbnail = true;
    NoteTable noteTable(db);
    Note n;
    noteTable.get(n,lid,false, false);
    formatter.setNote(n,false);
    QByteArray formattedContent = formatter.rebuildNoteHTML();
// Windows check for font bug when viewing Unicode characters
#ifdef _WIN32
    for(int i = 0; i < formattedContent.size(); ++i) {
        char c = formattedContent.at(i);
        QChar cc(c);
        if(cc.unicode() > 127) {
            c  = ' ';
            formattedContent.replace(c,' ');
        }
    }
#endif
    page->mainFrame()->setContent(formattedContent);
}



void Thumbnailer::startTimer() {
    timer.stop();
    timer.start(global.minimumThumbnailInterval*1000);
}



void Thumbnailer::pageReady(bool ok) {
    if (ok) {
        capturePage(page);
    }
    NoteTable ntable(db);
    ntable.setThumbnailNeeded(lid, false);
    idle = true;
}


void Thumbnailer::capturePage(QWebPage *page) {
    page->mainFrame()->setZoomFactor(3);
    page->setViewportSize(QSize(300,300));
    page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    QImage pix(QSize(300,300), QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&pix);
    page->mainFrame()->render(&painter);
    painter.end();
    QString filename = global.fileManager.getThumbnailDirPath()+QString::number(lid) +".png";
    pix.save(filename);
    NoteTable ntable(db);
    ntable.setThumbnail(lid, filename);
}


void Thumbnailer::generateNextThumbnail() {
    // If we are connected we are downloading or uploading or
    // if we have thumbnails disabled, so
    // we don't want to do this now.
    if (global.connected || global.disableThumbnails) {
        timer.start(global.maximumThumbnailInterval*1000);
        return;
    }

    timer.stop();
    NoteTable noteTable(db);
    int i=0;
    for (; i<global.batchThumbnailCount; i++) {
        QDateTime current;
        QDateTime deadlineTime;
        deadlineTime.addSecs(20);

        // Make sure we haven't been looping too long.
        if (current > deadlineTime) {
            QLOG_DEBUG() << "Thumbnail timer exceeded. Going to seep";
            timer.start(global.maximumThumbnailInterval*1000);
            return;
        }

        if (this->idle) {
            i++;
            qint32 lid = noteTable.getNextThumbnailNeeded();
            if (lid>0) {
                render(lid);
            } else {
                timer.start(global.maximumThumbnailInterval*1000);
                return;
            }
        }
    }
    timer.start(global.minimumThumbnailInterval*1000);
}

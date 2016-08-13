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
    minTime = 5;
    maxTime = 60;
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
    page->mainFrame()->setContent(formattedContent);
}



void Thumbnailer::startTimer(int minSeconds, int maxSeconds) {
    timer.stop();
    minTime = minSeconds*1000;
    maxTime = maxSeconds*1000;
    timer.start(minTime);
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
    // If we are connected we are downloading or uploading, so
    // we don't want to do this now.
    if (global.connected) {
        timer.start(maxTime);
        return;
    }

    timer.stop();
    NoteTable noteTable(db);
    qint32 lid = noteTable.getNextThumbnailNeeded();
    if (lid>0) {
        render(lid);
        timer.start(minTime);
    } else
        timer.start(maxTime);
}

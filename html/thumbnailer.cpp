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
Thumbnailer::Thumbnailer()
{
}


/* Set the note we should generate the thumbnail for */
void Thumbnailer::setNote(Note n) {
    formatter.setNote(n, true);
    formatter.thumbnail = true;
    formatter.rebuildNoteHTML();
    contents = formatter.getPage();
}



void Thumbnailer::render(qint32 lid, QString contents) {

    QFile f(global.fileManager.getTmpDirPath()+QString::number(lid)+QString(".html"));
    QTextStream outstream(&f);
    f.open(QIODevice::WriteOnly);
    outstream << contents;
    f.close();

    QTextDocument textDocument;
    textDocument.setHtml(contents);
    int textLen = textDocument.toPlainText().length();


    int minWidth = 600;
    if (contents.indexOf("<img") > 0)
        minWidth = 10;
    if (textLen < 600)
        minWidth = textLen*10;

    QString args;
    QString outfile = global.fileManager.getThumbnailDirPath() +QString::number(lid) +QString(".png");
    args = QString(" --out=") +outfile+
            QString(" --url=file://") +global.fileManager.getTmpDirPath() +QString::number(lid) +QString(".html ") +
            QString("--min-height=10 --min-width=")   +QString::number(minWidth);
    imgProcess.start(QString("cutycapt")+ args, QIODevice::ReadWrite|QIODevice::Unbuffered);
    imgProcess.waitForStarted();
    imgProcess.waitForFinished();

    NoteTable noteTable;
    noteTable.setThumbnail(lid, outfile);

}

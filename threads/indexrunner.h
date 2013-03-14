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

#ifndef INDEXRUNNER_H
#define INDEXRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>


#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

#include <html/thumbnailer.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <QFileInfo>
#include <QTimer>

using namespace evernote::edam;
using namespace std;

class IndexRunner : public QThread
{
    Q_OBJECT
private:
    QTimer *indexTimer;
    void indexRecognition(qint32 lid, Resource &r);
    void indexNote(qint32 lid, Note &n);
    void indexPdf(qint32 lid, Resource &r);
    QTextDocument *textDocument;
    Thumbnailer *hammer;

public:
    bool keepRunning;
    bool pauseIndexing;
    IndexRunner();
    ~IndexRunner();
    void run();

signals:
    void thumbnailNeeded(qint32);

 public slots:
    void index();

};

#endif // INDEXRUNNER_H

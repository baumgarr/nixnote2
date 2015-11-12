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


#ifndef NOTEINDEXER_H
#define NOTEINDEXER_H


#include <QString>
#include "sql/databaseconnection.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <QFileInfo>
#include <QTextDocument>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

using namespace std;


class NoteIndexer
{
public:
    NoteIndexer();
    void indexNote(qint32 lid);
    void addTextIndex(qint32 lid, QString content);
    void indexResource(qint32 lid, DatabaseConnection *db);
    void indexRecognition(qint32 reslid, Resource &r);
    void indexPdf(qint32 reslid);
};

#endif // NOTEINDEXER_H

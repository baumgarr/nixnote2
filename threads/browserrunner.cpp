/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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


#include "browserrunner.h"


#include "global.h"
#include "utilities/nuuid.h"
#include "utilities/noteindexer.h"
#include "sql/notetable.h"

extern Global global;

BrowserRunner::BrowserRunner(QObject *parent) : QObject(parent)
{
    isIdle = true;
    init = false;
}



void BrowserRunner::initialize() {
    init = true;

    QLOG_DEBUG() << "Starting CounterRunner";
    db = new DatabaseConnection("browserrunner-"+NUuid().create());
    QLOG_DEBUG() << "CounterRunner initialization complete.";
}


void BrowserRunner::updateNoteContent(qint32 lid, QString content, bool isDirty) {
    isIdle = false;

    if (!init)
        initialize();

    QLOG_DEBUG() << "Updating note content";
    NoteTable table(db);
    table.updateNoteContent(lid, content, isDirty);

    isIdle = true;
}


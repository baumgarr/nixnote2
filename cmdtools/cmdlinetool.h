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



#ifndef CMDLINETOOL_H
#define CMDLINETOOL_H

#include <QObject>
#include <QSharedMemory>

#include "settings/startupconfig.h"

class CmdLineTool : public QObject
{
    Q_OBJECT
public:
    explicit CmdLineTool(QObject *parent = 0);
    int run(StartupConfig &config);
    int readNote(StartupConfig config);
    int addNote(StartupConfig config);
    int appendNote(StartupConfig config);
    int queryNotes(StartupConfig config);
    int deleteNote(StartupConfig config);
    int emailNote(StartupConfig config);
    int exportNotes(StartupConfig config);
    int alterNote(StartupConfig config);
    int importNotes(StartupConfig config);
    int openNotebook(StartupConfig config);
    int closeNotebook(StartupConfig config);
    int sync(StartupConfig config);
    int signalGui(StartupConfig config);

signals:

public slots:

};

#endif // CMDLINETOOL_H

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

#ifndef STARTUPCONFIG_H
#define STARTUPCONFIG_H

#include <QString>
#include <QHash>
#include <QIcon>
#include <QPixmap>
#include <QBitArray>

#include "cmdtools/addnote.h"
#include "cmdtools/cmdlinequery.h"
#include "cmdtools/deletenote.h"
#include "cmdtools/emailnote.h"
#include "cmdtools/extractnotetext.h"
#include "cmdtools/extractnotes.h"
#include "cmdtools/alternote.h"
#include "cmdtools/importnotes.h"
#include "cmdtools/signalgui.h"

#define STARTUP_GUI 0
#define STARTUP_SYNC 1
#define STARTUP_SHUTDOWN 2
#define STARTUP_SHOW 3
#define STARTUP_ADDNOTE 4
#define STARTUP_QUERY 5
#define STARTUP_DELETENOTE 6
#define STARTUP_EMAILNOTE 7
#define STARTUP_ALTERNOTE 8
#define STARTUP_EXPORT 9
#define STARTUP_IMPORT 10
#define STARTUP_BACKUP 11
#define STARTUP_READNOTE 12
#define STARTUP_CLOSENOTEBOOK 13
#define STARTUP_OPENNOTEBOOK 14
#define STARTUP_APPENDNOTE 15
#define STARTUP_SQLEXEC 16
#define STARTUP_SIGNALGUI 17
#define STARTUP_OPTION_COUNT 18

class StartupConfig
{
private:
    void loadTheme(QString theme);
private:
    QBitArray *command;

public:
    StartupConfig();
    QString name;
    QString homeDirPath;
    QString programDirPath;
    QString queryString;
    bool forceNoStartMinimized;
    bool startupNewNote;
    bool sqlExec;
    int accountId;
    qint32 startupNoteLid;
    bool forceStartMinimized;
    bool enableIndexing;
    bool forceSystemTrayAvailable;
    bool disableEditing;
    bool purgeTemporaryFiles;
    AddNote *newNote;
    SignalGui *signalGui;
    DeleteNote *delNote;
    EmailNote *email;
    CmdLineQuery *queryNotes;
    ExtractNoteText *extractText;
    ExtractNotes *exportNotes;
    ImportNotes *importNotes;
    AlterNote *alter;
    bool gui();
    bool sync();
    bool addNote();
    bool appendNote();
    bool show();
    bool shutdown();
    bool query();
    bool deleteNote();
    bool emailNote();
    bool readNote();
    bool exports();
    bool backup();
    bool alterNote();
    bool openNotebook();
    bool closeNotebook();
    bool import();
    bool signalOtherGui();
    QString sqlString;
    QStringList notebookList;

    int init(int argc, char *argv[], bool &guiAvailable);
    void printHelp();
};

#endif // STARTUPCONFIG_H

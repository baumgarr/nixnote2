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

#include "startupconfig.h"
#include <QDir>
#include <QString>
#include <iostream>

//extern Global global;


StartupConfig::StartupConfig()
{
    homeDirPath = QDir().homePath() + QString("/.nixnote/");
    this->forceNoStartMinimized = false;
    this->syncAndExit = false;
    this->startupNewNote = false;
    this->forceStartMinimized = false;
    this->enableIndexing = false;
    this->startupNoteLid = 0;
    this->forceSystemTrayAvailable=false;
    this->disableEditing = false;
    this->accountId=-1;
    command = new QBitArray(STARTUP_OPTION_COUNT);
    command->fill(false);
    newNote = NULL;
    queryNotes = NULL;
    purgeTemporaryFiles=true;
    delNote = NULL;
    email = NULL;
    extractText = NULL;
}



// Print out user help
void StartupConfig::printHelp() {
    QString help = QString("usage: nixnote2 <command>\n"
                   +QString("  <command> options:\n\n")
                   +QString("  help or ? or --? or --help           Show this message\n")
                   +QString("  start <options>                      Start NixNote GUI with the specified options.\n")
                   +QString("                                       If no command is specified, this is the default.\n")
                   +QString("     start options:\n")
                   +QString("          --accountId=<id>             Start with specified user account\n")
                   +QString("          --dontStartMinimized         Override option to start minimized\n")
                   +QString("          --disableEditing             Disable note editing\n")
                   +QString("          --enableIndexing             Enable background Indexing (can cause problems)\n")
                   +QString("          --openNote=<lid>             Open a specific note on startup\n")
                   +QString("          --forceSystemTrayAvailable   Force the program to accept that\n")
                   +QString("                                       the desktop supports tray icons.\n")
                   +QString("          --startMinimized             Force a startup with NixNote minimized\n")
                   +QString("          --syncAndExit                Synchronize and exit the program.\n")
                   +QString("  sync                                 If running, ask NixNote to synchronize with Evernote\n")
                   +QString("  shutdown                             If running, ask NixNote to shutdown\n")
                   +QString("  show_window                          If running, ask NixNote to show the main window.\n")
                   +QString("  query <options>                      If running, search NixNote and display the results.\n")
                   +QString("     query options:\n")
                   +QString("          --accountId=<id>             Account number (defaults to last used account).\n")
                   +QString("          --search=\"search string\"     Search string.\n\n")
                   +QString("          --delimiter=\"character\"      Character to place between fields.  Defaults to |.\n")
                   +QString("          --noHeaders                  Do not show column headings.")
                   +QString("          --display=\"<output format>\"  Search string.\n\n")
                   +QString("             Output Format: <fieldID><padding><:>\n")
                   +QString("                %i                     Show the internal note ID.\n")
                   +QString("                %t                     Show the note title.\n")
                   +QString("                %n                     Show the notebook.\n")
                   +QString("                %g                     Show tags.\n")
                   +QString("                %c                     Show the date the note was created.\n")
                   +QString("                %u                     Show the last date updated.\n")
                   +QString("                %e                     Show if is synchronized with Evernote.\n")
                   +QString("                %s                     Show the source URL.\n")
                   +QString("                %a                     Show the author.\n")
                   +QString("                %x                     Show if the note has a todo item.\n")
                   +QString("                %r                     Show the reminder time.\n")
                   +QString("                %v                     Show the time the reminder was completed.\n")
                   +QString("                <padding>              Pad the field to this number of spaces on the display.\n")
                   +QString("                <:>                    Truncate the field if longer than the padding.\n")
                   +QString("  addNote <options>                    Add a new note via the command line.\n")
                   +QString("     addNote options:\n")
                   +QString("          --title=\"<title>\"            Title of the new note.\n")
                   +QString("          --notebook=\"<notebook>\"      Notebook for the new note.\n")
                   +QString("          --tag=\"<tag>\"                Assign a tag.\n")
                   +QString("                                       For multiple tags use multiple --tag statements.\n")
                   +QString("          --attachment=\"<file_path>\"   File to attach to the note.\n")
                   +QString("                                       For multiple files, use multiple --attachment statements.\n")
                   +QString("          --delimiter=\"<delmiiter>\"    Character string identifying attachment points.\n")
                   +QString("                                       Defaults to %%.\n")
                   +QString("          --created=\"<date_created>\"   Date created\n")
                   +QString("          --updated=\"<date_updated>\"   Date created\n")
                   +QString("          --noteText=\"<text>\"          Text of the note.  If not provided input\n")
                   +QString("                                       is read from stdin.\n")
                   +QString("          --accountId=<id>             Account number (defaults to last used account).\n")
                   +QString("  readNote <options>                   Read the text contents of a note.\n")
                   +QString("          --id=\"<note_id>\"             ID of the note to read.\n")
                   +QString("          --accountId=<id>             Account number (defaults to last used account).\n")
                   +QString("  deleteNote <options>                 Move a note to the trash via the command line.\n")
                   +QString("     deleteNote options:\n")
                   +QString("          --id=\"<note_id>\"             ID of the note to delete.\n")
                   +QString("          --noVerify                   Do not prompt for verification.\n")
                   +QString("          --accountId=<id>             Account number (defaults to last used account).\n")
                   +QString("  emailNote <options>                  Move a note to the trash via the command line.\n")
                   +QString("     emailNote options:\n")
                   +QString("          --id=\"<note_id>\"             ID of the note to email.\n")
                   +QString("          --subject=\"<subject>\"        Additional comments.\n")
                   +QString("          --to=\"<address list>\"        List of recipients for the email.\n")
                   +QString("          --cc=\"<address list>\"        List of recipients to carbon copy.\n")
                   +QString("          --bcc=\"<address list>\"       List of recipients to blind carbon copy.\n")
                   +QString("          --note=\"<note>.\"             Additional comments.\n")
                   +QString("          --ccSelf                     Send a copy to yourself.\n")
                   +QString("          --accountId=<id>             Account number (defaults to last used account).\n\n")
                   +QString("  Examples:\n\n")
                   +QString("     To Start NixNote, do a sync, and then exit.\n")
                   +QString("     nixnote2 start --syncAndExit\n\n")
                   +QString("     To start NixNote using a secondary account.\n")
                   +QString("     nixnote2 --accountId=2\n\n")
                   +QString("     To add a note to the notebook \"My Notebook\"\n")
                   +QString("     nixnote2 addNote --notebook=\"My Stuff\" --title=\"My New Note\" --tag=\"Tag1\" --tag=\"Tag2\" --noteText=\"My Note Text\"\n\n")
                   +QString("     Query notes for the search text. Results show the ID, note title (padded to 10 characters but truncated longer) and the notebook\n")
                   +QString("     nixnote2 query --search=\"Famous Authors\" --delimiter=\" * \" --display=\"\%i%t10:%n\"\n")
                   +QString("\n\n")
                   );

    std::cout << help.toStdString();
}




int StartupConfig::init(int argc, char *argv[]) {

    for (int i=1; i<argc; i++) {
        QString parm(argv[i]);
        if (parm == "--help" || parm == "-?" || parm == "help" || parm == "--?") {
            printHelp();
            return 1;
        }
        if (parm.startsWith("addNote")) {
            command->setBit(STARTUP_ADDNOTE,true);
            if (newNote == NULL)
                newNote = new AddNote();
        }
        if (parm.startsWith("emailNote")) {
            command->setBit(STARTUP_EMAILNOTE,true);
            if (email == NULL)
                email = new EmailNote();
        }
        if (parm.startsWith("query")) {
            command->setBit(STARTUP_QUERY);
            if (queryNotes == NULL)
                queryNotes = new CmdLineQuery();
        }
        if (parm.startsWith("readNote")) {
            command->setBit(STARTUP_READNOTE);
            if (extractText == NULL)
                extractText = new ExtractNoteText();
        }
        if (parm.startsWith("deleteNote")) {
            command->setBit(STARTUP_DELETENOTE);
            if (delNote == NULL)
                delNote = new DeleteNote();
        }
        if (parm.startsWith("sync")) {
            command->setBit(STARTUP_SYNC,true);
        }
        if (parm.startsWith("show_window")) {
            command->setBit(STARTUP_SHOW,true);
        }
        if (parm.startsWith("shutdown")) {
            command->setBit(STARTUP_SHUTDOWN,true);
        }
        if (command->at(STARTUP_ADDNOTE)) {
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                accountId = parm.toInt();
            }
            if (parm.startsWith("--title=", Qt::CaseSensitive)) {
                parm = parm.mid(8);
                newNote->title = parm;
            }
            if (parm.startsWith("--notebook=", Qt::CaseSensitive)) {
                parm = parm.mid(11);
                newNote->notebook = parm;
            }
            if (parm.startsWith("--tag=", Qt::CaseSensitive)) {
                parm = parm.mid(6);
                newNote->tags.append(parm);
            }
            if (parm.startsWith("--attachment=", Qt::CaseSensitive)) {
                parm = parm.mid(13);
                newNote->attachments.append(parm);
            }
            if (parm.startsWith("--created=", Qt::CaseSensitive)) {
                parm = parm.mid(10);
                newNote->created = parm;
            }
            if (parm.startsWith("--updated=", Qt::CaseSensitive)) {
                parm = parm.mid(10);
                newNote->updated = parm;
            }
            if (parm.startsWith("--noteText=", Qt::CaseSensitive)) {
                parm = parm.mid(11);
                newNote->content = parm;
            }
        }
        if (command->at(STARTUP_QUERY)) {
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                this->accountId=parm.toInt();
            }
            if (parm.startsWith("--search=", Qt::CaseSensitive)) {
                parm = parm.mid(9);
                queryNotes->query = parm;
            }
            if (parm.startsWith("--display=", Qt::CaseSensitive)) {
                parm = parm.mid(10);
                queryNotes->outputFormat = parm;
            }
            if (parm.startsWith("--delimiter=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                queryNotes->delimiter = parm;
            }
            if (parm.startsWith("--noHeaders", Qt::CaseSensitive)) {
                queryNotes->printHeaders=false;
            }
        }

        // This should be last because it is the default
        if (parm.startsWith("start")) {
            command->setBit(STARTUP_GUI,true);
        }
        if (command->at(STARTUP_GUI) || command->count(true) == 0) {
            command->setBit(STARTUP_GUI,true);
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                accountId = parm.toInt();
            }
            if (parm.startsWith("--openNote=", Qt::CaseSensitive)) {
                parm = parm.mid(11);
                startupNoteLid = parm.toInt();
            }
            if (parm == "--disableEditing") {
                disableEditing = true;
            }
            if (parm == "--dontStartMinimized") {
                forceNoStartMinimized = true;
            }
            if (parm == "--startMinimized") {
                forceStartMinimized = true;
            }
            if (parm == "--newNote") {
                startupNewNote = true;
            }
            if (parm == "--syncAndExit") {
                syncAndExit = true;
            }
            if (parm == "--enableIndexing") {
                enableIndexing = true;
            }
            if (parm == "--forceSystemTrayAvailable") {
                forceSystemTrayAvailable = true;
            }
        }
        if (command->at(STARTUP_DELETENOTE)) {
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                accountId = parm.toInt();
            }
            if (parm == "--noVerify") {
                delNote->verifyDelete = false;
            }
            if (parm.startsWith("--id=", Qt::CaseSensitive)) {
                parm = parm.mid(5);
                delNote->lid = parm.toInt();
            }
        }
        if (command->at(STARTUP_READNOTE)) {
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                accountId = parm.toInt();
            }
            if (parm.startsWith("--id=", Qt::CaseSensitive)) {
                parm = parm.mid(5);
                extractText->lid = parm.toInt();
            }
        }
        if (command->at(STARTUP_EMAILNOTE)) {
            if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
                parm = parm.mid(12);
                accountId = parm.toInt();
            }
            if (parm == "--ccSelf") {
                email->ccSelf = true;
            }
            if (parm.startsWith("--to=", Qt::CaseSensitive)) {
                parm = parm.mid(5);
                email->to = parm;
            }
            if (parm.startsWith("--cc=", Qt::CaseSensitive)) {
                parm = parm.mid(5);
                email->cc = parm;
            }
            if (parm.startsWith("--bcc=", Qt::CaseSensitive)) {
                parm = parm.mid(6);
                email->bcc = parm;
            }
            if (parm.startsWith("--note=", Qt::CaseSensitive)) {
                parm = parm.mid(7);
                email->note = parm;
            }
            if (parm.startsWith("--subject=", Qt::CaseSensitive)) {
                parm = parm.mid(10);
                email->subject = parm;
            }
            if (parm.startsWith("--id=", Qt::CaseSensitive)) {
                parm = parm.mid(5);
                email->lid = parm.toInt();
            }
        }
    }

    if (command->count(true) == 0)
        command->setBit(STARTUP_GUI,true);

    if (command->count(true) > 1) {
        std::cout << "\nInvalid options specified.  Only one command may be specified at a time.\n";
        return 16;
    }
    return 0;
}

void StartupConfig::setSyncAndExit() {
    syncAndExit=true;
    command->clear();
    command->setBit(STARTUP_GUI);
}

bool StartupConfig::query() {
    return command->at(STARTUP_QUERY);
}

bool StartupConfig::gui() {
    return command->at(STARTUP_GUI);
}

bool StartupConfig::sync() {
    return command->at(STARTUP_SYNC);
}

bool StartupConfig::addNote() {
    return command->at(STARTUP_ADDNOTE);
}

bool StartupConfig::show() {
    return command->at(STARTUP_SHOW);
}

bool StartupConfig::shutdown() {
    return command->at(STARTUP_SHUTDOWN);
}

bool StartupConfig::deleteNote() {
    return command->at(STARTUP_DELETENOTE);
}


bool StartupConfig::readNote() {
    return command->at(STARTUP_READNOTE);
}

bool StartupConfig::emailNote() {
    return command->at(STARTUP_EMAILNOTE);
}

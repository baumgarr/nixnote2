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



#include "cmdlinetool.h"
#include "global.h"
#include <iostream>
#include <unistd.h>
#include "html/enmlformatter.h"
#include "utilities/crossmemorymapper.h"
#include "filters/filtercriteria.h"
#include "filters/filterengine.h"
#include "sql/notebooktable.h"
#include "sql/tagtable.h"
#include "sql/notetable.h"
#include "utilities/nuuid.h"
#include "email/smtpclient.h"
#include "utilities/mimereference.h"
#include "threads/syncrunner.h"


extern Global global;

CmdLineTool::CmdLineTool(QObject *parent) :
    QObject(parent)
{
}

// Run the command line request.
int CmdLineTool::run(StartupConfig &config) {
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    // Force info level messages only
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::InfoLevel);

    QString errmsg(tr("Unable to attach to shared memory segment.  Is the other NixNote running?\n"));
    if (config.sync()) {
        // If the shared memory segment doesn't exist, we just do a sync & exit
        if (!global.sharedMemory->attach()) {
            return this->sync();
        }
        global.sharedMemory->write(QString("SNCHRONIZE"));
        global.sharedMemory->detach();
        return 1;
    }
    if (config.shutdown()) {
        if (!global.sharedMemory->attach()) {
            std::cout << errmsg.toStdString();
            return 16;
        }
        global.sharedMemory->write(QString("IMMEDIATE_SHUTDOWN"));
        global.sharedMemory->detach();
        return 1;
    }
    if (config.show()) {
        if (!global.sharedMemory->attach()) {
            std::cout << errmsg.toStdString();
            return 16;
        }
        global.sharedMemory->write(QString("SHOW_WINDOW"));
        global.sharedMemory->detach();
        return 1;
    }


    if (config.addNote()) {
        return addNote(config);
    }

    if (config.appendNote()) {
        return appendNote(config);
    }

    if (config.query()) {
        return queryNotes(config);
    }
    if (config.deleteNote()) {
        return deleteNote(config);
    }
    if (config.emailNote()) {
        return emailNote(config);
    }
    if (config.readNote()) {
        return readNote(config);
    }
    if (config.exports() || config.backup()) {
        return exportNotes(config);
    }
    if (config.import()) {
        return importNotes(config);
    }
    if (config.alterNote()) {
        return alterNote(config);
    }
    if (config.openNotebook()) {
        return openNotebook(config);
    }
    if (config.closeNotebook()) {
        return closeNotebook(config);
    }
    if (config.signalOtherGui()) {
        return signalGui(config);
    }
    return 0;
}



// Email a note via the command line.
int CmdLineTool::emailNote(StartupConfig config) {
    // Look to see if another NixNote is running.  If so, then we
    // expect a response if the note was delete.  Otherwise, we
    // do it ourself.
    bool useCrossMemory = true;
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        useCrossMemory = false;
    }
    if (useCrossMemory) {
        global.sharedMemory->write("EMAIL_NOTE:" + config.email->wrap());
    } else {
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        return config.email->sendEmail();
    }
    return 0;
}


// Delete a note via the command line
int CmdLineTool::deleteNote(StartupConfig config) {
    bool useCrossMemory = true;

    if (config.delNote->verifyDelete) {
        std::string verify;
        std::cout << QString(tr("Type DELETE to verify: ")).toStdString();
        std::cin >> verify;
        QString qVerify = QString::fromStdString(verify);
        if (qVerify.toLower() != "delete")
            return 16;
    }

    // Look to see if another NixNote is running.  If so, then we
    // expect a response if the note was delete.  Otherwise, we
    // do it ourself.
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        useCrossMemory = false;
    }
    if (useCrossMemory) {
        global.sharedMemory->write("DELETE_NOTE:" + QString::number(config.delNote->lid));
    } else {
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        NoteTable noteTable(global.db);
        noteTable.deleteNote(config.delNote->lid,true);
    }
    return 0;
}


// Query notes via the command line
int CmdLineTool::queryNotes(StartupConfig config) {
    bool expectResponse = true;

    // Look to see if another NixNote is running.  If so, then we
    // expect a response of the LID created.  First we detach it so
    // we are not talking to ourselves.
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        expectResponse = false;
    }
    if (expectResponse) {
        NUuid uuid;
        config.queryNotes->returnUuid = uuid.create();
        QString queryString = config.queryNotes->wrap();
        global.sharedMemory->write("CMDLINE_QUERY:" + queryString);

        int cnt = 0;
        int maxWait=10;
        QString tmpFile = global.fileManager.getTmpDirPath()+config.queryNotes->returnUuid+".txt";
        QFile responseFile(tmpFile);
        while (!responseFile.exists() && cnt<maxWait) {
            sleep(1);
            cnt++;
        }
        bool goodResponse = false;
        if (responseFile.exists()) {
            if (responseFile.open(QIODevice::ReadOnly)) {
               QTextStream in(&responseFile);
               while (!in.atEnd()) {
                  QString line = in.readLine();
                  std::cout << line.toStdString() << std::endl;
               }
               responseFile.close();
               goodResponse = true;
            }
        }
        if (!goodResponse)
            std::cout << QString(tr("No response received from NixNote.")).toStdString() << std::endl;
    } else {
        // The other NixNote isn't found, so we do the query ourself
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        FilterCriteria *filter = new FilterCriteria();
        global.filterCriteria.append(filter);
        global.filterPosition = 0;
        FilterEngine engine;
        filter->setSearchString(config.queryNotes->query);
        QList<qint32> lids;
        engine.filter(filter, &lids);
        config.queryNotes->write(lids, "");
    }
    return 0;
}


// Add a note via the command line.  if Nixnote is running,
// the new note is copied into the dbi directory and
// auto-imported.  If it is not running it is created directly.
int CmdLineTool::addNote(StartupConfig config) {

// Windows Check
#ifndef _WIN32
#ifdef Q_OS_WIN32
    _setmode(_fileno(stdin), _O_BINARY);
#endif
#endif // End windows check

    // If we are reding stdin
    if (config.newNote->content == "")  {
        QByteArray content;

        while(!std::cin.eof()) {
            char arr[1024];
            std::cin.read(arr,sizeof(arr));
            int s = std::cin.gcount();
            content.append(arr,s);
            content.append("<br>");
            content.replace("\n","<br>");
        }
        config.newNote->content = QString::fromAscii(content);
    }

    if (!config.newNote->content.contains("<body")) {
        config.newNote->content.prepend("<body>");
    }
    if (!config.newNote->content.contains("</body")) {
        config.newNote->content.append("</body>");
    }


    EnmlFormatter formatter;
    formatter.setHtml(config.newNote->content);
    config.newNote->content = formatter.rebuildNoteEnml();

    bool expectResponse = true;

    // Look to see if another NixNote is running.  If so, then we
    // expect a response of the LID created.  First we detach it so
    // we are not talking to ourselves.
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        expectResponse = false;
    } else {
        global.sharedMemory->detach();
    }
    if (expectResponse) {
        NUuid uuid;
        QString id = uuid.create();

        // Setup cross memory for response
        CrossMemoryMapper crossMemory(id);
        if (!crossMemory.allocate(512))
            expectResponse = false;

        // Write out the segment
        config.newNote->write(id);
        // Start checking for the response
        int cnt = 0;
        qint32 newLid = -1;
        int maxWait = 5;
        while (expectResponse && cnt<maxWait) {
            QByteArray data = crossMemory.read();
            if (!data.startsWith('\0')) {
                expectResponse = false;
                newLid = data.toInt();
            } else {
                sleep(1);
            }
            cnt++;
        }
        if (newLid > 0) {
            std::cout << newLid << QString(tr(" has been created.\n")).toStdString();
            return newLid;
        } else {
            std::cout << QString(tr("No response from NixNote.  Please verify that the note was created.\n")).toStdString();
        }
    } else {
       // Another NN isn't found, so we do this ourself
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        NUuid uuid;
        Note newNote;
        newNote.content = config.newNote->content;
        newNote.active = true;
        newNote.created = QDateTime::currentMSecsSinceEpoch();
        newNote.guid = uuid.create();
        if (config.newNote->title != "")
            newNote.title = config.newNote->title;
        else
            newNote.title = tr("Untitled Note");

        // Process tags
        newNote.tagGuids = QList<Guid>();
        newNote.tagNames=QStringList();
        for (int i=0; i<config.newNote->tags.size(); i++) {
            QString tagName = config.newNote->tags[i];
            TagTable tagTable(global.db);
            qint32 tagLid = tagTable.findByName(tagName, 0);
            QString tagGuid;

            // Do we need to add the tag?
            if (tagLid == 0) {
                Tag tag;
                tag.name = tagName;
                NUuid uuid;
                tagGuid = uuid.create();
                tag.guid = tagGuid;
                tagTable.add(0, tag, true, 0);
            } else {
                tagTable.getGuid(tagGuid, tagLid);
            }
            newNote.tagNames->append(tagName);
            newNote.tagNames->append(tagGuid);
        }

        // Process the notebook
        if (config.newNote->notebook != "") {
            QString notebookName = config.newNote->notebook;
            NotebookTable notebookTable(global.db);
            qint32 lid = notebookTable.findByName(notebookName);
            QString notebookGuid;

            // Do we need to add the notebook?
            if (lid == 0) {
                Notebook book;
                book.name = notebookName;
                NUuid uuid;
                QString newGuid = uuid.create();
                book.guid = newGuid;
                notebookGuid = newGuid;
                lid = notebookTable.add(0, book, true, false);
            } else {
                notebookTable.getGuid(notebookGuid, lid);
            }
            newNote.notebookGuid = notebookGuid;
        } else {
            NotebookTable notebookTable(global.db);
            newNote.notebookGuid = notebookTable.getDefaultNotebookGuid();
        }

        // Do the dates
        if (config.newNote->created != "") {
            QString dateString = config.newNote->created;
            QDateTime date = QDateTime::fromString(dateString.trimmed(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
            newNote.created = date.toMSecsSinceEpoch();
        }
        if (config.newNote->updated != "") {
            QString dateString = config.newNote->updated;
            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss.zzzZ");
            newNote.updated = date.toMSecsSinceEpoch();
        }
        if (config.newNote->reminder != "") {
            QString dateString = config.newNote->reminder;
            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss.zzzZ");
            if (date > QDateTime::currentDateTime()) {
                if (!newNote.attributes.isSet()) {
                    NoteAttributes na;
                    newNote.attributes = na;
                }
                newNote.attributes->reminderTime = date.toMSecsSinceEpoch();
            }
        }


        NoteTable noteTable(global.db);
        qint32 newLid = noteTable.addStub(newNote.guid);
        // Do the attachments
        for (int i=0; i<config.newNote->attachments.size(); i++) {
            QString filename = config.newNote->attachments[i];
            QFile file(filename);
            if (file.exists()) {

                file.open(QIODevice::ReadOnly);
                QByteArray ba = file.readAll();
                file.close();

                MimeReference mimeRef;
                QString extension = filename;
                int endPos = filename.lastIndexOf(".");
                if (endPos != -1)
                    extension = extension.mid(endPos);
                QString mime =  mimeRef.getMimeFromExtension(extension);
                Resource newRes;
                bool attachment = true;
                if (mime == "application/pdf" || mime.startsWith("image/"))
                    attachment = false;
                config.newNote->createResource(newRes, 0, ba, mime, attachment, QFileInfo(filename).fileName(), newLid);
                QByteArray hash;
                if (newRes.data.isSet()) {
                    Data d = newRes.data;
                    if (d.bodyHash.isSet())
                        hash = d.bodyHash;
                }
                if (!newNote.resources.isSet()) {
                    newNote.resources = QList<Resource>();
                }
                QString mediaString = "<en-media hash=\""+hash.toHex()+"\" type=\""+mime+"\"/>";
                if (newNote.content->contains(config.newNote->attachmentDelimiter)) {
                     //newNote.content = newNote.content->replace(config.newNote->attachmentDelimiter,mediaString);
                     newNote.content = newNote.content->replace(newNote.content->indexOf(config.newNote->attachmentDelimiter),
                                                         config.newNote->attachmentDelimiter.size(), mediaString);
                } else {
                    newNote.content = newNote.content->replace("</en-note>","<br>"+mediaString+"</en-note>");
                }
                newNote.resources->append(newRes);
            }
        }
        noteTable.expunge(newLid);
        noteTable.add(newLid,newNote,true);
        std::cout << newLid << QString(tr(" has been created.\n")).toStdString();
        return newLid;
    }
    return 0;
}



// Append text to a note via the command line.
int CmdLineTool::appendNote(StartupConfig config) {

// Windows Check
#ifndef _WIN32
#ifdef Q_OS_WIN32
    _setmode(_fileno(stdin), _O_BINARY);
#endif
#endif // end windows check

    // If we are reding stdin
    if (config.newNote->content == "")  {
        QByteArray content;

        while(!std::cin.eof()) {
            char arr[1024];
            std::cin.read(arr,sizeof(arr));
            int s = std::cin.gcount();
            content.append(arr,s);
            content.append("<br>");
            content.replace("\n","<br>");
        }
        config.newNote->content = QString::fromAscii(content);
    }

    if (!config.newNote->content.contains("<body")) {
        config.newNote->content.prepend("<body>");
    }
    if (!config.newNote->content.contains("</body")) {
        config.newNote->content.append("</body>");
    }


    EnmlFormatter formatter;
    formatter.setHtml(config.newNote->content);
    config.newNote->content = formatter.rebuildNoteEnml();

    bool expectResponse = true;

    // Look to see if another NixNote is running.  If so, then we
    // expect a response of the LID created.  First we detach it so
    // we are not talking to ourselves.
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        expectResponse = false;
    } else {
        global.sharedMemory->detach();
    }
    if (expectResponse) {
        NUuid uuid;
        QString id = uuid.create();

        // Setup cross memory for response
        CrossMemoryMapper crossMemory(id);
        if (!crossMemory.allocate(512))
            expectResponse = false;

        // Write out the segment
        config.newNote->write(id);
        // Start checking for the response
        int cnt = 0;
        qint32 newLid = -1;
        int maxWait = 5;
        while (expectResponse && cnt<maxWait) {
            QByteArray data = crossMemory.read();
            if (!data.startsWith('\0')) {
                expectResponse = false;
                newLid = data.toInt();
            } else {
                sleep(1);
            }
            cnt++;
        }
        if (newLid == -1) {
            std::cout << newLid << QString(tr(" was not found.")).toStdString();
        } else {
            if (newLid > 0) {
                std::cout << newLid << QString(tr(" has been appended.\n")).toStdString();
                return newLid;
            } else {
                std::cout << QString(tr("No response from NixNote.  Please verify that the note was appended.\n")).toStdString();
            }
        }
    } else {
       // Another NN isn't found, so we do this ourself
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        Note newNote;

        // Fetch the existing note
        NoteTable noteTable(global.db);
        if (!noteTable.get(newNote, config.newNote->lid, true, true)) {
            std::cerr << config.newNote->lid << QString(tr(" was not found.\n")).toStdString();
            return -1;
        }

        // Append the text to the existing note
        newNote.content->replace("</en-note>", "<br/>");

        // Chop off the beginning of the new text to remove the <en-note stuff
        int startOfNote = config.newNote->content.indexOf("<en-note");
        config.newNote->content = config.newNote->content.mid(startOfNote+9);

        // Append the two notes
        newNote.content = newNote.content + config.newNote->content;

        // Do the attachments
        for (int i=0; i<config.newNote->attachments.size(); i++) {
            QString filename = config.newNote->attachments[i];
            QFile file(filename);
            if (file.exists()) {

                file.open(QIODevice::ReadOnly);
                QByteArray ba = file.readAll();
                file.close();

                MimeReference mimeRef;
                QString extension = filename;
                int endPos = filename.lastIndexOf(".");
                if (endPos != -1)
                    extension = extension.mid(endPos);
                QString mime =  mimeRef.getMimeFromExtension(extension);
                Resource newRes;
                bool attachment = true;
                if (mime == "application/pdf" || mime.startsWith("image/"))
                    attachment = false;
                config.newNote->createResource(newRes, 0, ba, mime, attachment, QFileInfo(filename).fileName(), config.newNote->lid);
                QByteArray hash;
                if (newRes.data.isSet()) {
                    Data d = newRes.data;
                    if (d.bodyHash.isSet())
                        hash = d.bodyHash;
                }
                if (!newNote.resources.isSet()) {
                    newNote.resources = QList<Resource>();
                }
                QString mediaString = "<en-media hash=\""+hash.toHex()+"\" type=\""+mime+"\"/>";
                if (newNote.content->contains(config.newNote->attachmentDelimiter)) {
                     //newNote.content = newNote.content->replace(config.newNote->attachmentDelimiter,mediaString);
                     newNote.content = newNote.content->replace(newNote.content->indexOf(config.newNote->attachmentDelimiter),
                                                         config.newNote->attachmentDelimiter.size(), mediaString);
                } else {
                    newNote.content = newNote.content->replace("</en-note>","<br>"+mediaString+"</en-note>");
                }
                newNote.resources->append(newRes);
            }
        }
        noteTable.expunge(config.newNote->lid);
        noteTable.add(config.newNote->lid,newNote,true);
        std::cout << config.newNote->lid << QString(tr(" has been appended.\n")).toStdString();
        return config.newNote->lid;
    }
    return 0;
}




// Read a note via the command line and extract the text
// contents.
int CmdLineTool::readNote(StartupConfig config) {
    bool useCrossMemory = true;

    // Look to see if another NixNote is running.  If so, then we
    // expect a response.  Otherwise, we do it ourself.
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        useCrossMemory = false;
    }
    if (useCrossMemory) {
        NUuid uuid;
        config.extractText->returnUuid = uuid.create();
        CrossMemoryMapper sharedMemory(config.extractText->returnUuid);
        if (!sharedMemory.allocate(500*1024))
            return 16;
        sharedMemory.clearMemory();
        global.sharedMemory->write("READ_NOTE:" + config.extractText->wrap());
        int maxWait = 5;
        bool expectResponse = true;
        int cnt = 0;
        while (expectResponse && cnt<maxWait) {
            QByteArray data = sharedMemory.read();
            if (!data.startsWith('\0')) {
                expectResponse = false;
                config.extractText->unwrap(data);
            } else {
                sleep(1);
            }
            cnt++;
        }
        if (!expectResponse)
            std::cout << config.extractText->text.toStdString() << endl;
        else
            std::cout << tr("No response received from NixNote.").toStdString();
    } else {
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        NoteTable noteTable(global.db);
        Note n;
        QString text;
        if (noteTable.get(n,config.extractText->lid,false,false))
            text = config.extractText->stripTags(n.content);
        else
            text = tr("Note not found.");
        std::cout << text.toStdString() << endl;
    }
    return 0;
}



// Export notes or do a backup via the command line
int CmdLineTool::exportNotes(StartupConfig config) {
    if (global.sharedMemory->attach()) {
        std::cout << tr("This cannot be done with NixNote running.").toStdString() << endl;
        return 16;
    }
    global.db = new DatabaseConnection("nixnote");  // Startup the database
    if (config.exportNotes->backup)
        config.exportNotes->backupDB();
    else
        config.exportNotes->extract();
    return 0;
}



// Import notes from a nnex file.
int CmdLineTool::importNotes(StartupConfig config) {
    if (global.sharedMemory->attach()) {
        std::cout << tr("This cannot be done with NixNote running.").toStdString() << endl;
        return 16;
    }
    global.db = new DatabaseConnection("nixnote");  // Startup the database
    config.importNotes->import();
    return 0;
}


// Alter a note's notebook or add/remove tags for a note.
int CmdLineTool::alterNote(StartupConfig config) {
    // Look to see if another NixNote is running.  If so, then we
    // expect a response, otherwise we do it ourself.
    bool useCrossMemory = true;
    global.sharedMemory->unlock();
    global.sharedMemory->detach();
    if (!global.sharedMemory->attach()) {
        useCrossMemory = false;
    }
    if (useCrossMemory) {
        global.sharedMemory->write("ALTER_NOTE:" + config.alter->wrap());
    } else {
        global.db = new DatabaseConnection("nixnote");  // Startup the database
        return config.alter->alterNote();
    }
    return 0;
}


// Open a notebook
int CmdLineTool::openNotebook(StartupConfig config) {
    if (global.sharedMemory->attach()) {
        std::cout << tr("This cannot be done with NixNote running.").toStdString() << endl;
        return 16;
    }
    global.db = new DatabaseConnection("nixnote");  // Startup the database
    NotebookTable bookTable(global.db);
    for (int i=0; i<config.notebookList.size(); i++) {
        qint32 lid = bookTable.findByName(config.notebookList[i]);
        if (lid >0) {
            bookTable.openNotebook(lid);
        } else {
            std::cout << tr("Notebook not found: ").toStdString() << config.notebookList[i].toStdString() << endl;
        }
    }
    return 0;
}


// Close a notebook
int CmdLineTool::closeNotebook(StartupConfig config) {
    if (global.sharedMemory->attach()) {
        std::cout << tr("This cannot be done with NixNote running.").toStdString() << endl;
        return 16;
    }
    global.db = new DatabaseConnection("nixnote");  // Startup the database
    NotebookTable bookTable(global.db);
    for (int i=0; i<config.notebookList.size(); i++) {
        qint32 lid = bookTable.findByName(config.notebookList[i]);
        if (lid >0) {
            bookTable.closeNotebook(lid);
        } else {
            std::cout << tr("Notebook not found: ").toStdString() << config.notebookList[i].toStdString() << endl;
        }
    }
    return 0;
}


#include "models/notemodel.h"
#include "sql/nsqlquery.h"

// Do a sync
int CmdLineTool::sync() {
    if (!global.accountsManager->oauthTokenFound()) {
        std::cout << tr("OAuth token not found.").toStdString() << endl;
        return 16;
    }

    global.db = new DatabaseConnection("nixnote");  // Startup the database

    // Check if the table exists.  If not, create it.
    NSqlQuery sql(global.db);
    sql.exec("Select *  from sqlite_master where type='table' and name='NoteTable';");
    if (!sql.next()) {
        NoteModel model(this);
        model.createTable();
    }
    sql.finish();

    SyncRunner runner;
    runner.synchronize();
    if (runner.error) {
        std::cout << tr("Error synchronizing with Evernote.").toStdString() << std::endl;
        return 16;
    }
    std::cout << tr("Sync completed.").toStdString() << std::endl;
    return 0;
}




int CmdLineTool::signalGui(StartupConfig config) {

    // Make sure another one is actually running. If not, we exit out.
    if (!global.sharedMemory->attach()) {
        return 16;
    }
    if (config.signalGui->show)
        global.sharedMemory->write(QString("SIGNAL_GUI: SHOW"));
    if (config.signalGui->takeScreenshot)
        global.sharedMemory->write(QString("SIGNAL_GUI: SCREENSHOT"));
    if (config.signalGui->shutdown)
        global.sharedMemory->write(QString("SIGNAL_GUI: SHUTDOWN"));
    if (config.signalGui->newNote)
        global.sharedMemory->write(QString("SIGNAL_GUI: NEW_NOTE"));
    if (config.signalGui->newExternalNote)
        global.sharedMemory->write(QString("SIGNAL_GUI: NEW_EXTERNAL_NOTE"));
    if (config.signalGui->openNote)
        global.sharedMemory->write("SIGNAL_GUI: OPEN_NOTE " + QVariant(config.signalGui->lid).toString());
    if (config.signalGui->openExternalNote)
        global.sharedMemory->write("SIGNAL_GUI: OPEN_EXTERNAL_NOTE " + QVariant(config.signalGui->lid).toString());
    if (config.signalGui->openNoteNewTab)
        global.sharedMemory->write("SIGNAL_GUI: OPEN_NOTE_NEW_TAB " + QVariant(config.signalGui->lid).toString());
    if (config.signalGui->synchronize)
        global.sharedMemory->write(QString("SIGNAL_GUI: SYNCHRONIZE"));

    return 0;
}

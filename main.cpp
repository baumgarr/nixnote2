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


#include <QtGui/QApplication>
#include "nixnote.h"
#include "global.h"
#include "settings/startupconfig.h"

#include "logger/qslog.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <QSharedMemory>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


#include "application.h"


using namespace std;

//*********************************************
//* This is the main entry point for NixNote.
//*********************************************
extern Global global;

//*********************************************************************
//* Segmentation fault.  This is triggered to print a stack trace.
//*********************************************************************
void fault_handler(int sig) {
  void *array[30];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 30);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}




//using namespace cv;
//*********************************************************************
//* Main entry point to the program.
//*********************************************************************
int main(int argc, char *argv[])
{
    signal(SIGSEGV, fault_handler);   // install our handler

    // Setup the QApplication so we can begin
    Application *a = new Application(argc, argv);
    global.application = a;

    // Setup the QLOG functions for debugging & messages
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
//    const QString sLogPath(a->applicationDirPath());

    QsLogging::DestinationPtr debugDestination(
                QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());

    // Begin setting up the environment
    StartupConfig startupConfig;
    global.argc = argc;
    global.argv = argv;

    startupConfig.accountId = -1;

    for (int i=1; i<=argc; i++) {
        QString parm(argv[i]);
        if (parm == "--help" || parm == "-?") {
            printf("\n\n");
            printf("NixNote command line options:\n");
            printf("  --help or -?                  Show this message\n");
            printf("  --accountId=<id>              Start with specified user account\n");
            printf("  --dontStartMinimized          Override option to start minimized\n");
            printf("  --disablIndexing              Disable all indexing\n");
            printf("  --openNote=<lid>              Open a specific note on startup\n");
            printf("  --forceSystemTrayAvailable    Force the program to accept that\n");
            printf("                                the desktop supports tray icons.\n");
            printf("  --startMinimized              Force a startup with NixNote minimized\n");
            printf("  --syncAndExit                 Synchronize and exit the program.\n");
            printf("\n\n");
            return 0;
        }
        if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
            parm = parm.mid(12);
            startupConfig.accountId = parm.toInt();
        }
        if (parm.startsWith("--openNote=", Qt::CaseSensitive)) {
            parm = parm.mid(11);
            startupConfig.startupNoteLid = parm.toInt();
        }
        if (parm == "--dontStartMinimized") {
            startupConfig.forceNoStartMinimized = true;
        }
        if (parm == "--startMinimized") {
            startupConfig.forceStartMinimized = true;
        }
        if (parm == "--newNote") {
            startupConfig.startupNewNote = true;
        }
        if (parm == "--syncAndExit") {
            startupConfig.syncAndExit = true;
        }
        if (parm == "--disableIndexing") {
            startupConfig.disableIndexing = true;
        }
        if (parm == "--forceSystemTrayAvailable") {
            startupConfig.forceSystemTrayAvailable = true;
        }
    }

    startupConfig.programDirPath = global.getProgramDirPath() + QDir().separator();
    startupConfig.name = "NixNote";
    global.setup(startupConfig);

    QString logPath = global.fileManager.getLogsDirPath("")+"messages.log";
    QsLogging::DestinationPtr fileDestination(
                 QsLogging::DestinationFactory::MakeFileDestination(logPath) ) ;
    logger.addDestination(fileDestination.get());


    // Show Qt version.  This is useful for debugging
    QLOG_DEBUG() << "Program Home: " << global.fileManager.getProgramDirPath("");
    QLOG_INFO() << "Built on " << __DATE__ << " at " << __TIME__;
    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
    //QLOG_INFO() << "Thrift version: " << PACKAGE_VERSION;



    // Create a shared memory region.  We use this to communicate
    // with any other instance that may be running.  If another instance
    // is found we need to either show that one or kill this one.
    bool memInitNeeded = true;
    if( !global.sharedMemory->create( 512*1024, QSharedMemory::ReadWrite) ) {
        // Attach to it and detach.  This is done in case it crashed.
        global.sharedMemory->attach();
        global.sharedMemory->detach();
        if( !global.sharedMemory->create( 512*1024, QSharedMemory::ReadWrite) ) {
            if (startupConfig.startupNewNote) {
                global.sharedMemory->attach();
                global.sharedMemory->lock();
                void *dataptr = global.sharedMemory->data();
                memcpy(dataptr, "NEW_NOTE", 8);  // Tell the other guy create a new note
                QLOG_INFO() << "Another NixNote was found.  Requesting it to start another note";
                exit(0);  // Exit this one
            }
            if (startupConfig.startupNoteLid > 0) {
                global.sharedMemory->attach();
                global.sharedMemory->lock();
                void *dataptr = global.sharedMemory->data();
                QString msg = "OPEN_NOTE:" +QString::number(startupConfig.startupNoteLid) + " ";
                memcpy(dataptr, msg.toStdString().c_str(), msg.length());  // Tell the other guy to open a note
                QLOG_INFO() << "Another NixNote was found.  Requesting it to open a note";
                exit(0);  // Exit this one
            }
            // If we've gotten this far, we need to either stop this instance or stop the other
            global.settings->beginGroup("Debugging");
            QString startup = global.settings->value("onMultipleInstances", "SHOW_OTHER").toString();
            global.settings->endGroup();
            global.sharedMemory->attach();
            global.sharedMemory->lock();
            void *dataptr = global.sharedMemory->data();
            if (startup == "SHOW_OTHER") {
                memcpy(dataptr, "SHOW_WINDOW", 11);  // Tell the other guy to show himself
                QLOG_INFO() << "Another NixNote was found.  Stopping this instance";
                exit(0);  // Exit this one
            }
            if (startup == "STOP_OTHER") {
                memcpy(dataptr, "IMMEDIATE_SHUTDOWN", 18);  // Tell the other guy to shut down
                memInitNeeded = false;
            }
            global.sharedMemory->unlock();
        }
    }
    if (memInitNeeded) {
        global.sharedMemory->lock();
        memset(global.sharedMemory->data(), 0, global.sharedMemory->size());
        global.sharedMemory->unlock();
    }



    // Save the clipboard
    global.clipboard = QApplication::clipboard();

    NixNote *w = new NixNote();
    w->setAttribute(Qt::WA_QuitOnClose);
    bool show = true;
    if (global.syncAndExit)
        show = false;
    if (global.minimizeToTray() && global.startMinimized)
        show = false;
    if (show)
        w->show();
    else
        w->hide();
    if (global.startMinimized)
        w->showMinimized();

    int rc = a->exec();
    QLOG_DEBUG() << "Unlocking memory";
    global.sharedMemory->unlock();
    QLOG_DEBUG() << "Deleting NixNote instance";
    delete w;
    QLOG_DEBUG() << "Quitting application instance";
    a->exit(rc);
    QLOG_DEBUG() << "Deleting application instance";
    delete a;
    QLOG_DEBUG() << "Exiting: RC=" << rc;
    exit(rc);
    return rc;
}

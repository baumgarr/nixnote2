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
            printf("  --help or -?            Show this message\n");
            printf("  --accountId=<id>        Start with specified user account\n");
            printf("  --dontStartMinimized    Override option to start minimized\n");
            printf("  --syncAndExit           Synchronize and exit the program.\n");
            printf("\n\n");
            return 0;
        }
        if (parm.startsWith("--accountId=", Qt::CaseSensitive)) {
            parm = parm.mid(12);
            startupConfig.accountId = parm.toInt();
        }
        if (parm == "--dontStartMinimized") {
            startupConfig.forceNoStartMinimized = true;
        }
        if (parm == "--syncAndExit") {
            startupConfig.syncAndExit = true;
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
    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
    //QLOG_INFO() << "Thrift version: " << PACKAGE_VERSION;



    // Create a shared memory region.  We use this to communicate
    // with any other instance that may be running.  If another instance
    // is found we need to either show that one or kill this one.
    bool memInitNeeded = true;
    if( !global.sharedMemory->create( 512, QSharedMemory::ReadWrite) ) {
        // Attach to it and detach.  This is done in case it crashed.
        global.sharedMemory->attach();
        global.sharedMemory->detach();
        if( !global.sharedMemory->create( 512, QSharedMemory::ReadWrite) ) {
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
    if (!global.syncAndExit)
        w->show();
    else {
        w->hide();
        w->synchronize();
    }

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

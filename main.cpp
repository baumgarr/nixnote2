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


#include <QApplication>
#include "nixnote.h"
#include "global.h"
#include "settings/startupconfig.h"
#include "cmdtools/cmdlinetool.h"
//#include "cmdtools/cmdlineapp.h"

#include "logger/qslog.h"
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <QSharedMemory>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <QNetworkProxy>


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

    bool guiAvailable = true;
    signal(SIGSEGV, fault_handler);   // install our handler

    // Begin setting up the environment
    StartupConfig startupConfig;
    global.argc = argc;
    global.argv = argv;

    int retval = startupConfig.init(argc,argv, guiAvailable);
    if (retval != 0)
        return retval;

    // Setup the application. If we have a GUI, then we use Application.
    // If we don't, then we just use a derivitive of QCoreApplication
    QCoreApplication *a = NULL;
    if (guiAvailable) {
        Application *app = new Application(argc, argv);
        a = app;
    } else {
        a = new QCoreApplication(argc, argv);
    }
    global.application = a;

    // Setup the QLOG functions for debugging & messages
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    QsLogging::DestinationPtr debugDestination(
                QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());

    startupConfig.programDirPath = global.getProgramDirPath() + QDir().separator();
    startupConfig.name = "NixNote";
    global.setup(startupConfig, guiAvailable);
//    global.syncAndExit=startupConfig.syncAndExit;

    // We were passed a SQL command
    if (startupConfig.sqlExec) {
        DatabaseConnection *db = new DatabaseConnection("nixnote");  // Startup the database
        QLOG_DEBUG() << "Starting DB";
        QSqlQuery query(db->conn);
        QLOG_DEBUG() << "After DB Start";
        if (!query.exec(startupConfig.sqlString)) {
            QLOG_FATAL() << query.lastError();
            delete db;
            exit(16);
        }
        while (query.next()) {
            QString result = "";
            for (int i=0; i<query.record().count(); i++) {
                result = result + query.value(i).toString() + " | ";
            }
            QLOG_INFO() << result;
        }
        delete db;
        QLOG_INFO() << "Ok" << endl;
        exit(0);
    }


    // If we want something other than the GUI, try let the CmdLineTool deal with it.
    if (!startupConfig.gui()) {
        global.purgeTemporaryFilesOnShutdown=startupConfig.purgeTemporaryFiles;
        CmdLineTool cmdline;
        startupConfig.purgeTemporaryFiles=false;
        int retval = cmdline.run(startupConfig);
        if (global.sharedMemory->isAttached())
            global.sharedMemory->detach();
        QLOG_DEBUG() << "Exiting: RC=" << retval;
        delete a;
        exit(retval);
    }

    QString logPath = global.fileManager.getLogsDirPath("")+"messages.log";
    QsLogging::DestinationPtr fileDestination(
                 QsLogging::DestinationFactory::MakeFileDestination(logPath) ) ;
    logger.addDestination(fileDestination.get());


    // Show Qt version.  This is useful for debugging
    QLOG_DEBUG() << "Program Home: " << global.fileManager.getProgramDirPath("");
    QLOG_DEBUG() << "Built on " << __DATE__ << " at " << __TIME__;
    QLOG_DEBUG() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();



    // Create a shared memory region.  We use this to communicate
    // with any other instance that may be running.  If another instance
    // is found we need to either show that one or kill this one.
    bool memInitNeeded = true;
    if( !global.sharedMemory->allocate(512*1024) ) {
        // Attach to it and detach.  This is done in case it crashed.
        global.sharedMemory->attach();
        global.sharedMemory->detach();
        if( !global.sharedMemory->allocate(512*1024) ) {
            QLOG_DEBUG() << "segment created";
            if (startupConfig.startupNewNote) {
                global.sharedMemory->attach();
                global.sharedMemory->write(QString("NEW_NOTE"));
//                global.sharedMemory->detach();
                exit(0);  // Exit this one
            }
            if (startupConfig.startupNoteLid > 0) {
                global.sharedMemory->attach();
                global.sharedMemory->write("OPEN_NOTE"+QString::number(startupConfig.startupNoteLid) + " ");
//               global.sharedMemory->detach();
                exit(0);  // Exit this one
            }
            // If we've gotten this far, we need to either stop this instance or stop the other
            QLOG_DEBUG() << "Multiple instance found";
            global.settings->beginGroup("Debugging");
            QString startup = global.settings->value("onMultipleInstances", "SHOW_OTHER").toString();
            global.settings->endGroup();
            global.sharedMemory->attach();
            if (startup == "SHOW_OTHER") {
                global.sharedMemory->write(QString("SHOW_WINDOW"));
                global.sharedMemory->detach();
                exit(0);  // Exit this one
            }
            if (startup == "STOP_OTHER") {
                global.sharedMemory->write(QString("IMMEDIATE_SHUTDOWN"));
                memInitNeeded = false;
            }
        }
    }

    if (memInitNeeded) {
        global.sharedMemory->clearMemory();
    }


    // Save the clipboard
    global.clipboard = QApplication::clipboard();

    QLOG_DEBUG() << "Setting up NN";
    NixNote *w = new NixNote();
    w->setAttribute(Qt::WA_QuitOnClose);
    bool show = true;
    if (global.minimizeToTray() && global.startMinimized)
        show = false;
    if (show)
        w->show();
    else
        w->hide();
    if (global.startMinimized)
        w->showMinimized();

    // Setup the proxy
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    if (global.isProxyEnabled()) {
        QString host = global.getProxyHost();
        int port = global.getProxyPort();
        QString user = global.getProxyUserid();
        QString password = global.getProxyPassword();

        if (host.trimmed() != "")
            proxy.setHostName(host.trimmed());
        if (port > 0)
            proxy.setPort(port);
        if (user.trimmed() != "")
            proxy.setUser(user.trimmed());
        if (password.trimmed() != "")
            proxy.setPassword(password.trimmed());

        QNetworkProxy::setApplicationProxy(proxy);
    }

    QLOG_DEBUG() << "Launching";
    int rc = a->exec();
    if (global.sharedMemory->isAttached())
        global.sharedMemory->detach();
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

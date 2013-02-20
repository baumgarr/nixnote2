#include <QtGui/QApplication>
#include "nixnote.h"
#include "global.h"
#include "settings/startupconfig.h"
#include "botan/botan_all.h"

#include "logger/qslog.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <QSharedMemory>

//#include "utilities/encrypt.h"
#include "application.h"


//using namespace std;
//using namespace Botan;
//#include <QString>
//#include <QByteArray>


//*********************************************
//* This is the main entry point for NixNote.
//*********************************************
extern Global global;

int main(int argc, char *argv[])
{

    Botan::LibraryInitializer botanInit;

    // Setup the QApplication so we can begin
    Application a(argc, argv);

    // Setup the QLOG functions for debugging & messages
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(a.applicationDirPath());

    QsLogging::DestinationPtr fileDestination(
                QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
                QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());


    // Begin setting up the environment
    global.argc = argc;
    global.argv = argv;

    // Show Qt version.  This is useful for debugging
    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();


    StartupConfig startupConfig;
    startupConfig.programDirPath = global.getProgramDirPath() + QDir().separator();
    startupConfig.name = "NixNote";
    global.setup(startupConfig);

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

    NixNote w;
    w.show();

    return a.exec();

}

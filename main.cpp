#include <QtGui/QApplication>
#include "nixnote.h"
#include "global.h"
#include "settings/startupconfig.h"

#include "logger/qslog.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>


//*********************************************
//* This is the main entry point for NixNote.
//*********************************************
extern Global global;

int main(int argc, char *argv[])
{
    // Setup the QApplication so we can begin
    QApplication a(argc, argv);

    // Setup the QLOG functions for debugging & messages
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::DebugLevel);
    const QString sLogPath(a.applicationDirPath());

    QsLogging::DestinationPtr fileDestination(
        QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());


    // Begin setting up the environment
    QLOG_DEBUG() << "Saving command line parameters";
    global.argc = argc;
    global.argv = argv;

    // Show Qt version.  This is useful for debugging
    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();
    QLOG_DEBUG() << "Program starting";


    StartupConfig startupConfig;
    startupConfig.programDirPath = global.getProgramDirPath() + QDir().separator();
    startupConfig.name = "NixNote";
    QLOG_DEBUG() << "Running global setup";
    global.setup(startupConfig);

    NixNote w;
    w.show();
    //w.showMaximized();

    return a.exec();
}

/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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


#include "nsqlquery.h"
#include <QSqlQuery>
#include <QSqlError>

#include "global.h"

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

extern Global global;


// Constructor
NSqlQuery::NSqlQuery(DatabaseConnection *db) :
    QSqlQuery(db->conn)
{
    this->db = db;
}


// Destructor
NSqlQuery::~NSqlQuery() {
    this->finish();
    if (db->dbLocked) {
        QLOG_TRACE() << "*** Warning: NSqlQuery Terminating with lock active";

        global.settings->beginGroup("Debugging");
        int level = global.settings->value("messageLevel", -1).toInt();
        global.settings->endGroup();

        // Setup the QLOG functions for debugging & messages
        if (level == QsLogging::TraceLevel) {
            void *array[30];
            size_t size;

            size = backtrace(array, 30);
            if (size > 2)
                // print out all the frames to stderr
                backtrace_symbols_fd(array, 2, 2);
            QLOG_TRACE() << "  ***  ";
        }
    }
}


// Generic exec().  A prepare should have been done already
bool NSqlQuery::exec() {
    for (int i=1; i<1000; i++) {
        bool rc = QSqlQuery::exec();
        if (rc)
            return true;
        if (lastError().number() != DATABASE_LOCKED)
            return false;
        if (i>10) {
            QLOG_ERROR() << "DB Locked:  Retry #" << i;
        }
        QTime dieTime= QTime::currentTime().addSecs(1);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    return false;
}



// Execute a SQL statement
bool NSqlQuery::exec(const QString &query) {
    for (int i=1; i<1000; i++) {
        bool rc = QSqlQuery::exec(query);
        if (rc) {
            return true;
        }
        if (lastError().number() != DATABASE_LOCKED)
            return false;
        if (i>10) {
            QLOG_ERROR() << "DB Locked:  Retry #" << i;
        }
        QTime dieTime= QTime::currentTime().addSecs(1);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    return false;
}



// Execute a SQL statement
bool NSqlQuery::exec(const string query) {
    QString q;
    q = QString::fromStdString(query);
    return this->exec(q);
}


// Execute a SQL statement
bool NSqlQuery::exec(const char *query) {
    QString q;
    q = QString::fromStdString(query);
    return this->exec(q);
}

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

// The following include is needed for demangling names on a backtrace
#include <cxxabi.h>

extern Global global;


// Constructor
NSqlQuery::NSqlQuery(DatabaseConnection *db) :
    QSqlQuery(db->conn)
{
    this->db = db;
    DEBUG_TRIGGER = 100;
    INDEX_PAUSE_TRIGGER = 10;
}


// Destructor
NSqlQuery::~NSqlQuery() {
    this->finish();
//    if (db->dbLocked) {
//        QLOG_DEBUG() << "*** Warning: NSqlQuery Terminating with lock active";
//        global.stackDump();
//    }
}


// Generic exec().  A prepare should have been done already
bool NSqlQuery::exec() {
    bool indexPauseSave;
    bool indexRestoreNeeded = false;
    for (int i=1; i<1000; i++) {
        bool rc = QSqlQuery::exec();
        if (rc) {
            if (indexRestoreNeeded)
                global.indexRunner->pauseIndexing = indexPauseSave;
            return true;
        }
        if (lastError().number() != DATABASE_LOCKED)
            return false;
        if (i>DEBUG_TRIGGER) {
            QLOG_ERROR() << "DB Locked:  Retry #" << i;
        }

        if (i == INDEX_PAUSE_TRIGGER && this->db->getConnectionName() != "indexrunner") {
            QLOG_DEBUG() << "Pausing indexrunner due to db lock";
            indexPauseSave = global.indexRunner->pauseIndexing;
            indexRestoreNeeded = true;
            global.indexRunner->pauseIndexing=true;
        }


        // Print stack trace to see what is happening
        if (i==DEBUG_TRIGGER) {
            QLOG_DEBUG() << "Dumping stack due to DB lock limit of " << DEBUG_TRIGGER << " being reached.";
            global.stackDump();
        }

        QTime dieTime= QTime::currentTime().addSecs(1);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    if (indexRestoreNeeded)
        global.indexRunner->pauseIndexing = indexPauseSave;
    return false;
}



// Execute a SQL statement
bool NSqlQuery::exec(const QString &query) {
    bool indexPauseSave;
    bool indexRestoreNeeded = false;
    for (int i=1; i<1000; i++) {
        bool rc = QSqlQuery::exec(query);
        if (rc) {
            if (indexRestoreNeeded)
                global.indexRunner->pauseIndexing = indexPauseSave;
            return true;
        }
        if (lastError().number() != DATABASE_LOCKED)
            return false;

        if (i == INDEX_PAUSE_TRIGGER && this->db->getConnectionName() != "indexrunner") {
            QLOG_DEBUG() << "Pausing indexrunner due to db lock";
            indexPauseSave = global.indexRunner->pauseIndexing;
            indexRestoreNeeded = true;
            global.indexRunner->pauseIndexing=true;
        }

        if (i>DEBUG_TRIGGER) {
            QLOG_ERROR() << "DB Locked:  Retry #" << i;
        }

        // Print stack dump to see what is happening
        if (i==DEBUG_TRIGGER) {
            QLOG_DEBUG() << "Dumping stack due to DB lock limit of " << DEBUG_TRIGGER << " being reached.";
            global.stackDump();
        }
        QTime dieTime= QTime::currentTime().addSecs(1);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    if (indexRestoreNeeded)
        global.indexRunner->pauseIndexing = indexPauseSave;
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





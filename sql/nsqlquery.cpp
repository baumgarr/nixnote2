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
}


// Destructor
NSqlQuery::~NSqlQuery() {
    this->finish();
    if (db->dbLocked) {
        QLOG_TRACE() << "*** Warning: NSqlQuery Terminating with lock active";
        this->stackDump();
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


        // Print stack trace to see what is happening
        if (i==10) {
            this->stackDump();
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

        // Print stack dump to see what is happening
        if (i==10) {
            this->stackDump();
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




void NSqlQuery::stackDump() {
    void *array[30];
    size_t size;
    QLOG_ERROR() << "***** Dumping stack *****";

    // get void*'s for all entries on the stack
    size = backtrace(array, 30);
    char **messages = backtrace_symbols(array, size);

    for (size_t i = 1; i < size && messages != NULL; ++i)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_name = p;
            }
            else if (*p == '+')
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0)
            {
               QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                          << real_name << "+" << offset_begin << offset_end;

            }
            // otherwise, output the mangled function name
            else
            {
                QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                          << mangled_name << "+" << offset_begin << offset_end;
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else
        {
            QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i];
        }
    }

    free(messages);
    QLOG_ERROR() << "**** Stack dump complete *****";
}

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

extern Global global;

NSqlQuery::NSqlQuery(QSqlDatabase db) :
    QSqlQuery(db)
{
}



NSqlQuery::~NSqlQuery() {
    this->finish();
}

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


bool NSqlQuery::exec(const string query) {
    QString q;
    q = QString::fromStdString(query);
    return this->exec(q);
}

bool NSqlQuery::exec(const char *query) {
    QString q;
    q = QString::fromStdString(query);
    return this->exec(q);
}

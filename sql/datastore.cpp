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

#include "datastore.h"
#include <QtSql>
#include <QString>
#include <QList>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "configstore.h"
#include "searchtable.h"
#include "tagtable.h"
#include "notebooktable.h"


#define GUID 1;
#define DIRTY_RECORD 2;
#define ORIGINAL_GID 3;
#define UPDATE_SEQUENCE_NUMBER 4;

DataStore::DataStore(QObject *parent) :
    QSqlTableModel(parent)
{
  // Check if the table exists.  If not, create it.
  QSqlQuery sql;
  sql.exec("Select * from sqlite_master where type='table' and name='DataStore';");
  if (!sql.next())
      this->createTable();
  this->setTable("DataStore");
  this->select();
  this->setEditStrategy(QSqlTableModel::OnFieldChange);
}


//* Create the NoteTable table.
void DataStore::createTable() {
    QLOG_TRACE() << "Entering DataStore::createTable()";

    QLOG_DEBUG() << "Creating table DataStore";
    QSqlQuery sql;
    QString command("Create table DataStore (" +
                  QString("lid integer,") +
                  QString("key integer,") +
                  QString("data blob default null collate nocase)"));
    if (!sql.exec(command)) {
        QLOG_ERROR() << "Creation of DataStore table failed: " << sql.lastError();
    }

    sql.exec("CREATE INDEX DataStore_Lid on DataStore (lid)");
    sql.exec("CREATE INDEX DataStore_Key on DataStore (key)");

    sql.prepare("Create view SearchModel as select lid, data as name from DataStore where key=2001");
    if (!sql.exec()) {
        QLOG_ERROR() << "Creation of SearchModel table failed: " << sql.lastError();
    }

    sql.prepare("Create View TagModel as select a.lid, (select d.data from DataStore d where d.key=1000 and a.lid = d.lid) as guid, (select data from datastore b1 where b1.lid = (select b.data from DataStore b where b.key=1002 and a.lid = b.lid)) as parent_gid, (select c.data from DataStore c where c.key=1001 and a.lid = c.lid) as name, (select e.data from DataStore e where e.key=1006 and a.lid = e.lid) as account from DataStore a where a.key=1000;");
    if (!sql.exec()) {
        QLOG_ERROR() << "Creation of TagModel table failed: " << sql.lastError();
    }

    sql.prepare("Create View NotebookModel as select a.lid, (select b.data from DataStore b where b.key=3002 and a.lid = b.lid) as stack, (select c.data from DataStore c where c.key=3001 and a.lid = c.lid) as name, (select d.data from DataStore d where d.key=3201 and a.lid = d.lid) as username from DataStore a where a.key=3000;");
    if (!sql.exec()) {
        QLOG_ERROR() << "Creation of NotebookModel table failed: " << sql.lastError();
    }

    if (!sql.exec("Create virtual table SearchIndex using fts4 (lid int, weight int, source text, content text)")) {
        QLOG_ERROR() << "Creation of SearchIndex table failed: " << sql.lastError();
    }

    Notebook notebook;
    NotebookTable table;
    notebook.name = "My Notebook";
    notebook.defaultNotebook = true;
    notebook.__isset.name = true;
    notebook.__isset.defaultNotebook = true;
    table.add(0,notebook,true,false);
}


qint32 DataStore::getDirtyNoteGuids(QList<QString> &retVal) {
    retVal = retVal;  /* suppress unused warning */
    return 0;
}

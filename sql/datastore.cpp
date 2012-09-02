#include "datastore.h"
#include <QtSql>
#include <QString>
#include <QList>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "configstore.h"
#include "searchtable.h"
#include "tagtable.h"


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

    sql.prepare("Create View TagModel as select a.lid, (select d.data from DataStore d where d.key=1000 and a.lid = d.lid) as guid, (select b.data from DataStore b where b.key=1002 and a.lid = b.lid) as parent_gid, (select c.data from DataStore c where c.key=1001 and a.lid = c.lid) as name from DataStore a where a.key=1000;");
    if (!sql.exec()) {
        QLOG_ERROR() << "Creation of TagModel table failed: " << sql.lastError();
    }

    sql.prepare("Create View NotebookModel as select a.lid, (select b.data from DataStore b where b.key=3002 and a.lid = b.lid) as stack, (select c.data from DataStore c where c.key=3001 and a.lid = c.lid) as name from DataStore a where a.key=3000;");
    if (!sql.exec()) {
        QLOG_ERROR() << "Creation of NotebookModel table failed: " << sql.lastError();
    }

    if (!sql.exec("Create virtual table SearchIndex using fts4 (lid int, weight int, source text, content text)")) {
        QLOG_ERROR() << "Creation of SearchIndex table failed: " << sql.lastError();
    }


}


qint32 DataStore::getDirtyNoteGuids(QList<QString> &retVal) {
    retVal = retVal;  /* suppress unused warning */
    return 0;
}

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

#include "notemodel.h"
#include "logger/qslog.h"
#include "global.h"
#include "sql/nsqlquery.h"

#include <QString>
#include <QSqlDatabase>
#include <QtSql>


//****************************************************
//* This class is used to read & write information
//* to the NoteModel class.  This class holds what
//* a user sees in the table listing in the GUI.
//****************************************************

extern Global global;


// Generic constructor
NoteModel::NoteModel(QObject *parent)
    : QSqlTableModel(parent, global.db->conn) {
    // Check if the table exists.  If not, create it.
    NSqlQuery sql(global.db);

    sql.exec("Select * from sqlite_master where type='table' and name='NoteTable';");
    if (!sql.next())
        this->createNoteTable();

    sql.exec("Select * from sqlite_master where type='view' and name='NoteTableV';");
    if (!sql.next())
        this->createNoteTableV();

    sql.finish();
    this->setEditStrategy(QSqlTableModel::OnFieldChange);

    this->setTable("NoteTableV");
    this->setFilter("lid in (select lid from filter)");
}

QString NoteModel::orderByClause() const
{
    return "order by relevance desc, isDirty desc, dateUpdated desc";
}


// Destructor
NoteModel::~NoteModel() {
}




//* Create the NoteModel table.
void NoteModel::createNoteTable() {
    QLOG_TRACE() << "Entering NoteModel::createTable()";

    QLOG_DEBUG() << "Creating table NoteTable";
    NSqlQuery sql(global.db);
    QString command(
        "Create table if not exists NoteTable ("
            "lid integer primary key,"
            "dateCreated real default null,"
            "dateUpdated real default null,"
            "title text default null collate nocase,"
            "notebookLid integer default null,"
            "notebook text default null collate nocase,"
            "tags text default null collate nocase,"
            "author text default null collate nocase,"
            "dateSubject real default null,"
            "dateDeleted real default null,"
            "source text default null collate nocase,"
            "sourceUrl text default null collate nocase,"
            "sourceApplication text default null collate nocase,"
            "latitude real default null,"
            "longitude real default null,"
            "altitude real default null,"
            "hasEncryption integer default null,"
            "hasTodo integer default null,"
            "isDirty integer default null,"
            "size integer default null,"
            "reminderOrder real default null,"
            "reminderTime real default null,"
            "reminderDoneTime real default null,"
            "isPinned integer default null,"
            "titleColor text default null,"
            "thumbnail default null"
            ")");
    if (!sql.exec(command) ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Title_Index on NoteTable (title)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Author_Index on NoteTable (author)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Notebook_Index on NoteTable (notebook)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Notebook_Lid_Index on NoteTable (notebookLid)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_DateCreated_Index on NoteTable (dateCreated)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_DateUpdated_Index on NoteTable (dateUpdated)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Date_Subject_Index on NoteTable (dateSubject)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Date_Deleted_Index on NoteTable (dateDeleted)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Source_Index on NoteTable (source)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Source_Url_Index on NoteTable (sourceUrl)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Source_Application_Index on NoteTable (sourceApplication)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Latitude_Index on NoteTable (latitude)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Longitude_Index on NoteTable (longitude)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Altitude_Index on NoteTable (altitude)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Has_Encryption_Index on NoteTable (hasEncryption)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Has_Todo_Index on NoteTable (hasTodo)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Is_Dirty_Index on NoteTable (isDirty)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Reminder_Order_Index on NoteTable (reminderOrder)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Reminder_Time_Index on NoteTable (reminderTime)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_isPinned_Index on NoteTable (isPinned)") ||
        !sql.exec("CREATE INDEX if not exists NoteTable_Reminder_Done_Time_Index on NoteTable (reminderDoneTime)")
        ) {
        QLOG_ERROR() << "Creation of NoteTable table failed: " << sql.lastError();
    }
    sql.finish();
}


//* Create the NoteModel table.
void NoteModel::createNoteTableV() {
    QLOG_DEBUG() << "Creating table NoteTableV";
    NSqlQuery sql(global.db);
    sql.exec("create view NoteTableV as select lid,dateCreated,dateUpdated,title,notebookLid,notebook,tags,author,"
                 "dateSubject,dateDeleted,source,sourceUrl,sourceApplication,latitude,longitude,altitude,"
                 "hasEncryption,hasTodo,isDirty,size,reminderOrder,reminderTime,reminderDoneTime,"
                 "isPinned,titleColor,thumbnail,(select f.relevance from filter f where f.lid=n.lid) as relevance "
                 "from NoteTable n");
    sql.finish();
}

//int NoteModel::rowCount(const QModelIndex & /*parent*/) const
// {
//    QLOG_TRACE() << "Entering NoteModel::rowCount()";

//    NSqlQuery sql(*db);
//    sql.exec("Select count(lid) from NoteTable where lid in (select lid from filter)");
//    if (!sql.next()) {
//        QLOG_ERROR() << "Error retrieving NoteModel::rowCount: " << sql.lastError();
//        return 0;
//    }
//    int rowCount = QVariant(sql.value(0)).toInt();
//    return rowCount;

// }

int NoteModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);  // Suppress unused variable
//    parent.column();
    return NOTE_TABLE_COLUMN_COUNT;
}


Qt::ItemFlags NoteModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    // Make the table row non-editable
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}



QVariant NoteModel::data (const QModelIndex & index, int role) const {
    if (role == Qt::ForegroundRole) {
        QString color = index.sibling(index.row(), NOTE_TABLE_COLOR_POSITION).data().toString();
        if (color != "") {
            if (color == "blue" || color == "black")
                return QColor("white");
        }
    }

    if (role == Qt::BackgroundRole) {
        QString color = index.sibling(index.row(), NOTE_TABLE_COLOR_POSITION).data().toString();
        if (color != "") {
            return QColor(color);
        }
    }
    return QSqlTableModel::data(index,role);
}

bool NoteModel::select() {
    //QLOG_DEBUG() << "Performing NoteModel select " << selectStatement();
    return QSqlTableModel::select();
}


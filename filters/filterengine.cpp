/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#include "filterengine.h"
#include "filtercriteria.h"
#include "global.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "sql/tagtable.h"
#include "gui/nattributetree.h"
#include "sql/notebooktable.h"
#include "sql/resourcetable.h"
#include "sql/nsqlquery.h"
#include "sql/favoritesrecord.h"
#include "sql/favoritestable.h"

#include <QtSql>


extern Global global;

FilterEngine::FilterEngine(QObject *parent) :
    QObject(parent)
{
}



// prepare SQL query for selection by tag
// searched string is in "searchStr"
// relevance==0 means we a doing primary search
// relevance>0 means we are doing relevance update search
// "negativeSearch" may only be passed for relevance==0 and means negative search
//
void setupTagSelectionQuery(NSqlQuery &sql, QString searchStr, int relevance, bool negativeSearch) {

    // 0:  search (may then be positive or negative)
    // >0: relevance update
    bool isRelevanceUpdate = relevance > 0;

    // is it wildcard search?
    bool isWildcardSearch = searchStr.contains("*");
    // this is a bit hack/tinkering - in search we donÄt do automatic right truncating for tags
    // but for relevance update we do for "title", so let do it also for tags
    if (isRelevanceUpdate && !isWildcardSearch) {
        searchStr = searchStr + QString("*");
        isWildcardSearch = true;
    }

    QString cmdStr;
    QString selectionPart;
    if (!isWildcardSearch) {
        selectionPart = QString(
            "("
                "select lid from datastore where key=:notetagkey"
                "  and data in (select lid from DataStore where data=:tagname and key=:tagnamekey)"
                ")"
        );
    } else {
        selectionPart = QString(
            "("
                "select lid from datastore where key=:notetagkey"
                "  and data in (select lid from DataStore where data like :tagname and key=:tagnamekey)"
                ")"
        );

        searchStr = searchStr.replace("*", "%");
    }
    if (!isRelevanceUpdate) {
        // positive search: filter out records where tag is not found => "not"
        // negative search: filter out record where tag IS found
        QString negateStr = negativeSearch ? QString("") : QString("not");
        cmdStr = QString("delete from filter where lid ") + negateStr + QString(" in ") + selectionPart;
    } else {
        // update record where tag IS found
        cmdStr = QString("update filter set relevance=relevance+:relevance where lid in ") + selectionPart;
    }

    sql.prepare(cmdStr);
    QLOG_DEBUG() << "Tag query(" << searchStr << "): " + cmdStr;
    sql.bindValue(":tagname", searchStr);
    sql.bindValue(":tagnamekey", TAG_NAME);
    sql.bindValue(":notetagkey", NOTE_TAG_LID);

    if (isRelevanceUpdate) {
        sql.bindValue(":relevance", relevance);
    }
}

// prepare SQL query for selection by note title
// searched string is in "searchStr"
// relevance==0 means we a doing primary search
// relevance>0 means we are doing relevance update search
void setupTitleSelectionQuery(NSqlQuery &sql, QString searchStr, int relevance) {
    searchStr = searchStr.replace("*", "%");

    // default to right truncation
    if (!searchStr.endsWith("%"))
        searchStr = searchStr + QString("%");

    // we also require left truncation, which is a bit unlucky (may also find what we don't expect)
    if (!searchStr.startsWith("%"))
        searchStr = QString("%") + searchStr;

    QLOG_DEBUG() << "In title recheck by " << searchStr;
    sql.prepare(
        "update filter set relevance=relevance+:relevance where lid in (select lid from datastore "
            "where key=:key and data like :title)");
    sql.bindValue(":key", NOTE_TITLE);
    sql.bindValue(":title", searchStr);
    sql.bindValue(":relevance", relevance);
}


void FilterEngine::filter(FilterCriteria *newCriteria, QList <qint32> *results) {
    QLOG_TRACE_IN();
    bool internalSearch = true;

    NSqlQuery sql(global.db);
    QLOG_DEBUG() << "Purging filters";
    sql.exec("delete from filter");
    QLOG_DEBUG() << "Resetting filter table";
    sql.prepare("Insert into filter (lid,relevance) "
                    "select lid,0 from NoteTable where notebooklid not in "
                    "(select lid from datastore where key=:closedNotebooks)");
    sql.bindValue(":closedNotebooks", NOTEBOOK_IS_CLOSED);
    sql.exec();
    sql.finish();
    QLOG_DEBUG() << "Reset complete";


    FilterCriteria *criteria = newCriteria;
    if (criteria == NULL)
        criteria = global.filterCriteria[global.filterPosition];
    else
        internalSearch = false;

    QLOG_DEBUG() << "Filtering favorite";
    filterFavorite(criteria);
    QLOG_DEBUG() << "Filtering notebooks";
    filterNotebook(criteria);
    QLOG_DEBUG() << "Filtering tags";
    filterTags(criteria);
    QLOG_DEBUG() << "Filtering trash";
    filterTrash(criteria);
    QLOG_DEBUG() << "Filtering search string";
    filterSearchString(criteria);
    QLOG_DEBUG() << "Filtering attributes";
    filterAttributes(criteria);
    QLOG_DEBUG() << "Filtering complete";

    // Now, re-insert any pinned notes
    sql.prepare("Insert into filter (lid,relevance) select lid,1 from Datastore "
                    "where key=:key and lid not in (select lid from filter)");
    sql.bindValue(":key", NOTE_ISPINNED);
    sql.exec();

    // Remove any selected notes that are not in the filter.
    NSqlQuery query(global.db);
    QList <qint32> goodLids;
    query.exec("select lid from filter;");
    while (query.next()) {
        goodLids.append(query.value(0).toInt());
    }
    query.finish();

    if (internalSearch) {
        // Remove any selected notes that are not in the filter.
        if (global.filterCriteria.size() > 0) {
            FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
            QList <qint32> selectedLids;
            criteria->getSelectedNotes(selectedLids);
            for (int i = selectedLids.size() - 1; i >= 0; i--) {
                if (!goodLids.contains(selectedLids[i]))
                    selectedLids.removeAll(selectedLids[i]);
            }
            criteria->setSelectedNotes(selectedLids);
        }
    } else {
        results->clear();
        for (int i = 0; i < goodLids.size(); i++) {
            if (!results->contains(goodLids[i])) {
                results->append(goodLids[i]);
            }
        }
    }
}



void FilterEngine::filterAttributes(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isAttributeSet())
        return;
    QLOG_TRACE_IN();

    int attribute = criteria->getAttribute()->data(0,Qt::UserRole).toInt();
    NSqlQuery sql(global.db);
    QDateTime dt;
    dt.setDate(QDate().currentDate());
    int dow = QDate().currentDate().dayOfWeek();
    int moy = QDate().currentDate().month();
    int dom = QDate().currentDate().day();
    dt.setTime(QTime(0,0,0,1));

    switch (attribute)
    {
    case CREATED_SINCE_TODAY:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_YESTERDAY:
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_THIS_WEEK:
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_LAST_WEEK:
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_THIS_MONTH:
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_LAST_MONTH:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_THIS_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_SINCE_LAST_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_TODAY:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_YESTERDAY:
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_THIS_WEEK:
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_LAST_WEEK:
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_THIS_MONTH:
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_LAST_MONTH:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_THIS_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CREATED_BEFORE_LAST_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_TODAY:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_YESTERDAY:
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_THIS_WEEK:
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_LAST_WEEK:
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_THIS_MONTH:
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_LAST_MONTH:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_THIS_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_SINCE_LAST_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_TODAY:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_YESTERDAY:
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_THIS_WEEK:
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_LAST_WEEK:
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_THIS_MONTH:
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_LAST_MONTH:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_THIS_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case MODIFIED_BEFORE_LAST_YEAR:
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
        break;
    case CONTAINS_IMAGES:
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like 'image/%'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        break;
    case CONTAINS_AUDIO:
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like 'audio/%'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        break;
    case CONTAINS_INK:
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data = 'application/vnd.evernote.ink'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        break;
    case CONTAINS_ENCRYPTED_TEXT:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:encryptedkey)");
        sql.bindValue(":encryptedkey", NOTE_HAS_ENCRYPT);
        break;
    case CONTAINS_TODO_ITEMS:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where (key=:comp or key=:uncomp) and data=1)");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
        sql.bindValue(":uncomp", NOTE_HAS_TODO_UNCOMPLETED);
        break;
    case CONTAINS_FINISHED_TODO_ITEMS:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:comp and data=1)");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
        break;
    case CONTAINS_UNFINISHED_TODO_ITEMS:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:uncomp and data=1)");
        sql.bindValue(":uncomp", NOTE_HAS_TODO_UNCOMPLETED);
        break;
    case CONTAINS_PDF_DOCUMENT:
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data ='application/pdf'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        break;
    case CONTAINS_ATTACHMENT:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key)");
        sql.bindValue(":key", NOTE_HAS_ATTACHMENT);
        break;
    case CONTAINS_REMINDER:
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
            break;
    case CONTAINS_UNCOMPLETED_REMINDER:
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
            sql.exec();
            sql.prepare("delete from filter where lid in (select lid from datastore where key=:key and data>0)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
            break;
    case CONTAINS_FUTURE_REMINDER:
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data>:dt)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
            sql.bindValue(":dt",QDateTime::currentMSecsSinceEpoch());
            break;
    case SOURCE_EMAIL:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'mail.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        break;
    case SOURCE_EMAILED_TO_EVERNOTE:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'mail.smtp')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        break;
    case SOURCE_MOBILE:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like 'mobile.%')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        break;
    case SOURCE_WEB_PAGE:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'web.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        break;
    case SOURCE_ANOTHER_APPLICATION:
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data != 'web.clip' and "
                    "data not like 'mobile.%' and data != 'mail.smtp' and data != 'mail.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        break;
    }

    sql.exec();
    sql.finish();
}



void FilterEngine::filterFavorite(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isFavoriteSet())
        return;
    QLOG_TRACE_IN();

    FavoritesTable ftable(global.db);
    FavoritesRecord rec;
    if (!ftable.get(rec, criteria->getFavorite()))
        return;

    if (rec.type == FavoritesRecord::ConflictNotebook ||
        rec.type == FavoritesRecord::LocalNotebook ||
        rec.type == FavoritesRecord::LinkedNotebook ||
        rec.type == FavoritesRecord::SharedNotebook ||
        rec.type == FavoritesRecord::SynchronizedNotebook) {
        qint32 notebookLid = rec.target.toInt();
        NotebookTable ntable(global.db);
        QString guid="";
        if (ntable.getGuid(guid, notebookLid)) {
            filterIndividualNotebook(guid);
        }
        return;
    }

    if (rec.type == FavoritesRecord::NotebookStack ||
        rec.type == FavoritesRecord::LinkedStack) {
        QString stackname = rec.target.toString();
        filterStack(stackname);
        return;
    }

    if (rec.type == FavoritesRecord::Tag) {
        NoteTable noteTable(global.db);
        TagTable tagTable(global.db);
        NSqlQuery sql(global.db);
        sql.exec("create temporary table if not exists goodLids (lid integer)");
        sql.exec("delete from goodLids");
        QList<qint32> notes;
        QString tagGuid="";
        tagTable.getGuid(tagGuid, rec.target.toInt());
        noteTable.getNotesWithTag(notes, tagGuid);
        sql.prepare("insert into goodLids (lid) values (:note)");
        for (qint32 i=0; i<notes.size(); i++) {
            sql.bindValue(":note", notes[i]);
            sql.exec();
        }
        sql.exec("delete from filter where lid not in (select lid from goodLids)" );
        sql.finish();
    }

    if (rec.type == FavoritesRecord::Note) {
        NSqlQuery sql(global.db);
        sql.prepare("delete from filter where lid <> :lid");
        sql.bindValue(":lid", rec.target);
        sql.exec();
        sql.finish();
    }

}

void FilterEngine::filterNotebook(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isNotebookSet())
        return;
    QLOG_TRACE_IN();

    QString stackTest = criteria->getNotebook()->data(0,Qt::UserRole).toString();
    if (stackTest == "STACK") {
        QString stackName = criteria->getNotebook()->text(0);
        filterStack(stackName);
    } else {
        FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
        qint32 notebookLid = criteria->getNotebook()->data(0,Qt::UserRole).toInt();
        NotebookTable notebookTable(global.db);
        QString notebook;
        notebookTable.getGuid(notebook, notebookLid);
        filterIndividualNotebook(notebook);
    }
}




// If they only chose one notebook, then delete everything else
void FilterEngine::filterIndividualNotebook(QString &notebook) {
    QLOG_TRACE_IN();
    NotebookTable notebookTable(global.db);
    qint32 notebookLid = notebookTable.getLid(notebook);
    // Filter out the records
    NSqlQuery sql(global.db);
    sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data=:notebookLid)");
    sql.bindValue(":type", NOTE_NOTEBOOK_LID);
    sql.bindValue(":notebookLid", notebookLid);
    sql.exec();
    sql.finish();
}


// Filter based on stack name
void FilterEngine::filterStack(QString &stack) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (stack.startsWith("-")) {
        negative=true;
        stack = stack.mid(1);
    }
    if (stack.startsWith("stack:"))
        stack = stack.mid(stack.indexOf("stack:")+6);

    NotebookTable notebookTable(global.db);
    QList<qint32> books;
    QList<qint32> stackBooks;
    notebookTable.getAll(books);
    notebookTable.getStack(stackBooks, stack);

    NSqlQuery sql(global.db);
    if (negative) {
        sql.exec("create temporary table if not exists goodLids (lid integer)");
        sql.exec("delete from goodLids");
        sql.prepare("insert into goodLids (lid) select lid from DataStore where key=:key");
        sql.bindValue(":key", NOTEBOOK_GUID);
        sql.exec();
        sql.prepare("delete from goodLids where lid=:notebookLid");
    } else {
        sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:type and data=:notebookLid)");
        sql.bindValue(":type", NOTE_NOTEBOOK_LID);
    }

    for (qint32 i=0; i<books.size(); i++) {
        if (!stackBooks.contains(books[i])) {
            sql.bindValue(":notebookLid", books[i]);
            sql.exec();
        }
    }

    if (negative) {
        sql.prepare("delete from filter where lid in (select lid from DataStore where key=:key and data in (select lid from goodLids))");
        sql.bindValue(":key", NOTE_NOTEBOOK_LID);
        sql.exec();
    }
    sql.finish();
}



// Filter tags
void FilterEngine::filterTags(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isTagsSet())
        return;
    QLOG_TRACE_IN();
    QList<QTreeWidgetItem*> tags = criteria->getTags();

    if (!global.getTagSelectionOr()) {
        NSqlQuery query(global.db);
        for (qint32 i=0; i<tags.size(); i++) {
            query.prepare("Delete from filter where lid not in (select lid from datastore where key=:notetagkey and data=:data)");
            query.bindValue(":notetagkey", NOTE_TAG_LID);
            query.bindValue(":data", tags[i]->data(0,Qt::UserRole).toInt())  ;
            query.exec();
        }
        query.finish();
    } else {
        NoteTable noteTable(global.db);
        TagTable tagTable(global.db);
        QList<qint32> goodNotes;
        for (qint32 i=0; i<tags.size(); i++) {
            QList<qint32> notes;
            QString tagGuid;
            tagTable.getGuid(tagGuid, tags[i]->data(0,Qt::UserRole).toInt());
            noteTable.getNotesWithTag(notes, tagGuid);
            for (qint32 j=0; j<notes.size(); j++) {
                if (!goodNotes.contains(notes[j]))
                    goodNotes.append(notes[j]);
            }
        }

        NSqlQuery sql(global.db);
        sql.exec("create temporary table if not exists goodLids (lid integer)");
        sql.exec("delete from goodLids");
        sql.prepare("insert into goodLids (lid) values (:note)");
        for (qint32 i=0; i<goodNotes.size(); i++) {
            sql.bindValue(":note", goodNotes[i]);
            sql.exec();
        }
        sql.exec("delete from filter where lid not in (select lid from goodLids)" );
        sql.finish();
    }
}


// If they only chose one tag, then delete everything else
void FilterEngine::filterTrash(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isDeletedOnlySet()
            || (criteria->isDeletedOnlySet() && !criteria->getDeletedOnly()))
    {
        NSqlQuery sql(global.db);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data=1)");
        sql.bindValue(":type", NOTE_ACTIVE);
        sql.exec();
        sql.finish();
        return;
    }
    if (!criteria->getDeletedOnly())
        return;

    // Filter out the records
    NSqlQuery sql(global.db);
    sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data=0)");
    sql.bindValue(":type", NOTE_ACTIVE);
    sql.exec();
    sql.finish();
}



// Filter the data based upon what the user enters in the search string
void FilterEngine::filterSearchString(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isSearchStringSet() ||
            criteria->getSearchString().trimmed() == "")
        return;
    QLOG_TRACE_IN();

    anyFlagSet = false;
    if (criteria->getSearchString().trimmed().startsWith("any:", Qt::CaseInsensitive))
        anyFlagSet = true;

    // Tokenize out the words
    QStringList list;
    QLOG_DEBUG() << "Original String Search: " << criteria->getSearchString();
    splitSearchTerms(list, criteria->getSearchString());

    if (!anyFlagSet)
        filterSearchStringAll(list);
    else
        filterSearchStringAny(list);
}


// Split the search term into specific tokens.
void FilterEngine::splitSearchTerms(QStringList &words, QString search) {
    QLOG_TRACE_IN();
    words.clear();


    // First go through the string and put null characters between
    // the search terms.  This helps parse out the terms later, since
    // some may be in quotes
    qint32 len = search.length();
    char nextChar = ' ';
    bool quote = false;
    for (qint32 i = 0; i < len; i++) {
        if (search[i] == nextChar && !quote) {
            search[i] = '\0';
            nextChar = ' ';
        } else {
            if (search[i] == '\"') {
                if (!quote) {
                    quote = true;
                } else {
                    quote = false;
                }
            }
        }
        if (((i + 2) < len) && search[i] == '\\') {
            i = i + 2;
        }
    }

    // Now that we have null characters between them, we parse
    // out based upon them rather than spaces.
    qint32 pos = 0;
    for (qint32 i = 0; i < search.length() && search.length() > 0; i++) {
        if (search[i] == '\0') {
            search = search.remove(0, 1);
            i = -1;
        } else {
            pos = search.indexOf(QChar('\0'));
            if (pos != -1) {
                words.append(search.left(pos).toLower());
                search.remove(0, pos);
                i = -1;
            } else {
                words.append(search.toLower());
                search = "";
            }
        }
    }

    // Now that we have everything separated, we can remove the unneeded " marks
    for (qint32 i = 0; i < words.length(); i++) {
        words[i].remove("\"");
    }
}


// Filter based upon the words the user specified (as opposed to the notebook, tags ...)
// this is for the "all" filter (the default), not the "any:"
void FilterEngine::filterSearchStringAll(QStringList list) {
    QLOG_TRACE_IN();

    // Filter out the records
    NSqlQuery sql(global.db), sqlnegative(global.db);

    sql.prepare(
        "Delete from filter where lid not in "
            "(select lid from SearchIndex where weight>=:weight and content match :word)"
            "and lid not in (select data from DataStore where key=:key and lid in "
            "(select lid from SearchIndex where weight>=:weight2 and content match :word2))");
    sqlnegative.prepare(
        "Delete from filter where lid in "
            "(select lid from SearchIndex where weight>=:weight and content match :word)"
            " or lid in (select data from DataStore where key=:key and lid in "
            "(select lid from SearchIndex where weight>=:weight2 and content match :word2))");

    sql.bindValue(":weight", global.getMinimumRecognitionWeight());
    sql.bindValue(":weight2", global.getMinimumRecognitionWeight());
    sql.bindValue(":key", RESOURCE_NOTE_LID);

    sqlnegative.bindValue(":weight", global.getMinimumRecognitionWeight());
    sqlnegative.bindValue(":weight2", global.getMinimumRecognitionWeight());
    sqlnegative.bindValue(":key", RESOURCE_NOTE_LID);

    for (qint32 i = 0; i < list.size(); i++) {
        QString string = list[i];
        string.remove(QChar('"'));

        QString origString(string); // copy original unmodified value

        // If we have a notebook search request
        if (string.startsWith("notebook:", Qt::CaseInsensitive) ||
            string.startsWith("-notebook:", Qt::CaseInsensitive)) {
            filterSearchStringNotebookAll(string);
        } else if (string.startsWith("stack:", Qt::CaseInsensitive) ||
                   string.startsWith("-stack:", Qt::CaseInsensitive)) {
            filterStack(string);
        } else if (string.startsWith("todo:", Qt::CaseInsensitive) ||
                   string.startsWith("-todo:", Qt::CaseInsensitive)) {
            filterSearchStringTodoAll(string);
        } else if (string.startsWith("reminderOrder:", Qt::CaseInsensitive) ||
                   string.startsWith("-reminderOrder:", Qt::CaseInsensitive)) {
            filterSearchStringReminderOrderAll(string);
        } else if (string.startsWith("reminderTime:", Qt::CaseInsensitive) ||
                   string.startsWith("-reminderTime:", Qt::CaseInsensitive)) {
            filterSearchStringReminderTimeAll(string);
        } else if (string.startsWith("reminderDoneTime:", Qt::CaseInsensitive) ||
                   string.startsWith("-reminderDoneTime:", Qt::CaseInsensitive)) {
            filterSearchStringReminderDoneTimeAll(string);
        } else if (string.startsWith("tag:", Qt::CaseInsensitive) ||
                   string.startsWith("-tag:", Qt::CaseInsensitive)) {
            filterSearchStringTagAll(string);
        } else if (string.startsWith("intitle:", Qt::CaseInsensitive) ||
                   string.startsWith("-intitle:", Qt::CaseInsensitive)) {
            filterSearchStringIntitleAll(string);
        } else if (string.startsWith("resource:", Qt::CaseInsensitive) ||
                   string.startsWith("-resource:", Qt::CaseInsensitive)) {
            filterSearchStringResourceAll(string);
        } else if (string.startsWith("longitude:", Qt::CaseInsensitive) ||
                   string.startsWith("-longitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_LONGITUDE);
        } else if (string.startsWith("latitude:", Qt::CaseInsensitive) ||
                   string.startsWith("-latitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_LATITUDE);
        } else if (string.startsWith("altitude:", Qt::CaseInsensitive) ||
                   string.startsWith("-altitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_ALTITUDE);
        } else if (string.startsWith("author:", Qt::CaseInsensitive) ||
                   string.startsWith("-author:", Qt::CaseInsensitive)) {
            filterSearchStringAuthorAll(string);
        } else if (string.startsWith("source:", Qt::CaseInsensitive) ||
                   string.startsWith("-source:", Qt::CaseInsensitive)) {
            filterSearchStringSourceAll(string);
        } else if (string.startsWith("sourceapplication:", Qt::CaseInsensitive) ||
                   string.startsWith("-sourceapplication:", Qt::CaseInsensitive)) {
            filterSearchStringSourceApplicationAll(string);
        } else if (string.startsWith("contentclass:", Qt::CaseInsensitive) ||
                   string.startsWith("-contentclass:", Qt::CaseInsensitive)) {
            filterSearchStringContentClassAll(string);
        } else if (string.startsWith("recotype:", Qt::CaseInsensitive) ||
                   string.startsWith("-recotype:", Qt::CaseInsensitive)) {
            filterSearchStringResourceRecognitionTypeAll(string);
        } else if (string.startsWith("placename:", Qt::CaseInsensitive) ||
                   string.startsWith("-placename:", Qt::CaseInsensitive)) {
            filterSearchStringContentClassAll(string);
        } else if (string.startsWith("created:", Qt::CaseInsensitive) ||
                   string.startsWith("-created:", Qt::CaseInsensitive) ||
                   string.startsWith("updated:", Qt::CaseInsensitive) ||
                   string.startsWith("-updated:", Qt::CaseInsensitive) ||
                   string.startsWith("subjectdate:", Qt::CaseInsensitive) ||
                   string.startsWith("-subjectdate:", Qt::CaseInsensitive)) {
            filterSearchStringDateAll(string);
        } else if (string.startsWith("-*")) {   // Negative postfix search.  FTS doesn't do this.
            string = string.mid(1);
            string = string.replace("*", "%");
            if (!string.endsWith("%"))
                string = string + QString("%");
            NSqlQuery prefix(global.db);
            prefix.prepare(
                "Delete from filter where lid in (select lid from SearchIndex where weight>=:weight "
                    "and content like :word) or lid in (select data from DataStore where lid in "
                    "(select lid from SearchIndex where weight>:weight2 and content like :word2))");

            prefix.bindValue(":weight", global.getMinimumRecognitionWeight());
            prefix.bindValue(":weight2", global.getMinimumRecognitionWeight());
            prefix.bindValue(":word", string);
            prefix.bindValue(":word2", string);
            prefix.exec();
        } else if (string.indexOf("_") >= 0) {    // underscore search.  FTS doesn't do this.
            string = string.replace("_", "/_");
            string = string.replace("*", "%");
            if (!string.endsWith("%"))
                string = string + QString("%");
            if (!string.startsWith("%"))
                string = QString("%") + string;
            NSqlQuery prefix(global.db);
            prefix.prepare(
                "Delete from filter where lid not in (select lid from SearchIndex where weight>=:weight "
                    "and content like :word escape '/') and "
                    "lid not in (select data from DataStore where key=:key and lid in (select lid from SearchIndex "
                    "where weight>:weight2 and content like :word2 escape '/'))");

            prefix.bindValue(":weight", global.getMinimumRecognitionWeight());
            prefix.bindValue(":weight2", global.getMinimumRecognitionWeight());
            prefix.bindValue(":word", string);
            prefix.bindValue(":word2", string);
            prefix.bindValue(":key", RESOURCE_NOTE_LID);
            prefix.exec();
        } else if (string.indexOf("-") >= 0) {    // Hyphen search.  FTS doesn't do this.
            string = string.replace("*", "%");
            if (!string.endsWith("%"))
                string = string + QString("%");
            if (!string.startsWith("%"))
                string = QString("%") + string;
            NSqlQuery prefix(global.db);
            prefix.prepare(
                "Delete from filter where lid not in (select lid from SearchIndex where weight>=:weight and content "
                    "like :word) and lid not in (select data from DataStore where key=:key and lid in (select lid "
                    "from SearchIndex where weight>:weight2 and content like :word2))");

            prefix.bindValue(":weight", global.getMinimumRecognitionWeight());
            prefix.bindValue(":weight2", global.getMinimumRecognitionWeight());
            prefix.bindValue(":word", string);
            prefix.bindValue(":word2", string);
            prefix.bindValue(":key", RESOURCE_NOTE_LID);
            prefix.exec();
        } else if (string.startsWith("*")) {    // Postfix search.  FTS doesn't do this.
            string = string.replace("*", "%");
            if (!string.endsWith("%"))
                string = string + QString("%");
            NSqlQuery prefix(global.db);
            prefix.prepare(
                "Delete from filter where lid not in (select lid from SearchIndex where weight>=:weight and content "
                    "like :word) and lid not in (select data from DataStore where key=:key and lid in (select lid "
                    "from SearchIndex where weight>:weight2 and content like :word2))");

            prefix.bindValue(":weight", global.getMinimumRecognitionWeight());
            prefix.bindValue(":weight2", global.getMinimumRecognitionWeight());
            prefix.bindValue(":word", string);
            prefix.bindValue(":word2", string);
            prefix.bindValue(":key", RESOURCE_NOTE_LID);
            prefix.exec();
        } else {
            // Filter not found.  Use FTS search (full text search)

            QLOG_TRACE() << "Using FTS search";
            if (string.startsWith("-")) {
                string = string.remove(0, 1).trimmed();
                if (!string.endsWith("*"))
                    string = string + QString("*");
                if (string.contains(" "))
                    string = "\"" + string + "\"";
                sqlnegative.bindValue(":key", RESOURCE_NOTE_LID);
                sqlnegative.bindValue(":word", string);
                sqlnegative.bindValue(":word2", string);
                sqlnegative.exec();
            } else {
                if (!string.endsWith("*"))
                    string = string + QString("*");
                if (string.contains(" "))
                    string = "\"" + string + "\"";
                sql.bindValue(":key", RESOURCE_NOTE_LID);
                sql.bindValue(":word", string);
                sql.bindValue(":word2", string);
                sql.exec();

            }


            // update relevance by +1 where search term is found in title
            NSqlQuery relUpdtSql(global.db);
            setupTitleSelectionQuery(relUpdtSql, origString, 1);
            relUpdtSql.exec();

            // update relevance by +1 where search term is found as tag value
            setupTagSelectionQuery(relUpdtSql, origString, 1, true);
            relUpdtSql.exec();

            relUpdtSql.finish();
        }
    }
    sql.finish();
}


// filter based upon the title string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringIntitleAll(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        // in" title

        string.remove(0, 8);    // remove 8 chars of "intitle:"
        if (string == "")
            string = "*";

        // Filter out the records
        NSqlQuery tagSql(global.db);
        string = string.replace("*", "%");

        // default to right truncation
        if (!string.endsWith("%"))
            string = string + QString("%");

        // we also require left truncation, which is a bit unlucky (may also find what we don't expect)
        if (!string.startsWith("%"))
            string = QString("%") + string;

        QLOG_DEBUG() << "In title search by " << string;

        tagSql.prepare(
            "Delete from filter where lid not in (select lid from datastore where key=:key and data like :title)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":title", string);

        tagSql.exec();
        tagSql.finish();
    } else {
        // NOT "in" title
        string.remove(0, 9); // remove 9 chars of "!intitle:"
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery tagSql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            string = QString("%") + string + QString("%");


        tagSql.prepare(
            "Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":data", string);

        tagSql.exec();
        tagSql.finish();
    }
}




// filter based upon the note coordinates the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringCoordinatesAll(QString string, int key) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "0";
        // Filter out the records
        NSqlQuery sql(global.db);
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data >= :data)");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "0";
        // Filter out the records
        NSqlQuery sql(global.db);
        sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data <= :data)");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}


// filter based upon the note author the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringAuthorAll(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




// filter based upon the note source the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringSourceAll(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




// filter based upon the note content class the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringContentClassAll(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}


// filter based upon the note content class the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringPlaceNameAll(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




// filter based upon the note source application the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringSourceApplicationAll(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}





// filter based upon the mime type the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringResourceAll(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        else
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        else
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    }
}



// filter based upon the mime type the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringResourceRecognitionTypeAll(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        else
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        else
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    }
}




// filter based upon the tag string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringTagAll(QString string) {
    QLOG_TRACE_IN();
    NSqlQuery sql(global.db);

    if (!string.startsWith("-")) {
        // positive search
        string.remove(0, 4);
        if (string == "")
            string = "*";

        // Filter out the records
        setupTagSelectionQuery(sql, string, 0, false);
    } else {
        // negative search
        string.remove(0, 5);
        if (string == "")
            string = "*";

        // Filter out the records
        setupTagSelectionQuery(sql, string, 0, true);
    }
    sql.exec();
    sql.finish();
}



// filter based upon the notebook string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringNotebookAll(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery notebookSql(global.db);
        if (not string.contains("*"))
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook = :notebook)");
        else {
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook like :notebook)");
            string.replace("*", "%");
        }
//        notebookSql.bindValue(":type", NOTE_NOTEBOOK_LID);
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
        notebookSql.finish();

    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery notebookSql(global.db);
        if (not string.contains("*"))
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook <> :notebook)");
        else {
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook not like :notebook)");
            string.replace("*", "%");
        }
        //notebookSql.bindValue(":type", NOTE_NOTEBOOK);
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
        notebookSql.finish();
    }
}


// filter based upon the notebook string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringTodoAll(QString string) {
    QLOG_TRACE_IN();

    if (!global.forceSearchLowerCase)
        string = string.toLower();

    if (!string.startsWith("-")) {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1 or key=:key2)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        else if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        else if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,6);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1 or key=:key2)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        else if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        else if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
        sql.finish();
    }
}


//#define NOTE_ATTRIBUTE_REMINDER_ORDER          5032
//#define NOTE_ATTRIBUTE_REMINDER_TIME           5033
//#define NOTE_ATTRIBUTE_REMINDER_DONE_TIME      5034



// filter based upon the reminder string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringReminderOrderAll(QString string) {
    QLOG_TRACE_IN();

    if (!global.forceSearchLowerCase)
        string = string.toLower();
    if (!string.startsWith("-")) {
        string.remove(0,14);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
        } else {
            int data= string.toInt();
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1 and data=:data)");
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
            sql.bindValue(":data", data);
        }
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,15);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
        } else {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1 and data=:data)");
            int data = string.toInt();
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
            sql.bindValue(":data", data);
        }
        sql.exec();
        sql.finish();
    }
}





void FilterEngine::filterSearchStringDateAll(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key=0;

    if (string.startsWith("created:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000)))");;
        key = NOTE_CREATED_DATE;
    }
    else if (string.startsWith("updated:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000)))");;
        key = NOTE_UPDATED_DATE;
    }
    else if (string.startsWith("subjectdate:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000)))");;
        key = NOTE_ATTRIBUTE_SUBJECT_DATE;
    }
    else if (string.startsWith("-created:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000)))");;
        key = NOTE_CREATED_DATE;
    }
    else if (string.startsWith("-updated:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000)))");;
        key = NOTE_UPDATED_DATE;
    }
    else if (string.startsWith("-subjectdate:", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000)))");;
        key = NOTE_ATTRIBUTE_SUBJECT_DATE;
    }

    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
}





QDateTime FilterEngine::calculateDateTime(QString string) {
    QLOG_TRACE_IN();
    QDateTime tam;  // datetime - midnight today
    tam.setDate(QDate().currentDate());
    tam.setTime(QTime(0,0,0,1));

    int dow = QDate().currentDate().dayOfWeek();  // Current day of week
    int moy = QDate().currentDate().month();  // Current month
    int dom = QDate().currentDate().day();   // current day of month

    int offset;
    QDateTime value;
    if (string.startsWith("today")) {
        value = tam;
        string = string.mid(5);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addDays(offset);
        return value;
    } else if (string.startsWith("day")) {
        value = tam;
        string = string.mid(3);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addDays(offset);
        return value;
    }  else if (string.startsWith("month")) {
        value = tam;
        value = value.addDays(-1*dom+1);
        string = string.mid(5);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addMonths(offset);
        QLOG_DEBUG() << value.toString();
        return value;
    } else if (string.startsWith("year")) {
        value = tam;
        value = value.addDays(-1*dom+1);
        value = value.addMonths(-1*moy+1);
        string = string.mid(4);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addYears(offset);
        QLOG_DEBUG() << value.toString();
        return value;
    } else if (string.startsWith("week")) {
        value = tam;
        value = value.addDays(-1*dow);
        string = string.mid(4);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addDays(offset*7);
        QLOG_DEBUG() << value.toString();
        return value;
    }

    // If we've gotten this far then we have some type of number
    int year = string.mid(0,4).toInt();
    int month = string.mid(4,2).toInt();
    int day = string.mid(6,2).toInt();
    int hour = 0;
    int minute = 0;
    int seconds = 0;
    value.setDate(QDate(year, month, day));

    string = string.mid(8);
    if (string.startsWith("t",Qt::CaseInsensitive)) {
        hour = string.mid(1,2).toInt();
        minute = string.mid(3,2).toInt();
        seconds = string.mid(5,2).toInt();
        value.setTime(QTime(hour, minute, seconds, 0));
        if (string.endsWith("z", Qt::CaseInsensitive))
            value = value.toUTC();
    }
    return value;
}





// Filter based upon the words the user specified (as opposed to the notebook, tags ...)
// this is for the "any" filter (the default), not the default of all
void FilterEngine::filterSearchStringAny(QStringList list) {
    QLOG_TRACE_IN();
    // Filter out the records
    NSqlQuery sql(global.db), sqlnegative(global.db);
    NSqlQuery resSql(global.db), resSqlNegative(global.db);

    sql.exec("create table if not exists anylidsfilter (lid int);");
    sql.exec("delete from anylidsfilter");

    sql.exec("create table if not exists anylidsfilterRes (lid int);");
    sql.exec("delete from anylidsfilterRes");

    sql.prepare("insert into anylidsfilter (lid) select lid from SearchIndex where weight>=:weight and source='text' and content match :word");
    resSql.prepare("insert into anylidsfilterRes (lid) select lid from SearchIndex where source='recognition' and weight>=:weight and content match :word");

    sqlnegative.prepare("insert into anylidsfilter (lid) select lid from SearchIndex where lid not in (select lid from searchindex where source='text' and weight>=:weight and content match :word)");
    resSqlNegative.prepare("insert into anylidsfilterRes (lid) select lid from SearchIndex where lid not in (select lid from searchindex where source='recognition' and weight>=:weight and content match :word)");

    sql.bindValue(":weight", global.getMinimumRecognitionWeight());
    sqlnegative.bindValue(":weight", global.getMinimumRecognitionWeight());

    resSql.bindValue(":weight", global.getMinimumRecognitionWeight());
    resSqlNegative.bindValue(":weight", global.getMinimumRecognitionWeight());

    // We start at the second entry because the first is "any:"
    for (qint32 i=1; i<list.size(); i++) {
        QString string = list[i];
        string.remove(QChar('"'));

        // If we have a notebook search request
        if (string.startsWith("notebook:", Qt::CaseInsensitive) ||
                string.startsWith("-notebook:", Qt::CaseInsensitive)) {
            filterSearchStringNotebookAny(string);
        }
        else if (string.startsWith("todo:", Qt::CaseInsensitive) ||
                string.startsWith("-todo:", Qt::CaseInsensitive)) {
            filterSearchStringTodoAny(string);
        }
        else if (string.startsWith("reminderOrder:", Qt::CaseInsensitive) ||
                string.startsWith("-reminderOrder:", Qt::CaseInsensitive)) {
            filterSearchStringReminderOrderAny(string);
        }
        else if (string.startsWith("reminderTime:", Qt::CaseInsensitive) ||
                string.startsWith("-reminderTime:", Qt::CaseInsensitive)) {
            filterSearchStringReminderTimeAny(string);
        }
        else if (string.startsWith("reminderDoneTime:", Qt::CaseInsensitive) ||
                string.startsWith("-reminderDoneTime:", Qt::CaseInsensitive)) {
            filterSearchStringReminderDoneTimeAny(string);
        }
        else if (string.startsWith("tag:", Qt::CaseInsensitive) ||
                string.startsWith("-tag:", Qt::CaseInsensitive)) {
            filterSearchStringTagAny(string);
        }
        else if (string.startsWith("intitle:", Qt::CaseInsensitive) ||
                string.startsWith("-intitle:", Qt::CaseInsensitive)) {
            filterSearchStringIntitleAny(string);
        }
        else if (string.startsWith("resource:", Qt::CaseInsensitive) ||
                string.startsWith("-resource:", Qt::CaseInsensitive)) {
            filterSearchStringResourceAny(string);
        }
        else if (string.startsWith("longitude:", Qt::CaseInsensitive) ||
                string.startsWith("-longitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAny(string, NOTE_ATTRIBUTE_LONGITUDE);
        }
        else if (string.startsWith("latitude:", Qt::CaseInsensitive) ||
                string.startsWith("-latitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAny(string, NOTE_ATTRIBUTE_LATITUDE);
        }
        else if (string.startsWith("altitude:", Qt::CaseInsensitive) ||
                string.startsWith("-altitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAny(string, NOTE_ATTRIBUTE_ALTITUDE);
        }
        else if (string.startsWith("author:", Qt::CaseInsensitive) ||
                string.startsWith("-author:", Qt::CaseInsensitive)) {
            filterSearchStringAuthorAny(string);
        }
        else if (string.startsWith("source:", Qt::CaseInsensitive) ||
                string.startsWith("-source:", Qt::CaseInsensitive)) {
            filterSearchStringSourceAny(string);
        }
        else if (string.startsWith("sourceapplication:", Qt::CaseInsensitive) ||
                string.startsWith("-sourceapplication:", Qt::CaseInsensitive)) {
            filterSearchStringSourceApplicationAny(string);
        }
        else if (string.startsWith("contentclass:", Qt::CaseInsensitive) ||
                string.startsWith("-contentclass:", Qt::CaseInsensitive)) {
            filterSearchStringContentClassAny(string);
        }
        else if (string.startsWith("recotype:", Qt::CaseInsensitive) ||
                string.startsWith("-recotype:", Qt::CaseInsensitive)) {
            filterSearchStringResourceRecognitionTypeAny(string);
        }
        else if (string.startsWith("created:", Qt::CaseInsensitive) ||
                string.startsWith("-created:", Qt::CaseInsensitive) ||
                string.startsWith("updated:", Qt::CaseInsensitive) ||
                string.startsWith("-updated:", Qt::CaseInsensitive) ||
                string.startsWith("subjectDate:", Qt::CaseInsensitive) ||
                string.startsWith("-subjectdate", Qt::CaseInsensitive)) {
            filterSearchStringDateAny(string);
        }
        else { // Filter not found
            if (string.startsWith("-")) {
                string = string.remove(0,1);
                sqlnegative.bindValue(":word", string.trimmed()+"*");
                sqlnegative.exec();
                resSqlNegative.bindValue(":word", string.trimmed()+"*");
                resSqlNegative.exec();
            } else {
                sql.bindValue(":word", string.trimmed()+"*");
                sql.exec();
                resSql.bindValue(":word", string.trimmed()+"*");
                resSql.exec();
            }
        }
    }

    // At this point we have two tables. One has resource LIDs and the other note LIDs.  We need to
    // map the resource LIDs to note LIDs for the filter;
    sql.prepare("insert into anylidsfilter (lid) select a.data from datastore a where a.key=:key and lid in (select lid from anylidsfilterRes)");
    sql.bindValue(":key", RESOURCE_NOTE_LID);
    sql.exec();
    sql.exec("delete from filter where lid not in (select lid from anylidsfilter)");
    sql.finish();
}



// filter based upon the notebook string the user specified.  This is for the "any"
// filter and not the default
void FilterEngine::filterSearchStringNotebookAny(QString string) {
    QLOG_TRACE_IN();

    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery notebookSql(global.db);
        if (not string.contains("*"))
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook=:notebook");
        else {
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook like :notebook");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
        notebookSql.finish();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery notebookSql(global.db);
        if (not string.contains("*"))
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook <> :notebook");
        else {
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook not like :notebook");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
        notebookSql.finish();
    }
}




// filter based upon the notebook string the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringTodoAny(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1 or key=:key2");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,6);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key<>:key1 or key<>:key2");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        sql.exec();
        sql.finish();
    }
}






// filter based upon the reminder: string the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringReminderOrderAny(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,14);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1");
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
        } else {
            int data=string.toInt();
            sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key1 and data=:data");
            sql.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_ORDER);
            sql.bindValue(":data", data);
        }
        sql.exec();
        sql.finish();
    } else {
        string.remove(0,15);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.startsWith("*")) {
            sql.prepare("insert into anylidsfilter (lid) select distinct lid from DataStore where lid not in (select lid from DataStore where key = :key)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
        } else {
            int data = string.toInt();
            sql.prepare("insert into anylidsfilter (lid) select distinct lid from DataStore where lid not in (select lid from DataStore where key = :key and data=:data)");
            sql.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
            sql.bindValue(":data", data);
        }
        sql.exec();
        sql.finish();
    }
}




// filter based upon the tag string the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringTagAny(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,4);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery tagSql(global.db);
        if (not string.contains("*"))
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey)");
        else {
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey)");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);

        tagSql.exec();
        tagSql.finish();
    } else {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery tagSql(global.db);
        if (not string.contains("*"))
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey))");
        else {
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey))");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);
        tagSql.exec();
        tagSql.finish();
    }
}



// filter based upon the title string the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringIntitleAny(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        int pos = string.indexOf(":")+1;
        string = string.mid(pos);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery tagSql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            string = QString("%") +string +QString("%");
        tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :title");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":title", string);

        tagSql.exec();
        tagSql.finish();
    } else {
        int pos = string.indexOf(":")+1;
        string = string.mid(pos);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery tagSql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            string = QString("%") +string +QString("%");
        tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :title)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":title", string);

        tagSql.exec();
        tagSql.finish();
    }
}




// filter based upon the mime type the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringResourceAny(QString string) {
    QLOG_TRACE_IN();
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("insert into anylidsfilter (lid) select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data)");
        else
            sql.prepare("insert into anylidsfilter (lid) select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data)");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        QLOG_DEBUG() << sql.lastError();
        sql.finish();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        string = string.replace("*", "%");
        if (not string.contains("%"))
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select data from datastore where key=:notelid and lid in (select lid from DataStore where data=:data and key = :mimekey))");
        else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select data from datastore where key=:notelid and lid not in (select lid from DataStore where data=:data and key like :mimekey))");
        sql.bindValue(":notelid", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        QLOG_DEBUG() << sql.lastError();
        sql.finish();
    }
}




// filter based upon the note coordinates the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringCoordinatesAny(QString string, int key) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "0";
        // Filter out the records
        NSqlQuery sql(global.db);
        sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data >= :data");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "0";
        // Filter out the records
        NSqlQuery sql(global.db);
        sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data <= :data");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




// filter based upon the note author the user specified.  This is for the "any"
// filter and not the default
void FilterEngine::filterSearchStringAuthorAny(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data = :data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data = :data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




void FilterEngine::filterSearchStringDateAny(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key=0;

    if (string.startsWith("created:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000))");
        key = NOTE_CREATED_DATE;
    }
    else if (string.startsWith("updated:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000))");
        key = NOTE_UPDATED_DATE;
    }
    else if (string.startsWith("subjectdate:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000))");
        key = NOTE_ATTRIBUTE_SUBJECT_DATE;
    }
    else if (string.startsWith("-created:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000))");
        key = NOTE_CREATED_DATE;
    }
    else if (string.startsWith("-updated:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000))");
        key = NOTE_UPDATED_DATE;
    }
    else if (string.startsWith("-subjectdate:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)<=(datetime(:data/1000))");
        key = NOTE_ATTRIBUTE_SUBJECT_DATE;
    }

    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
}






// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringSourceAny(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}




// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringSourceApplicationAny(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}





// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringContentClassAny(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}



// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringResourceRecognitionTypeAny(QString string) {
    QLOG_TRACE_IN();
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string);
        sql.exec();
        sql.finish();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        NSqlQuery sql(global.db);
        if (string.contains("*")) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
        sql.finish();
    }
}



// Check if a resource contains a specific search string.  Used in highlighting PDFs & attachments
// This function is used in two different ways.  If the *returnHits pointer is NULL, it searches
// for the first match in a PDF and exits with true/false if a match is found.  If the pointer is
// not null, it will return a list of all of the searchString terms that are found.  This is useful
// in knowing what to highlight in a PDF.
bool FilterEngine::resourceContains(qint32 resourceLid, QString searchString, QStringList *returnHits) {
    QLOG_TRACE_IN();
    bool returnValue = false;
    if (returnHits != NULL)
        returnHits->empty();
    NSqlQuery query(global.db);
    NSqlQuery query2(global.db);
    query.prepare("select lid from SearchIndex where lid=:resourceLid and weight>=:weight and content match :word");
    query2.prepare("select lid from SearchIndex where lid=:resourceLid and weight>=:weight and content like :word");
    QStringList terms;
    splitSearchTerms(terms, searchString);
    for (int i=0; i<terms.size(); i++) {

        // ignore special search terms
        if (!searchString.startsWith("notebook:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-notebook:", Qt::CaseInsensitive) &&
                !searchString.startsWith("stack:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-stack:", Qt::CaseInsensitive) &&
                !searchString.startsWith("todo:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-todo:", Qt::CaseInsensitive) &&
                !searchString.startsWith("tag:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-tag:", Qt::CaseInsensitive) &&
                !searchString.startsWith("intitle:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-intitle:", Qt::CaseInsensitive) &&
                !searchString.startsWith("resource:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-resource:", Qt::CaseInsensitive) &&
                !searchString.startsWith("longitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-longitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("latitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-latitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("altitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-altitude:", Qt::CaseInsensitive) &&
                !searchString.startsWith("author:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-author:", Qt::CaseInsensitive) &&
                !searchString.startsWith("source:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-source:", Qt::CaseInsensitive) &&
                !searchString.startsWith("sourceapplication:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-sourceapplication:", Qt::CaseInsensitive) &&
                !searchString.startsWith("contentclass:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-contentclass:", Qt::CaseInsensitive) &&
                !searchString.startsWith("recotype:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-recotype:", Qt::CaseInsensitive) &&
                !searchString.startsWith("placename:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-placename:", Qt::CaseInsensitive) &&
                !searchString.startsWith("created:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-created:", Qt::CaseInsensitive) &&
                !searchString.startsWith("updated:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-updated:", Qt::CaseInsensitive) &&
                !searchString.startsWith("subjectdate:", Qt::CaseInsensitive) &&
                !searchString.startsWith("-subjectdate:", Qt::CaseInsensitive)) {
            QString term = terms[i];
            if (term.endsWith("*"))
                term.chop(1);
            if (term.startsWith("*")) {
                term = term.mid(1);
                query2.bindValue(":resourceLid", resourceLid);
                query2.bindValue(":weight", global.getMinimumRecognitionWeight());
                query2.bindValue(":word", "%"+term+"%");
                query2.exec();
                if (query2.next()) {
                    returnValue = true;
                    if (returnHits != NULL)
                        returnHits->append(term);
                    else
                        return true;
                }
            } else {
                query.bindValue(":resourceLid", resourceLid);
                query.bindValue(":weight", global.getMinimumRecognitionWeight());
                query.bindValue(":word", term);
                query.exec();
                if (query.next()) {
                    returnValue = true;
                    if (returnHits != NULL)
                        returnHits->append(term);
                    else
                        return true;
                }
            }
        }
    }
    return returnValue;
}




// Filter based on reminder time
void FilterEngine::filterSearchStringReminderTimeAll(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key= NOTE_ATTRIBUTE_REMINDER_TIME;

    if (string.startsWith("-", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
    } else {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000)))");;
    }

    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
    QLOG_TRACE_OUT();
}


// Filter based on reminder time
void FilterEngine::filterSearchStringReminderTimeAny(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key = NOTE_ATTRIBUTE_REMINDER_TIME;

    if (string.startsWith("-reminderDoneTime:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000))");
    } else {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000))");
    }
    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
}



// Filter based on reminder done time
void FilterEngine::filterSearchStringReminderDoneTimeAll(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key = NOTE_ATTRIBUTE_REMINDER_DONE_TIME;

    if (string.startsWith("-", Qt::CaseInsensitive)) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");
    } else {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000)))");
    }

    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
    QLOG_TRACE_OUT();
}


// Filter based on reminder done time
void FilterEngine::filterSearchStringReminderDoneTimeAny(QString string) {
    QLOG_TRACE_IN();
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    NSqlQuery sql(global.db);
    int key = NOTE_ATTRIBUTE_REMINDER_DONE_TIME;

    if (string.startsWith("-reminderDoneTime:", Qt::CaseInsensitive)) {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000))");
    } else {
        sql.prepare("insert into anylidsfilter (lid) select lid from DataStore where key=:key and datetime(data/1000)>=(datetime(:data/1000))");
    }
    sql.bindValue(":key", key);
    sql.bindValue(":data", dt.toMSecsSinceEpoch());
    sql.exec();
    sql.finish();
    QLOG_TRACE_OUT();
}

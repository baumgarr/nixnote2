#include "filterengine.h"
#include "filtercriteria.h"
#include "global.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "sql/tagtable.h"
#include "gui/nattributetree.h"
#include "sql/notebooktable.h"
#include "sql/resourcetable.h"

#include <QtSql>

extern Global global;

FilterEngine::FilterEngine(QObject *parent) :
    QObject(parent)
{
}


void FilterEngine::filter() {

    QSqlQuery sql;
    QLOG_DEBUG() << "Purging filters";
    sql.exec("delete from filter");
    QLOG_DEBUG() << "Resetting filter table";
    sql.exec("Insert into filter (lid) select lid from NoteTable");
    QLOG_DEBUG() << "Reset complete";
    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];

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


    // Remove any selected notes that are not in the filter.
    QList<qint32> oldLids;
    QSqlQuery query;
    query.exec("select lid from filter");
    QList<qint32> goodLids;

    while (query.next()) {
        goodLids.append(query.value(0).toInt());
    }

    // Remove any selected notes that are not in the filter.
    if (global.filterCriteria.size() > 0) {
        FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
        QList<qint32> selectedLids;
        criteria->getSelectedNotes(selectedLids);
        for (int i=selectedLids.size()-1; i>=0; i--) {
            if (!goodLids.contains(selectedLids[i]))
                selectedLids.removeAll(selectedLids[i]);
        }
        criteria->setSelectedNotes(selectedLids);
    }

}



void FilterEngine::filterAttributes(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isAttributeSet())
        return;

    int attribute = criteria->getAttribute()->data(0,Qt::UserRole).toInt();
    QSqlQuery sql;
    QDateTime dt;
    dt.setDate(QDate().currentDate());
    int dow = QDate().currentDate().dayOfWeek();
    int moy = QDate().currentDate().month();
    int dom = QDate().currentDate().day();
    dt.setTime(QTime(0,0,0,1));

    if (attribute == CREATED_SINCE_TODAY) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_YESTERDAY) {
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_THIS_WEEK) {
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_LAST_WEEK) {
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_THIS_MONTH) {
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_LAST_MONTH) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_THIS_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_SINCE_LAST_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }


    if (attribute == CREATED_BEFORE_TODAY) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_YESTERDAY) {
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_THIS_WEEK) {
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_LAST_WEEK) {
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_THIS_MONTH) {
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_LAST_MONTH) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_THIS_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == CREATED_BEFORE_LAST_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_CREATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }

    if (attribute == MODIFIED_SINCE_TODAY) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_YESTERDAY) {
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_THIS_WEEK) {
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_LAST_WEEK) {
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_THIS_MONTH) {
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_LAST_MONTH) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_THIS_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_SINCE_LAST_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)>(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }


    if (attribute == MODIFIED_BEFORE_TODAY) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_YESTERDAY) {
        dt = dt.addDays(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_THIS_WEEK) {
        dt = dt.addDays(-1*dow);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_LAST_WEEK) {
        dt = dt.addDays(-1*dow-7);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_THIS_MONTH) {
        dt = dt.addDays(-1*dom+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_LAST_MONTH) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_THIS_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }
    if (attribute == MODIFIED_BEFORE_LAST_YEAR) {
        dt = dt.addDays(-1*dom+1);
        dt = dt.addMonths(-1*moy+1);
        dt = dt.addYears(-1);
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key and datetime(data/1000)<(datetime(:data/1000)))");;
        sql.bindValue(":key", NOTE_UPDATED_DATE);
        sql.bindValue(":data", dt.toMSecsSinceEpoch());
    }

    if (attribute == CONTAINS_IMAGES) {
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like 'image/%'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
    }
    if (attribute == CONTAINS_AUDIO) {
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like 'audio/%'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
    }
    if (attribute == CONTAINS_INK) {
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data = 'application/vnd.evernote.ink'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
    }
    if (attribute == CONTAINS_ENCRYPTED_TEXT) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:encryptedkey)");
        sql.bindValue(":encryptedkey", NOTE_HAS_ENCRYPT);
    }
    if (attribute == CONTAINS_TODO_ITEMS) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where (key=:comp or key=:uncomp) and data='true')");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
        sql.bindValue(":uncomp", NOTE_HAS_TODO_UNCOMPLETED);
    }
    if (attribute == CONTAINS_FINISHED_TODO_ITEMS) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:comp and data='true')");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
    }
    if (attribute == CONTAINS_UNFINISHED_TODO_ITEMS) {
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:uncomp and data='true')");
        sql.bindValue(":uncomp", NOTE_HAS_TODO_UNCOMPLETED);
    }
    if (attribute == CONTAINS_PDF_DOCUMENT) {
        sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data ='application/pdf'))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
    }
    if (attribute == CONTAINS_ATTACHMENT) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key)");
        sql.bindValue(":key", NOTE_HAS_ATTACHMENT);
    }
    if (attribute == SOURCE_EMAIL) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'mail.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
    }
    if (attribute == SOURCE_EMAILED_TO_EVERNOTE) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'mail.smtp')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
    }
    if (attribute == SOURCE_MOBILE) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like 'mobile.%')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
    }
    if (attribute == SOURCE_WEB_PAGE) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data = 'web.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
    }
    if (attribute == SOURCE_ANOTHER_APPLICATION) {
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data != 'web.clip' and "
                    "data not like 'mobile.%' and data != 'mail.smtp' and data != 'mail.clip')");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
    }



    sql.exec();
    QLOG_DEBUG() << sql.lastError();

}



void FilterEngine::filterNotebook(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isNotebookSet())
        return;

    QString stackTest = criteria->getNotebook()->data(0,Qt::UserRole).toString();
    if (stackTest == "STACK") {
        QString stackName = criteria->getNotebook()->text(0);
        filterStack(stackName);
    } else {
        FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
        qint32 notebookLid = criteria->getNotebook()->data(0,Qt::UserRole).toInt();
        NotebookTable notebookTable;
        QString notebook;
        notebookTable.getGuid(notebook, notebookLid);
        filterIndividualNotebook(notebook);
    }
}

// If they only chose one notebook, then delete everything else
void FilterEngine::filterIndividualNotebook(QString &notebook) {
    // Filter out the records
    QSqlQuery sql;
    sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data=:notebookLid)");
    sql.bindValue(":type", NOTE_NOTEBOOK);
    sql.bindValue(":notebookLid", notebook);
    sql.exec();
}

void FilterEngine::filterStack(QString &stack) {
    NotebookTable notebookTable;
    QList<qint32> books;
    QList<qint32> stackBooks;
    notebookTable.getAll(books);
    notebookTable.getStack(stackBooks, stack);

    QSqlQuery sql;
    sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:type and data=:notebookLid)");
    sql.bindValue(":type", NOTE_NOTEBOOK);

    for (qint32 i=0; i<books.size(); i++) {
        if (!stackBooks.contains(books[i])) {
            Notebook n;
            notebookTable.get(n,books[i]);
            sql.bindValue(":notebookLid", QString::fromStdString(n.guid));
            sql.exec();
        }
    }
}


void FilterEngine::filterTags(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isTagsSet())
        return;

    QList<QTreeWidgetItem*> tags = criteria->getTags();
    QList<int> selectedTags;
    for (qint32 i=0; i<tags.size(); i++) {
        selectedTags.push_back(tags[i]->data(0,Qt::UserRole).toInt());
    }

    NoteTable noteTable;
    TagTable tagTable;
    QList<qint32> goodNotes;
    for (qint32 i=0; i<selectedTags.size(); i++) {
        QList<qint32> notes;
        QString tagGuid;
        tagTable.getGuid(tagGuid, selectedTags[i]);
        noteTable.getNotesWithTag(notes, tagGuid);
        for (qint32 j=0; j<notes.size(); j++) {
            if (!goodNotes.contains(notes[j]))
                goodNotes.append(notes[j]);
        }
    }

    QSqlQuery sql;
    sql.exec("create temporary table if not exists goodLids (lid integer)");
    sql.exec("delete from goodLids");
    sql.prepare("insert into goodLids (lid) values (:note)");
    for (qint32 i=0; i<goodNotes.size(); i++) {
        sql.bindValue(":note", goodNotes[i]);
        sql.exec();
    }
    sql.exec("delete from filter where lid not in (select lid from goodLids)" );
}


// If they only chose one notebook, then delete everything else
void FilterEngine::filterTrash(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isDeletedOnlySet()
            || (criteria->isDeletedOnlySet() && !criteria->getDeletedOnly()))
    {
        QSqlQuery sql;
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data='true')");
        sql.bindValue(":type", NOTE_ACTIVE);
        sql.exec();
        return;
    }
    if (!criteria->getDeletedOnly())
        return;

    // Filter out the records
    QSqlQuery sql;
    sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data='false')");
    sql.bindValue(":type", NOTE_ACTIVE);
    sql.exec();
}



// Filter the data based upon what the user enters in the search string
void FilterEngine::filterSearchString(FilterCriteria *criteria) {
    if (!criteria->isSet() || !criteria->isSearchStringSet() ||
            criteria->getSearchString().trimmed() == "")
        return;

    anyFlagSet = false;
    if (criteria->getSearchString().trimmed().startsWith(":any", Qt::CaseInsensitive))
        anyFlagSet = true;

    // Tokenize out the words
    QStringList list;
    splitSearchTerms(list, criteria->getSearchString());
    if (!anyFlagSet) {
        filterSearchStringAll(list);
    }


}


// Split the search term into specific tokens.
void FilterEngine::splitSearchTerms(QStringList &words, QString search) {
    words.empty();


    // First go through the string and put null characters between
    // the search terms.  This helps parse out the terms later, since
    // some may be in quotes
    qint32 len = search.length();
    char nextChar = ' ';
    bool quote = false;
    for (qint32 i=0; i<len; i++) {
        if (search[i]==nextChar && !quote) {
            search[i] = '\0';
            nextChar = ' ';
        } else {
            if (search[i] =='\"') {
                if (!quote) {
                    quote=true;
                } else {
                    quote=false;
                }
            }
        }
        if (((i+2)<len) && search[i] == '\\') {
            i=i+2;
        }
    }

    // Now that we have null characters between them, we parse
    // out based upon them rather than spaces.
    qint32 pos = 0;
    for (qint32 i=0; i<search.length() && search.length() > 0; i++) {
        if (search[i] == '\0') {
           search = search.remove(0,1);
                i=-1;
        } else {
            pos = search.indexOf(QChar('\0'));
            if (pos > 0) {
                words.append(search.left(pos).toLower());
                search.remove(0,pos);
                i=-1;
            } else {
                words.append(search.toLower());
                search = "";
            }
        }
    }

    // Now that we have everything separated, we can remove the unneeded " marks
    for (qint32 i=0; i<words.length(); i++) {
        words[i].remove("\"");
    }
}


// Filter based upon the words the user specified (as opposed to the notebook, tags ...)
// this is for the "all" filter (the default), not the "any:"
void FilterEngine::filterSearchStringAll(QStringList list) {
    // Filter out the records
    QSqlQuery sql, sqlnegative;
    sql.prepare("Delete from filter where lid not in (select lid from SearchIndex where content match :word)");
    sqlnegative.prepare("Delete from filter where lid in (select lid from SearchIndex where content match :word)");
    for (qint32 i=0; i<list.size(); i++) {
        QString string = list[i];
        string.remove(QChar('"'));

        bool filterFound = false;
        // If we have a notebook search request
        if (string.startsWith("notebook:", Qt::CaseInsensitive) ||
                string.startsWith("-notebook:", Qt::CaseInsensitive)) {
            filterSearchStringNotebookAll(string);
            filterFound = true;
        }
        if (string.startsWith("todo:", Qt::CaseInsensitive) ||
                string.startsWith("-todo:", Qt::CaseInsensitive)) {
            filterSearchStringTodoAll(string);
            filterFound = true;
        }
        if (string.startsWith("tag:", Qt::CaseInsensitive) ||
                string.startsWith("-tag:", Qt::CaseInsensitive)) {
            filterSearchStringTagAll(string);
            filterFound = true;
        }
        if (string.startsWith("intitle:", Qt::CaseInsensitive) ||
                string.startsWith("-intitle:", Qt::CaseInsensitive)) {
            filterSearchStringIntitleAll(string);
            filterFound = true;
        }
        if (string.startsWith("mime:", Qt::CaseInsensitive) ||
                string.startsWith("-mime:", Qt::CaseInsensitive)) {
            filterSearchStringMimeAll(string);
            filterFound = true;
        }
        if (!filterFound) {
            if (string.startsWith("-")) {
                string = string.remove(0,1);
                sqlnegative.bindValue(":word", string.trimmed()+"*");
                sqlnegative.exec();
            } else {
                sql.bindValue(":word", string.trimmed()+"*");
                sql.exec();
            }
        }
    }
}





// filter based upon the title string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringIntitleAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,8);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            string = QString("%") +string +QString("%");
        tagSql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :title)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":data", string);

        tagSql.exec();
    } else {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            string = QString("%") +string +QString("%");
        tagSql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :title)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":data", string);

        tagSql.exec();
    }
}




// filter based upon the mime type the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringMimeAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        else
            sql.prepare("Delete from filter where lid not in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        QLOG_DEBUG() << sql.lastError();
    } else {
        string.remove(0,6);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data))");
        else
            sql.prepare("Delete from filter where lid in (select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data))");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
    }
}


// filter based upon the tag string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringTagAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,4);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        if (string.indexOf("*") < 0)
            tagSql.prepare("Delete from filter where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey))");
        else {
            tagSql.prepare("Delete from filter where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey))");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG);

        tagSql.exec();
    } else {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        if (string.indexOf("*") < 0)
            tagSql.prepare("Delete from filter where lid in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey))");
        else {
            tagSql.prepare("Delete from filter where lid in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey))");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG);
        tagSql.exec();
    }
}



// filter based upon the notebook string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringNotebookAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery notebookSql;
        if (string.indexOf("*") < 0)
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook = :notebook)");
        else {
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook like :notebook)");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":type", NOTE_NOTEBOOK);
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery notebookSql;
        if (string.indexOf("*") < 0)
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook <> :notebook)");
        else {
            notebookSql.prepare("Delete from filter where lid not in (select lid from NoteTable where notebook not like :notebook)");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":type", NOTE_NOTEBOOK);
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
    }
}


// filter based upon the notebook string the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringTodoAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1 or key=:key2)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
    } else {
        string.remove(0,6);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.startsWith("*")) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1 or key=:key2)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
            sql.bindValue(":key2", NOTE_HAS_TODO_UNCOMPLETED);
        }
        if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
    }
}

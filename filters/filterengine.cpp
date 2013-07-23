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
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where (key=:comp or key=:uncomp) and data='true')");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
        sql.bindValue(":uncomp", NOTE_HAS_TODO_UNCOMPLETED);
        break;
    case CONTAINS_FINISHED_TODO_ITEMS:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:comp and data='true')");
        sql.bindValue(":comp", NOTE_HAS_TODO_COMPLETED);
        break;
    case CONTAINS_UNFINISHED_TODO_ITEMS:
        sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:uncomp and data='true')");
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
    NotebookTable notebookTable;
    qint32 notebookLid = notebookTable.getLid(notebook);
    // Filter out the records
    QSqlQuery sql;
    sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:type and data=:notebookLid)");
    sql.bindValue(":type", NOTE_NOTEBOOK_LID);
    sql.bindValue(":notebookLid", notebookLid);
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
    sql.bindValue(":type", NOTE_NOTEBOOK_LID);

    for (qint32 i=0; i<books.size(); i++) {
        if (!stackBooks.contains(books[i])) {
            sql.bindValue(":notebookLid", books[i]);
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


// If they only chose one tag, then delete everything else
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
    if (criteria->getSearchString().trimmed().startsWith("any:", Qt::CaseInsensitive))
        anyFlagSet = true;

    // Tokenize out the words
    QStringList list;
    splitSearchTerms(list, criteria->getSearchString());
    if (!anyFlagSet)
        filterSearchStringAll(list);
    else
        filterSearchStringAny(list);


}


// Split the search term into specific tokens.
void FilterEngine::splitSearchTerms(QStringList &words, QString search) {
    words.clear();


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
    sql.prepare("Delete from filter where lid not in (select lid from SearchIndex where weight>=:weight and content match :word)");
    sqlnegative.prepare("Delete from filter where lid in (select lid from SearchIndex where weight>=:weight and content match :word)");
    sql.bindValue(":weight", global.getMinimumRecognitionWeight());
    for (qint32 i=0; i<list.size(); i++) {
        QString string = list[i];
        string.remove(QChar('"'));

        // If we have a notebook search request
        if (string.startsWith("notebook:", Qt::CaseInsensitive) ||
                string.startsWith("-notebook:", Qt::CaseInsensitive)) {
            filterSearchStringNotebookAll(string);
        }
        else if (string.startsWith("todo:", Qt::CaseInsensitive) ||
                string.startsWith("-todo:", Qt::CaseInsensitive)) {
            filterSearchStringTodoAll(string);
        }
        else if (string.startsWith("tag:", Qt::CaseInsensitive) ||
                string.startsWith("-tag:", Qt::CaseInsensitive)) {
            filterSearchStringTagAll(string);
        }
        else if (string.startsWith("intitle:", Qt::CaseInsensitive) ||
                string.startsWith("-intitle:", Qt::CaseInsensitive)) {
            filterSearchStringIntitleAll(string);
        }
        else if (string.startsWith("resource:", Qt::CaseInsensitive) ||
                string.startsWith("-resource:", Qt::CaseInsensitive)) {
            filterSearchStringResourceAll(string);
        }
        else if (string.startsWith("longitude:", Qt::CaseInsensitive) ||
                string.startsWith("-longitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_LONGITUDE);
        }
        else if (string.startsWith("latitude:", Qt::CaseInsensitive) ||
                string.startsWith("-latitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_LATITUDE);
        }
        else if (string.startsWith("altitude:", Qt::CaseInsensitive) ||
                string.startsWith("-altitude:", Qt::CaseInsensitive)) {
            filterSearchStringCoordinatesAll(string, NOTE_ATTRIBUTE_ALTITUDE);
        }
        else if (string.startsWith("author:", Qt::CaseInsensitive) ||
                string.startsWith("-author:", Qt::CaseInsensitive)) {
            filterSearchStringAuthorAll(string);
        }
        else if (string.startsWith("source:", Qt::CaseInsensitive) ||
                string.startsWith("-source:", Qt::CaseInsensitive)) {
            filterSearchStringSourceAll(string);
        }
        else if (string.startsWith("sourceapplication:", Qt::CaseInsensitive) ||
                string.startsWith("-sourceapplication:", Qt::CaseInsensitive)) {
            filterSearchStringSourceApplicationAll(string);
        }
        else if (string.startsWith("contentclass:", Qt::CaseInsensitive) ||
                string.startsWith("-contentclass:", Qt::CaseInsensitive)) {
            filterSearchStringContentClassAll(string);
        }
        else if (string.startsWith("recotype:", Qt::CaseInsensitive) ||
                string.startsWith("-recotype:", Qt::CaseInsensitive)) {
            filterSearchStringResourceRecognitionTypeAll(string);
        }
        else if (string.startsWith("placename:", Qt::CaseInsensitive) ||
                string.startsWith("-placename:", Qt::CaseInsensitive)) {
            filterSearchStringContentClassAll(string);
        }
        else if (string.startsWith("created:", Qt::CaseInsensitive) ||
                string.startsWith("-created:", Qt::CaseInsensitive) ||
                string.startsWith("updated:", Qt::CaseInsensitive) ||
                string.startsWith("-updated:", Qt::CaseInsensitive) ||
                string.startsWith("subjectdate:", Qt::CaseInsensitive) ||
                string.startsWith("-subjectdate:", Qt::CaseInsensitive)) {
            filterSearchStringDateAll(string);
        }
        else { // Filter not found
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




// filter based upon the note coordinates the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringCoordinatesAll(QString string, int key) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "0";
        // Filter out the records
        QSqlQuery sql;
        sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data >= :data)");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    } else {
        if (string == "")
            string = "0";
        // Filter out the records
        QSqlQuery sql;
        sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data <= :data)");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}


// filter based upon the note author the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringAuthorAll(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




// filter based upon the note source the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringSourceAll(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




// filter based upon the note content class the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringContentClassAll(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}


// filter based upon the note content class the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringPlaceNameAll(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




// filter based upon the note source application the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringSourceApplicationAll(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("Delete from filter where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}





// filter based upon the mime type the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringResourceAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,9);
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
    } else {
        string.remove(0,10);
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



// filter based upon the mime type the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringResourceRecognitionTypeAll(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,9);
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
        sql.bindValue(":mimekey", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        string.remove(0,10);
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
        sql.bindValue(":mimekey", RESOURCE_RECO_TYPE);
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
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);

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
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);
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
//        notebookSql.bindValue(":type", NOTE_NOTEBOOK_LID);
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
        //notebookSql.bindValue(":type", NOTE_NOTEBOOK);
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
        else if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid not in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        else if (string.startsWith("false", Qt::CaseInsensitive)) {
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
        else if (string.startsWith("true", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_COMPLETED);
        }
        else if (string.startsWith("false", Qt::CaseInsensitive)) {
            sql.prepare("Delete from filter where lid in (select lid from DataStore where key=:key1)");
            sql.bindValue(":key1", NOTE_HAS_TODO_UNCOMPLETED);
        }
        sql.exec();
    }
}





void FilterEngine::filterSearchStringDateAll(QString string) {
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    QSqlQuery sql;
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

}





QDateTime FilterEngine::calculateDateTime(QString string) {
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
    }
    if (string.startsWith("day")) {
        value = tam;
        string = string.mid(3);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addDays(offset);
        return value;
    }
    else if (string.startsWith("month")) {
        value = tam;
        value = value.addDays(-1*dom+1);
        string = string.mid(5);
        offset = 0;
        if (string != "")
            offset = string.toInt();
        value = value.addMonths(offset);
        QLOG_DEBUG() << value.toString();
        return value;
    }
    else if (string.startsWith("year")) {
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
    }
    else if (string.startsWith("week")) {
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
    // Filter out the records
    QSqlQuery sql, sqlnegative;
    sql.exec("create temporary table if not exists anylidsfilter (lid int);");
    sql.exec("delete from anylidsfilter");

    sql.prepare("insert into anylidsfilter (lid) select lid from SearchIndex where weight>=:weight and content match :word");
    sqlnegative.prepare("insert into anylidsfilter (lid) select lid from SearchIndex where lid not in (select lid from searchindex where weight>=:weight and content match :word)");
    sql.bindValue(":weight", global.getMinimumRecognitionWeight());
    sqlnegative.bindValue(":weight", global.getMinimumRecognitionWeight());

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
            } else {
                sql.bindValue(":word", string.trimmed()+"*");
                sql.exec();
                QLOG_DEBUG() << sql.lastError();
            }
        }
    }

    sql.exec("delete from filter where lid not in (select lid from anylidsfilter)");
}



// filter based upon the notebook string the user specified.  This is for the "any"
// filter and not the default
void FilterEngine::filterSearchStringNotebookAny(QString string) {

    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery notebookSql;
        if (string.indexOf("*") < 0)
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook=:notebook");
        else {
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook like :notebook");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery notebookSql;
        if (string.indexOf("*") < 0)
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook <> :notebook");
        else {
            notebookSql.prepare("insert into anylidsfilter (lid) select lid from NoteTable where notebook not like :notebook");
            string.replace("*", "%");
        }
        notebookSql.bindValue(":notebook", string);
        notebookSql.exec();
    }
}




// filter based upon the notebook string the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringTodoAny(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
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
    } else {
        string.remove(0,6);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
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
    }
}




// filter based upon the tag string the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringTagAny(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,4);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        if (string.indexOf("*") < 0)
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey)");
        else {
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey)");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);

        tagSql.exec();
    } else {
        string.remove(0,5);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        if (string.indexOf("*") < 0)
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data=:tagname and key=:tagnamekey))");
        else {
            tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:notetagkey and data in (select lid from DataStore where data like :tagname and key=:tagnamekey))");
            string = string.replace("*", "%");
        }
        tagSql.bindValue(":tagname", string);
        tagSql.bindValue(":tagnamekey", TAG_NAME);
        tagSql.bindValue(":notetagkey", NOTE_TAG_LID);
        tagSql.exec();
    }
}



// filter based upon the title string the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringIntitleAny(QString string) {
    if (!string.startsWith("-")) {
        int pos = string.indexOf(":")+1;
        string = string.mid(pos);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            string = QString("%") +string +QString("%");
        tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :title");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":data", string);

        tagSql.exec();
    } else {
        int pos = string.indexOf(":")+1;
        string = string.mid(pos);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery tagSql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            string = QString("%") +string +QString("%");
        tagSql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :title)");
        tagSql.bindValue(":key", NOTE_TITLE);
        tagSql.bindValue(":data", string);

        tagSql.exec();
    }
}




// filter based upon the mime type the user specified.  This is for the "any:"
// filter and not the default
void FilterEngine::filterSearchStringResourceAny(QString string) {
    if (!string.startsWith("-")) {
        string.remove(0,9);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            sql.prepare("insert into anylidsfilter (lid) select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data=:data)");
        else
            sql.prepare("insert into anylidsfilter (lid) select data from datastore where key=:notelidkey and lid in (select lid from DataStore where key=:mimekey and data like :data)");
        sql.bindValue(":notelidkey", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        QLOG_DEBUG() << sql.lastError();
    } else {
        string.remove(0,10);
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        string = string.replace("*", "%");
        if (string.indexOf("%") < 0)
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select data from datastore where key=:notelid and lid in (select lid from DataStore where data=:data and key = :mimekey))");
        else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select data from datastore where key=:notelid and lid not in (select lid from DataStore where data=:data and key like :mimekey))");
        sql.bindValue(":notelid", RESOURCE_NOTE_LID);
        sql.bindValue(":mimekey", RESOURCE_MIME);
        sql.bindValue(":data", string);
        sql.exec();
        QLOG_DEBUG() << sql.lastError();
    }
}




// filter based upon the note coordinates the user specified.  This is for the "all"
// filter and not the "any".
void FilterEngine::filterSearchStringCoordinatesAny(QString string, int key) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "0";
        // Filter out the records
        QSqlQuery sql;
        sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data >= :data");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    } else {
        if (string == "")
            string = "0";
        // Filter out the records
        QSqlQuery sql;
        sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data <= :data");
        sql.bindValue(":key", key);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




// filter based upon the note author the user specified.  This is for the "any"
// filter and not the default
void FilterEngine::filterSearchStringAuthorAny(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data = :data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data = :data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




void FilterEngine::filterSearchStringDateAny(QString string) {
    int separator = string.indexOf(":")+1;
    QString tempString = string.mid(separator);
    QDateTime dt = calculateDateTime(tempString);
    QSqlQuery sql;
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

}






// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringSourceAny(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
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
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}




// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringSourceApplicationAny(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}





// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringContentClassAny(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}



// filter based upon the note source the user specified.  This is for the "any"
// filter and not the default.
void FilterEngine::filterSearchStringResourceRecognitionTypeAny(QString string) {
    bool negative = false;
    if (string.startsWith("-"))
        negative = true;
    int separator = string.indexOf(":")+1;
    string = string.mid(separator);
    if (negative) {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data like :data");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where key=:key and data=:data");
        sql.bindValue(":key", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string);
        sql.exec();
    } else {
        if (string == "")
            string = "*";
        // Filter out the records
        QSqlQuery sql;
        if (string.indexOf("*")>=0) {
            string = string.replace("*", "%");
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data like :data)");
        } else
            sql.prepare("insert into anylidsfilter (lid) select lid from datastore where lid not in (select lid from datastore where key=:key and data=:data)");
        sql.bindValue(":key", RESOURCE_RECO_TYPE);
        sql.bindValue(":data", string.toDouble());
        sql.exec();
    }
}

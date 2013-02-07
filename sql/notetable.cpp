#include "notetable.h"
#include "resourcetable.h"
#include "configstore.h"
#include "notebooktable.h"
#include "tagtable.h"
#include "global.h"

#include <QSqlTableModel>
#include "html/tagscanner.h"


// Default constructor
NoteTable::NoteTable()
{
}



// Given a note's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NoteTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE() << "Entering NoteTable::updateNoteGuid()";

    QSqlQuery query;
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();

    QLOG_TRACE() << "Leaving NoteTable::updateNoteGuid()";
}




// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(Note &note) {
    sync(0, note);
}



// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(qint32 lid, Note &note) {
   // QLOG_TRACE() << "Entering NoteTable::sync()";

    if (lid > 0) {
        QSqlQuery query;

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();

        ResourceTable resTable;
        for (unsigned int i=0;i<note.resources.size(); i++) {
            qint32 resLid = resTable.getLid(note.resources[i].noteGuid, note.resources[i].guid);
            query.bindValue(":lid", resLid);
            query.exec();
        }
    } else {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }

    add(lid, note, false);

    //QLOG_TRACE() << "Leaving NoteTable::sync()";
}




// Given a note's GUID, we return the LID
qint32 NoteTable::getLid(QString guid) {

    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", guid);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Given a note's lid, return the guid
QString NoteTable::getGuid(qint32 lid) {

    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        return query.value(0).toString();
    else
        return "";
}


// Given a note's GUID, we return the LID
qint32 NoteTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new note to the database
qint32 NoteTable::add(qint32 l, Note &t, bool isDirty) {
    QLOG_DEBUG() << "Adding note: " << QString::fromStdString(t.title);
    ResourceTable resTable;
    ConfigStore cs;
    QSqlQuery query;
    qint32 position;
    TagScanner scanner;
    qint32 lid = l;
    qint32 notebookLid;

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    if (lid <= 0)
        lid = cs.incrementLidCounter();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", QString::fromStdString(t.guid));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", true);
    query.exec();

    if (t.__isset.title) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_TITLE);
        query.bindValue(":data", QString::fromStdString(t.title.c_str()));
        query.exec();
    }

    if (t.__isset.content) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT);
        QByteArray b;
        b.append(QString::fromStdString(t.content).toAscii());
        query.bindValue(":data", b);
        query.exec();
    }

    if (t.__isset.contentHash) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_HASH);
        query.bindValue(":data", QString::fromStdString(t.contentHash));
        query.exec();
    }

    if (t.__isset.contentLength) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_LENGTH);
        query.bindValue(":data", t.contentLength);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", t.updateSequenceNum);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (t.__isset.created) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CREATED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.created));
        query.exec();
    }

    if (t.__isset.updated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.updated));
        query.exec();
    }

    if (t.__isset.deleted) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_DELETED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.deleted));
        query.exec();
    }

    if (t.__isset.active) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ACTIVE);
        query.bindValue(":data", QVariant::fromValue(t.active));
        query.exec();
    }

    if (t.__isset.notebookGuid) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_NOTEBOOK);
        NotebookTable notebookTable;
        notebookLid = notebookTable.getLid(QString::fromStdString(t.notebookGuid));

        // If not found, we insert one to avoid problems.  We'll probably get the real data later
        if (notebookLid == 0) {
            notebookLid = cs.incrementLidCounter();
            Notebook notebook;
            notebook.guid = t.notebookGuid;
            notebook.name = "";
            notebook.__isset.guid = true;
            notebook.__isset.name = true;
            notebookTable.add(notebookLid, notebook, false, false);
        }
        query.bindValue(":data", QString::fromStdString(t.notebookGuid));
        query.exec();
    }

    for (unsigned int i=0; t.__isset.tagGuids && i<t.tagGuids.size(); i++) {
        TagTable tagTable;
        qint32 tagLid = tagTable.getLid(t.tagGuids.at(i));
        if (tagLid == 0) {
            // create a dummy tag to avoid later problems
            Tag newTag;
            newTag.guid = t.tagGuids.at(i);
            newTag.name = "";
            newTag.__isset.guid = true;
            newTag.__isset.name = true;
            tagLid = cs.incrementLidCounter();
            tagTable.add(tagLid, newTag, false);
        }

        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_TAG);
        query.bindValue(":data", tagLid);
        query.exec();
    }

    for (unsigned int i=0; t.__isset.resources && i<t.resources.size(); i++) {
        qint32 resLid;
        Resource *r;
        r = &t.resources[i];
        resLid = resTable.getLid(t.guid,t.resources[i].guid);
        if (resLid == 0)
            resLid = cs.incrementLidCounter();
        resTable.add(resLid, t.resources[i], isDirty);

        if (r->__isset.mime) {
            QString mime = QString::fromStdString(r->mime);
            if (!mime.startsWith("image/") && mime != "vnd.evernote.ink") {
                query.bindValue(":lid", lid);
                query.bindValue(":key", NOTE_HAS_ATTACHMENT);
                query.bindValue(":data", true);
                query.exec();
            }
        }
    }

    if (t.__isset.attributes) {
        if (t.attributes.__isset.subjectDate) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SUBJECT_DATE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.subjectDate));
            query.exec();
        }
        if (t.attributes.__isset.latitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LATITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.latitude));
            query.exec();
        }
        if (t.attributes.__isset.latitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.longitude));
            query.exec();
        }
        if (t.attributes.__isset.altitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_ALTITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.altitude));
            query.exec();
        }
        if (t.attributes.__isset.author) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
            query.bindValue(":data", QString::fromStdString(t.attributes.author));
            query.exec();
        }
        if (t.attributes.__isset.source) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
            query.bindValue(":data", QString::fromStdString(t.attributes.source));
            query.exec();
        }
        if (t.attributes.__isset.sourceURL) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_URL);
            query.bindValue(":data", QString::fromStdString(t.attributes.sourceURL));
            query.exec();
        }
        if (t.attributes.__isset.sourceApplication) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
            query.bindValue(":data", QString::fromStdString(t.attributes.sourceApplication));
            query.exec();
        }
        if (t.attributes.__isset.shareDate) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SHARE_DATE);
            query.bindValue(":data",QVariant::fromValue(t.attributes.shareDate));
            query.exec();
        }
        if (t.attributes.__isset.placeName) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
            query.bindValue(":data", QString::fromStdString(t.attributes.placeName));
            query.exec();
        }
        if (t.attributes.__isset.contentClass) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
            query.bindValue(":data", QString::fromStdString(t.attributes.contentClass));
            query.exec();
        }
    }

    // No determine some attributes of the note based upon the content
    QString content;
    if (t.__isset.content)
        content = QString::fromStdString(t.content);
    else
        content = "";

    position = content.indexOf("<en-crypt");
    if (position > 0) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_HAS_ENCRYPT);
        query.bindValue(":data", true);
        query.exec();
    }
    position = content.indexOf("<en-todo");
    if (position > 0) {
        position = content.indexOf("<en-todo checked=\"true\"");
        if (position > 0) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_HAS_TODO_COMPLETED);
            query.bindValue(":data", true);
            query.exec();
        }
        position = content.indexOf("en-todo checked=\"false\"");
        if (position > 0) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_HAS_TODO_UNCOMPLETED);
            query.bindValue(":data", true);
            query.exec();
        }
    }

    updateNoteList(lid, t, isDirty);
    return lid;
}


bool NoteTable::updateNoteList(qint32 lid, Note &t, bool isDirty) {

    NotebookTable notebookTable;
    qint32 notebookLid = notebookTable.getLid(t.notebookGuid);
    Notebook notebook;
    notebookTable.get(notebook, notebookLid);
    // Now let's update the user table
    QSqlQuery query;

    query.prepare("Delete from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare(QString("Insert into NoteTable (lid, title, author, ") +
                  QString("dateCreated, dateUpdated, dateSubject, dateDeleted, source, sourceUrl, sourceApplication, ") +
                  QString("latitude, longitude, altitude, hasEncryption, hasTodo, isDirty, size, notebook, notebookLid, tags) ") +
                  QString("Values (:lid, :title, :author, ") +
                  QString(":dateCreated, :dateUpdated, :dateSubject, :dateDeleted, :source, :sourceUrl, :sourceApplication, ") +
                  QString(":latitude, :longitude, :altitude, :hasEncryption, :hasTodo, :isDirty, :size, :notebook, :notebookLid, :tags) ")) ;

    query.bindValue(":lid", lid);

    if (t.__isset.title)
        query.bindValue(":title", QString::fromStdString(t.title));
    else
        query.bindValue(":title", "");
    if (t.__isset.attributes && t.attributes.__isset.author)
        query.bindValue(":author", QString::fromStdString(t.attributes.author));
    else
        query.bindValue(":author", "");
    if (t.__isset.created)
        query.bindValue(":dateCreated", QVariant::fromValue(t.created));
    else
        query.bindValue(":dateCreated", 0);
    if (t.__isset.updated)
        query.bindValue(":dateUpdated", QVariant::fromValue(t.updated));
    else
        query.bindValue(":dateUpdated", 0);
    if (t.__isset.attributes && t.attributes.__isset.subjectDate)
        query.bindValue(":dateSubject", QVariant::fromValue(t.attributes.subjectDate));
    else
        query.bindValue(":dateSubject", 0);
    if (t.__isset.deleted)
        query.bindValue(":dateDeleted", QVariant::fromValue(t.deleted));
    else
        query.bindValue(":dateDeleted", 0);
    if (t.__isset.attributes && t.attributes.__isset.source)
        query.bindValue(":source", QString::fromStdString(t.attributes.source));
    else
        query.bindValue(":source", "");
    if (t.__isset.attributes && t.attributes.__isset.sourceURL)
        query.bindValue(":sourceUrl", QString::fromStdString(t.attributes.sourceURL));
    else
        query.bindValue(":sourceUrl", "");
    if (t.__isset.attributes && t.attributes.__isset.sourceApplication)
        query.bindValue(":sourceApplication", QString::fromStdString(t.attributes.sourceApplication));
    else
        query.bindValue(":sourceApplication", "");
    if (t.__isset.attributes && t.attributes.__isset.latitude)
        query.bindValue(":latitude", QVariant::fromValue(t.attributes.latitude));
    else
        query.bindValue(":latitude", 0);
    if (t.__isset.attributes && t.attributes.__isset.longitude)
        query.bindValue(":longitude", QVariant::fromValue(t.attributes.longitude));
    else
        query.bindValue(":longitude", 0);
    if (t.__isset.attributes && t.attributes.__isset.altitude)
        query.bindValue(":altitude", QVariant::fromValue(t.attributes.altitude));
    else
        query.bindValue(":altitude", 0);


    bool hasEncryption;
    if (t.content.find("<en-crypt") != string::npos)
        hasEncryption = true;
    else
        hasEncryption = false;
    query.bindValue(":hasEncryption", hasEncryption);
    bool hasTodo;
    if (t.content.find("<en-todo") != string::npos)
        hasTodo = true;
    else
        hasTodo = false;
    query.bindValue(":hasTodo", hasTodo);
    query.bindValue(":isDirty", !isDirty);
    qlonglong size = t.content.length();
    for (unsigned int i=0; i<t.resources.size(); i++) {
        size+=t.resources[i].data.size;
    }
    query.bindValue(":size", size);

    query.bindValue(":notebook", QString::fromStdString(notebook.name));
    query.bindValue(":notebookLid", notebookLid);

    QString tagNames;
    QStringList sortedNames;
    for (unsigned int i=0; i<t.tagNames.size(); i++) {
        sortedNames.append(QString::fromStdString(t.tagNames.at(i)).toLower());
    }
    sortedNames.sort();

    TagTable tagTable;
    for (int i=0; i<sortedNames.size(); i++) {
        if (i>0)
            tagNames = tagNames+", ";
        Tag currentTag;
        qint32 tagLid = tagTable.findByName(sortedNames[i]);
        tagTable.get(currentTag, tagLid);
        tagNames = tagNames + QString::fromStdString(currentTag.name);
    }

    query.bindValue(":tags", tagNames);

    if (!query.exec()) {
        QLOG_ERROR() << "Error inserting into NoteTable: " << query.lastError();
        return false;
    }
    return true;
}




bool NoteTable::updateNotebookName(qint32 lid, QString name) {
    QSqlQuery query;
    query.prepare("Update NoteTable set notebook=:name where notebooklid=:lid");
    query.bindValue(":name", name);
    query.bindValue(":lid", lid);
    return query.exec();
}


// Return a note structure given the LID
bool NoteTable::get(Note &note, qint32 lid,bool loadResources, bool loadResourceRecognition) {

    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);

    query.exec();
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (NOTE_GUID):
            note.guid = query.value(1).toString().toStdString();
            note.__isset.guid = true;
            break;
        case (NOTE_UPDATE_SEQUENCE_NUMBER):
            note.updateSequenceNum = query.value(1).toInt();
            note.__isset.updateSequenceNum = true;
            break;
        case (NOTE_ACTIVE):
            note.active = query.value(1).toBool();
            note.__isset.active = true;
            break;
        case (NOTE_DELETED_DATE):
            note.active = query.value(1).toLongLong();
            note.__isset.deleted = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE_URL):
            note.attributes.sourceURL = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.sourceURL = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE_APPLICATION):
            note.attributes.sourceApplication = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.sourceApplication = true;
            break;
        case (NOTE_CONTENT_LENGTH):
            note.contentLength = query.value(1).toLongLong();
            note.__isset.contentLength = true;
            break;
        case (NOTE_ATTRIBUTE_LONGITUDE):
            note.attributes.longitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.longitude = true;
            break;
        case (NOTE_TITLE):
            note.title = query.value(1).toString().toStdString();
            note.__isset.title = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE):
            note.attributes.source = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.source = true;
            break;
        case (NOTE_ATTRIBUTE_ALTITUDE):
            note.attributes.altitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.altitude = true;
            break;
        case (NOTE_NOTEBOOK):
            note.notebookGuid = query.value(1).toString().toStdString();
            note.__isset.notebookGuid = true;
            break;
        case (NOTE_UPDATED_DATE):
            note.updated = query.value(1).toLongLong();
            note.__isset.updated = true;
            break;
        case (NOTE_CREATED_DATE):
            note.created = query.value(1).toLongLong();
            note.__isset.created = true;
            break;
        case (NOTE_ATTRIBUTE_SUBJECT_DATE):
            note.attributes.subjectDate = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.subjectDate = true;
            break;
        case (NOTE_ATTRIBUTE_LATITUDE):
            note.attributes.latitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.latitude = true;
            break;
        case (NOTE_CONTENT):
            note.content = query.value(1).toByteArray().data();
            note.__isset.content = true;
            break;
        case (NOTE_CONTENT_HASH):
            note.contentHash = query.value(1).toString().toStdString();
            note.__isset.contentHash = true;
            break;
        case (NOTE_ATTRIBUTE_AUTHOR):
            note.attributes.author = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.author = true;
            break;
        case (NOTE_ISDIRTY):
            break;
        case (NOTE_ATTRIBUTE_SHARE_DATE) :
            note.attributes.shareDate = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.shareDate = true;
            break;
        case (NOTE_ATTRIBUTE_PLACE_NAME) :
            note.attributes.placeName = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.placeName = true;
            break;
        case (NOTE_ATTRIBUTE_CONTENT_CLASS) :
            note.attributes.contentClass = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.contentClass = true;
            break;
        case (NOTE_TAG) :
            TagTable tagTable;
            qint32 tagLid = query.value(1).toInt();
            Tag tag;
            tagTable.get(tag, tagLid);
            note.__isset.tagGuids = true;
            note.__isset.tagNames = true;
            note.tagGuids.push_back(tag.guid);
            note.tagNames.push_back(tag.name);
            break;
        }
    }

    ResourceTable resTable;
    QList<qint32> resList;
    if (resTable.getResourceList(resList, lid)) {
        for (int i=0; i<resList.size(); i++) {
            Resource resource;
            if (loadResources) {
                resTable.get(resource, resList[i]);
            } else {
                QString resGuid = resTable.getGuid(resList[i]);
                resource.guid = resGuid.toStdString();
                resource.__isset.guid = true;
            }
            note.__isset.resources = true;
            note.resources.push_back(resource);
        }
    }

    /*
    TagScanner test;
    test.setData(QString::fromStdString(note.content));
    QList<TagScannerRecord> retval;
    int k = test.findAll(retval, QString("en-note"));
    */

    if (note.__isset.guid)
        return true;
    else
        return false;
}



// Return a note given the GUID
bool NoteTable::get(Note& note, QString guid,bool loadResources, bool loadResourceRecognition) {
    qint32 lid = getLid(guid);
    return get(note, lid, loadResources, loadResourceRecognition);
}



// Return a note given the GUID as a std::string
bool NoteTable::get(Note &note, string guid, bool loadResources, bool loadResourceRecognition) {
    qint32 lid = getLid(guid);
    return get(note, lid, loadResources, loadResourceRecognition);
}



// Return if a note is dirty given its lid
bool NoteTable::isDirty(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Determine if a note is dirty given a guid
bool NoteTable::isDirty(QString guid) {
    qint32 lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a note is dirty given a guid
bool NoteTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}


// Does this note exist?
bool NoteTable::exists(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


// Determine if a note is dirty given a guid
bool NoteTable::exists(QString guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Determine if a note is dirty given a guid
bool NoteTable::exists(string guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}



// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, qint32 tagLid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:tagLid");
    query.bindValue(":key", NOTE_TAG);
    query.bindValue(":tagLid", tagLid);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }
    return values.size();
}

// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, QString data) {
    TagTable tagTable;
    qint32 tagLid = tagTable.getLid(data);
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:tagLid");
    query.bindValue(":key", NOTE_TAG);
    query.bindValue(":tagLid", tagLid);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }
    return values.size();
}


qint32 NoteTable::findNotesByTag(QList<qint32> &values, string data) {
    return findNotesByTag(values, QString::fromStdString(data));
}


// Update the user note list with the proper tag names
void NoteTable::updateNoteListTags(qint32 noteLid, QString tags) {
    QSqlQuery query;
    query.prepare("Update NoteTable set tags=:tags where lid=:lid");
    query.bindValue(":lid", noteLid);
    query.bindValue(":tags", tags);
    query.exec();
}



// Update the user's notebook name list
void NoteTable::updateNoteListNotebooks(QString guid, QString name) {
    QSqlQuery sql, sql2;
    sql.prepare("select lid from DataStore where data=:guid and key=:key");
    sql.bindValue(":data", guid);
    sql.bindValue(":key", NOTE_NOTEBOOK);
    sql.exec();
    while (sql.next()) {
        sql2.prepare("Update NoteTable set notebook=:note where lid=:lid");
        sql2.bindValue(":note", name);
        sql2.bindValue(":lid", sql.value(0).toLongLong());
        sql2.exec();
    }
}


qint32 NoteTable::getNotesWithTag(QList<qint32> &retval, QString tag) {
    QSqlQuery query;
    TagTable tagTable;
    qint32 tagLid = tagTable.getLid(tag);
    query.prepare("Select lid data from DataStore where data=:tag and key=:key");
    query.bindValue(":tag", tagLid);
    query.bindValue(":key", NOTE_TAG);
    query.exec();
    while(query.next()) {
        retval.append(query.value(0).toInt());
    }
    return retval.size();
}


void NoteTable::setIndexNeeded(qint32 lid, bool indexNeeded) {
    QSqlQuery query;
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", indexNeeded);
    query.exec();
}


qint32 NoteTable::getIndexNeeded(QList<qint32> &lids) {
    QSqlQuery query;
    lids.empty();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}


void NoteTable::updateNotebook(qint32 noteLid, qint32 notebookLid, bool setAsDirty=false) {
    Notebook book;
    NotebookTable notebookTable;
    notebookTable.get(book, notebookLid);

    if (book.__isset.guid) {
        QSqlQuery query;
        query.prepare("Update DataStore set data=:notebookGuid where lid=:lid and key=:key;");
        query.bindValue(":notebookGuid", QString::fromStdString(book.guid));
        query.bindValue(":lid", noteLid);
        query.bindValue(":key", NOTE_NOTEBOOK);
        query.exec();

        if (setAsDirty) {
            setDirty(noteLid, setAsDirty);
        }

        QString bookName = QString::fromStdString(book.name);
        query.prepare("Update NoteTable set notebook=:name where lid=:lid");
        query.bindValue(":name", bookName);
        query.bindValue(":lid", noteLid);
        query.exec();

        query.prepare("Update NoteTable set notebookLid=:nlid where lid=:lid");
        query.bindValue(":nlid", notebookLid);
        query.bindValue(":lid", noteLid);
        query.exec();
    }
}



void NoteTable::updateUrl(qint32 noteLid, QString url, bool setAsDirty=false) {
    QSqlQuery query;
    query.prepare("Update DataStore set data=:url where lid=:lid and key=:key;");
    query.bindValue(":url", url);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_URL);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set SourceURl=:url where lid=:lid");
    query.bindValue(":url", url);
    query.bindValue(":lid", noteLid);
    query.exec();

}



void NoteTable::updateTitle(qint32 noteLid, QString title, bool setAsDirty=false) {
    QSqlQuery query;
    query.prepare("Update DataStore set data=:url where lid=:lid and key=:key;");
    query.bindValue(":url", title);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_TITLE);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set Title=:url where lid=:lid");
    query.bindValue(":url", title);
    query.bindValue(":lid", noteLid);
    query.exec();

}



void NoteTable::updateAuthor(qint32 noteLid, QString author, bool setAsDirty=false) {
    QSqlQuery query;
    query.prepare("Update DataStore set data=:author where lid=:lid and key=:key;");
    query.bindValue(":author", author);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set Author=:author where lid=:lid");
    query.bindValue(":url", author);
    query.bindValue(":lid", noteLid);
    query.exec();

}


void NoteTable::updateDate(qint32 lid, Timestamp ts, qint32 key, bool isDirty = false) {
    QSqlQuery query;
    query.prepare("Update DataStore set data=:ts where lid=:lid and key=:key;");
    query.bindValue(":ts", QVariant::fromValue(ts));
    query.bindValue(":lid", lid);
    query.bindValue(":key",key);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }

    if (key == NOTE_CREATED_DATE)
        query.prepare("Update NoteTable set dateCreated=:date where lid=:lid");
    if (key == NOTE_UPDATED_DATE)
        query.prepare("Update NoteTable set dateUpdated=:date where lid=:lid");
    if (key == NOTE_DELETED_DATE)
        query.prepare("Update NoteTable set dateDeleted=:date where lid=:lid");
    if (key == NOTE_ATTRIBUTE_SUBJECT_DATE)
        query.prepare("Update NoteTable set dateSubject=:date where lid=:lid");

    query.bindValue(":date", QVariant::fromValue(ts));
    query.bindValue(":lid", lid);
    query.exec();
}



void NoteTable::removeTag(qint32 lid, qint32 tag, bool isDirty = false) {
    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG);
    query.bindValue(":tag:", tag);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


void NoteTable::addTag(qint32 lid, qint32 tag, bool isDirty = false) {
    QSqlQuery query;
    query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :tag)");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG);
    query.bindValue(":tag:", tag);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


void NoteTable::rebuildNoteListTags(qint32 lid) {
    // update the note list
    QStringList tagNames;
    TagTable tagTable;
    QSqlQuery query;
    query.prepare("select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_TAG);
    query.exec();
    while (query.next()) {
        qint32 tagLid = query.value(0).toInt();
        Tag t;
        tagTable.get(t, tagLid);
        tagNames.append(QString::fromStdString(t.name));
    }
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);
    QString tagCol;
    for (qint32 i=0; i<tagNames.size(); i++) {
        tagCol = tagCol + tagNames[i];
        if (i<tagNames.size()-1)
            tagCol=tagCol+", ";
    }
    query.prepare("update NoteTable set tags=:tags where lid=:lid");
    query.bindValue(":tags", tagCol);
    query.bindValue(":lid", lid);
    query.exec();
}


void NoteTable::setDirty(qint32 lid, bool dirty) {
    QSqlQuery query;
    query.prepare("Update NoteTable set isDirty=:isDirty where lid=:lid");
    query.bindValue(":isDirty", !dirty);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("Update DataStore set data=:dirty where lid=:lid and key=:key");
    query.bindValue(":data", dirty);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
}


void NoteTable::deleteNote(qint32 lid, bool isDirty=true) {
    QSqlQuery query;
    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_DELETED_DATE);
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("delete from DataStore where key=:key and lid=:lid");
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":lid", lid);
        query.exec();
    }

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":data", false);
    query.exec();

    query.prepare("update notetable set dateDeleted=strftime('%s','now') where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", true);
        query.exec();

    }
}


void NoteTable::expunge(qint32 lid) {
    Note note;
    this->get(note, lid, true, false);
    ResourceTable resTable;
    for (unsigned int i=0; note.__isset.resources && i<note.resources.size(); i++) {
        Resource r = note.resources[i];
        resTable.expunge(r.guid);
    }

    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
}


void NoteTable::expunge(string guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


void NoteTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}





qint32 NoteTable::findNotesByNotebook(QList<qint32> &notes, QString guid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:notebookGuid");
    query.bindValue(":key", NOTE_NOTEBOOK);
    query.bindValue(":notebookGuid", guid);
    query.exec();
    while (query.next()) {
        notes.append(query.value(0).toInt());
    }
    return notes.size();
}


qint32 NoteTable::findNotesByNotebook(QList<qint32> &notes, string guid) {
    return findNotesByNotebook(notes, QString::fromStdString(guid));
}


void NoteTable::updateNoteContent(qint32 lid, QString content, bool isDirty) {
    QSqlQuery query;
    query.prepare("update datastore set data=:content where lid=:lid and key=:key");
    query.bindValue(":content", content);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_CONTENT);
    query.exec();

    query.prepare("update datastore set data=:content where lid=:lid and key=:key");
    query.bindValue(":content", content.length());
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_CONTENT_LENGTH);
    query.exec();

    query.prepare("update datastore set data=:dirty where lid=:lid and key=:key");
    query.bindValue(":dirty", isDirty);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();

    query.prepare("update datastore set data='true' where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();


}

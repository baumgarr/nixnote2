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
#include "resourcetable.h"
#include "configstore.h"
#include "notetable.h"
#include "utilities/mimereference.h"

#include <QSqlTableModel>

#include <iostream>
#include <fstream>

using namespace std;
extern Global global;

// Default constructor
ResourceTable::ResourceTable()
{
}



// Given a resource's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void ResourceTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE() << "Entering ResourceTable::updateGuid()";

    QSqlQuery query;
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();

    QLOG_TRACE() << "Leaving ResourceTable::updateGuid()";
}




// Synchronize a new resource with what is in the database.  We basically
// just delete the old one & give it a new entry
void ResourceTable::sync(Resource &resource) {
    QLOG_TRACE() << "Leaving ResourceTable::sync()";
    sync(0, resource);
    QLOG_TRACE() << "Leaving ResourceTable::sync()";
}



// Synchronize a new resource with what is in the database.  We basically
// just delete the old one & give it a new entry
void ResourceTable::sync(qint32 lid, Resource &resource) {
    QLOG_TRACE() << "Leaving ResourceTable::sync()";

    if (lid > 0) {
        QSqlQuery query;

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
    } else {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }

    add(lid, resource, false);

    QLOG_TRACE() << "Leaving ResourceTable::sync()";
}




// Given a resource's GUID, we return the LID
qint32 ResourceTable::getLid(QString noteGuid, QString guid) {

    QSqlQuery query;
    NoteTable n;
    qint32 noteLid = n.getLid(noteGuid);
    query.prepare("Select a.lid from DataStore a where a.key=:key and a.lid = (select b.lid from DataStore b where b.key=:key2 and b.data=:noteGuid)");
    query.bindValue(":data", guid);
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":noteGuid", noteLid);
    query.bindValue(":key2", RESOURCE_NOTE_LID);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Given a resource's GUID, we return the LID
qint32 ResourceTable::getLid(string noteGuid, string guid) {
    QString nGuid(QString::fromStdString(noteGuid));
    QString rGuid(QString::fromStdString(guid));
    return getLid(nGuid, rGuid);
}


// Get the lid for a resource guid
qint32 ResourceTable::getLid(string resourceGuid) {
    return this->getLid(QString::fromStdString(resourceGuid));
}


// Get the lid for a given resource's guid
qint32 ResourceTable::getLid(QString resourceGuid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:guid");
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":data", resourceGuid);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Get the guid for a given resource lid
QString ResourceTable::getGuid(int lid) {
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        return query.value(0).toString();
    else
        return "";
}

// Return a resource structure given the LID
bool ResourceTable::get(Resource &resource, qint32 lid) {

    QSqlQuery query;
    QByteArray byteArray;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    NoteTable ntable;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (RESOURCE_GUID):
            resource.guid = query.value(1).toString().toStdString();
            resource.__isset.guid = true;
            break;
        case (RESOURCE_NOTE_LID):
            resource.noteGuid = ntable.getGuid(query.value(1).toInt()).toStdString();
            resource.__isset.noteGuid = true;
            break;
        case (RESOURCE_DATA_BODY):
//            byteArray.clear();
//            byteArray.append(query.value(1).toByteArray());
//            resource.data.body = QString(byteArray).toStdString();
//            resource.__isset.data = true;
//            resource.data.__isset.body = true;
              break;
        case (RESOURCE_DATA_HASH):
            byteArray.clear();
            byteArray.append(query.value(1).toByteArray());
            resource.data.bodyHash = QString(QByteArray::fromHex(byteArray)).toStdString();
            resource.__isset.data = true;
            resource.data.__isset.bodyHash = true;
            break;
        case (RESOURCE_DATA_SIZE):
            resource.data.size = query.value(1).toInt();
            resource.__isset.recognition = true;
            resource.data.__isset.size = true;
            break;
        case (RESOURCE_MIME):
            resource.mime = query.value(1).toString().toStdString();
            resource.__isset.mime = true;
            break;
        case (RESOURCE_ACTIVE):
            resource.active = query.value(1).toBool();
            resource.__isset.active = true;
            break;
        case (RESOURCE_HEIGHT):
            resource.height = query.value(1).toString().toInt();
            resource.__isset.height = true;
            break;
        case (RESOURCE_WIDTH):
            resource.width = query.value(1).toString().toInt();
            resource.__isset.width = true;
            break;
        case (RESOURCE_DURATION):
            resource.duration = query.value(1).toString().toInt();
            resource.__isset.duration = true;
            break;
        case (RESOURCE_RECOGNITION_BODY):
            resource.recognition.body = query.value(1).toString().toStdString();
            resource.__isset.recognition = true;
            resource.recognition.__isset.body = true;
            break;
        case (RESOURCE_RECOGNITION_HASH):
            byteArray.clear();
            byteArray.append(query.value(1).toByteArray());
            resource.recognition.bodyHash = QString(QByteArray::fromHex(byteArray)).toStdString();
            resource.__isset.recognition = true;
            resource.recognition.__isset.bodyHash = true;
            break;
        case (RESOURCE_RECOGNITION_SIZE):
            resource.recognition.size = query.value(1).toInt();
            resource.__isset.recognition = true;
            resource.recognition.__isset.size = true;
            break;
        case (RESOURCE_UPDATE_SEQUENCE_NUMBER):
            resource.duration = query.value(1).toString().toInt();
            resource.__isset.duration = true;
            break;
        case (RESOURCE_ALTERNATE_BODY):
            resource.alternateData.body = query.value(1).toString().toStdString();
            resource.__isset.alternateData = true;
            resource.alternateData.__isset.body = true;
            break;
        case (RESOURCE_ALTERNATE_HASH):
            byteArray.clear();
            byteArray.append(query.value(1).toByteArray());
            resource.alternateData.bodyHash = QString(QByteArray::fromHex(byteArray)).toStdString();
            resource.__isset.alternateData = true;
            resource.alternateData.__isset.bodyHash = true;
            break;
        case (RESOURCE_ALTERNATE_SIZE):
            resource.alternateData.size = query.value(1).toInt();
            resource.__isset.alternateData = true;
            resource.alternateData.__isset.size = true;
            break;
        case (RESOURCE_SOURCE_URL):
            resource.attributes.sourceURL = query.value(1).toString().toStdString();
            resource.__isset.attributes = true;
            resource.attributes.__isset.sourceURL = true;
            break;
        case (RESOURCE_CAMERA_MAKE):
            resource.attributes.cameraMake = query.value(1).toString().toStdString();
            resource.__isset.attributes = true;
            resource.attributes.__isset.cameraMake = true;
            break;
        case (RESOURCE_CAMERA_MODEL):
            resource.attributes.cameraModel = query.value(1).toString().toStdString();
            resource.__isset.attributes = true;
            resource.attributes.__isset.cameraModel = true;
            break;
        case (RESOURCE_ALTITUDE):
            resource.attributes.altitude = query.value(1).toString().toDouble();
            resource.__isset.attributes = true;
            resource.attributes.__isset.altitude = true;
            break;
        case (RESOURCE_LONGITUDE):
            resource.attributes.longitude = query.value(1).toString().toDouble();
            resource.__isset.attributes = true;
            resource.attributes.__isset.longitude = true;
            break;
        case (RESOURCE_LATITUDE):
            resource.attributes.latitude = query.value(1).toString().toDouble();
            resource.__isset.attributes = true;
            resource.attributes.__isset.latitude = true;
            break;
        case (RESOURCE_RECO_TYPE):
            resource.attributes.recoType = query.value(1).toString().toStdString();
            resource.__isset.attributes = true;
            resource.attributes.__isset.recoType = true;
            break;
        case (RESOURCE_ATTACHMENT):
            resource.attributes.attachment = query.value(1).toBool();
            resource.__isset.attributes = true;
            resource.attributes.__isset.attachment = true;
            break;
        case (RESOURCE_FILENAME):
            resource.attributes.fileName = query.value(1).toString().toStdString();
            resource.__isset.attributes = true;
            resource.attributes.__isset.fileName = true;
            break;
        case (RESOURCE_CLIENT_WILL_INDEX):
            resource.attributes.clientWillIndex = query.value(1).toBool();
            resource.__isset.attributes = true;
            resource.attributes.__isset.clientWillIndex = true;
            break;
        case (RESOURCE_TIMESTAMP):
            resource.attributes.timestamp = query.value(1).toDouble();
            resource.__isset.attributes = true;
            resource.attributes.__isset.timestamp = true;
            break;
        }
    }

    // Now read the binary data from the disk
    QString mimetype = QString::fromStdString(resource.mime);
    MimeReference ref;
    QString filename;
    if (resource.__isset.attributes && resource.attributes.__isset.fileName)
        filename = QString::fromStdString(resource.attributes.fileName);
    QString fileExt = ref.getExtensionFromMime(mimetype, filename);
    QFile tfile(global.fileManager.getDbDirPath("/dba/"+QString::number(lid)) +fileExt );
    tfile.open(QIODevice::ReadOnly);
    QByteArray b = tfile.readAll();
    resource.data.body.clear();
    resource.data.body.append(b.data(), b.size());
    tfile.close();
    if (resource.data.body.size() > 0) {
        resource.data.__isset.body = true;
        resource.__isset.data = true;
    }

    return true;
}




// Return a resource given the GUID
bool ResourceTable::get(Resource &resource, QString noteGuid, QString guid) {
    qint32 lid = getLid(noteGuid, guid);
    return get(resource, lid);
}



// Return a resource given the GUID as a std::string
bool ResourceTable::get(Resource &resource, string noteGuid, string guid) {
    qint32 lid = getLid(noteGuid, guid);
    return get(resource, lid);
}



// Return if a resource is dirty given its lid
bool ResourceTable::isDirty(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Determine if a resource is dirty given a guid
bool ResourceTable::isDirty(QString noteGuid, QString guid) {
    qint32 lid = getLid(noteGuid, guid);
    return isDirty(lid);
}


// Determine if a resource is dirty given a guid
bool ResourceTable::isDirty(string noteGuid, string guid) {
    QString g(QString::fromStdString(guid));
    QString ng(QString::fromStdString(noteGuid));
    return isDirty(ng, g);
}


// Does this resource exist?
bool ResourceTable::exists(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


// Determine if a resource exists given a guid
bool ResourceTable::exists(QString noteGuid, QString guid) {
    qint32 lid = getLid(noteGuid, guid);
    return exists(lid);
}


// Determine if a resource exists given a guid
bool ResourceTable::exists(string noteGuid, string guid) {
    qint32 lid = getLid(noteGuid, guid);
    return exists(lid);
}


// Add a resource to the database
qint32 ResourceTable::add(qint32 l, Resource &t, bool isDirty) {
    ConfigStore cs;
    qint32 lid = l;
    if (lid <= 0)
        lid = cs.incrementLidCounter();
    else
        expunge(lid);

    QSqlQuery query;
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":data", QString::fromStdString(t.guid));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.bindValue(":data", true);
    query.exec();

    NoteTable noteTable;
    qint32 noteLid = noteTable.getLid(t.noteGuid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":data", noteLid);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (t.__isset.data) {
        if (t.data.__isset.size) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_DATA_SIZE);
            query.bindValue(":data", t.data.size);
            query.exec();
        }

        if (t.data.__isset.bodyHash) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_DATA_HASH);
            QByteArray b;
            b.append(t.data.bodyHash.data(),t.data.bodyHash.size());
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (t.data.__isset.body) {
            QString mimetype = QString::fromStdString(t.mime);
            QString filename;
            MimeReference ref;
            if (t.__isset.attributes && t.attributes.__isset.fileName)
                filename = QString::fromStdString(t.attributes.fileName);
            QString fileExt = ref.getExtensionFromMime(mimetype, filename);
            QFile tfile(global.fileManager.getDbDirPath("/dba/"+QString::number(lid)) +fileExt );
            tfile.open(QIODevice::WriteOnly);
            tfile.write(t.data.body.data(),t.data.size);
            tfile.close();
        }
    }

    if (t.__isset.mime) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_MIME);
        query.bindValue(":data", QString::fromStdString(t.mime));
        query.exec();
    }

    if (t.__isset.width) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_WIDTH);
        query.bindValue(":data", t.width);
        query.exec();
    }

    if (t.__isset.height) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_HEIGHT);
        query.bindValue(":data", t.height);
        query.exec();
    }

    if (t.__isset.duration) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_DURATION);
        query.bindValue(":data", t.duration);
        query.exec();
    }

    if (t.__isset.active) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_ACTIVE);
        query.bindValue(":data", t.active);
        query.exec();
    }

    if (t.__isset.recognition) {
        if (t.recognition.__isset.size) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_SIZE);
            query.bindValue(":data", t.recognition.size);
            query.exec();
        }

        if (t.recognition.__isset.bodyHash) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_HASH);
            QByteArray b;
            b.append(t.recognition.bodyHash.data());
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (t.recognition.__isset.body) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_BODY);
            query.bindValue(":data", QString::fromStdString(t.recognition.body));
            query.exec();
        }
    }

    if (t.__isset.updateSequenceNum) {
        query.bindValue(":key", RESOURCE_UPDATE_SEQUENCE_NUMBER);
        query.bindValue(":data", t.updateSequenceNum);
        query.exec();
    }


    if (t.__isset.alternateData) {
        if (t.alternateData.__isset.size) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_SIZE);
            query.bindValue(":data", t.alternateData.size);
            query.exec();
        }

        if (t.alternateData.__isset.bodyHash) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_HASH);
            QByteArray b;
            b.append(t.alternateData.bodyHash.data());
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (t.alternateData.__isset.body) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_BODY);
            query.bindValue(":data", QString::fromStdString(t.alternateData.body));
            query.exec();
        }
    }


    if (t.__isset.attributes) {
        if (t.attributes.__isset.sourceURL) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_SOURCE_URL);
            query.bindValue(":data", QString::fromStdString(t.attributes.sourceURL));
            query.exec();
        }

        if (t.attributes.__isset.timestamp) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_TIMESTAMP);
            query.bindValue(":data", QVariant::fromValue(t.attributes.timestamp));
            query.exec();
        }

        if (t.attributes.__isset.latitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_LATITUDE);
            query.bindValue(":data", t.attributes.latitude);
            query.exec();
        }

        if (t.attributes.__isset.longitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_LONGITUDE);
            query.bindValue(":data", t.attributes.longitude);
            query.exec();
        }

        if (t.attributes.__isset.altitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTITUDE);
            query.bindValue(":data", t.attributes.altitude);
            query.exec();
        }

        if (t.attributes.__isset.cameraMake) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CAMERA_MAKE);
            query.bindValue(":data", QString::fromStdString(t.attributes.cameraMake));
            query.exec();
        }

        if (t.attributes.__isset.cameraModel) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CAMERA_MODEL);
            query.bindValue(":data", QString::fromStdString(t.attributes.cameraModel));
            query.exec();
        }

        if (t.attributes.__isset.clientWillIndex) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CLIENT_WILL_INDEX);
            query.bindValue(":data", t.attributes.clientWillIndex);
            query.exec();
        }

        if (t.attributes.__isset.recoType) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECO_TYPE);
            query.bindValue(":data", QString::fromStdString(t.attributes.recoType));
            query.exec();
        }

        if (t.attributes.__isset.fileName) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_FILENAME);
            query.bindValue(":data", QString::fromStdString(t.attributes.fileName));
            query.exec();
        }

        if (t.attributes.__isset.attachment) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ATTACHMENT);
            query.bindValue(":data", t.attributes.attachment);
            query.exec();
        }
    }
    return lid;
}


// Get the recognition data for a resource
bool ResourceTable::getResourceRecognition(Resource &resource, qint32 lid) {

    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid and (key=:body or key=:size or key=:hash)");
    query.bindValue(":lid", lid);
    query.bindValue(":body", RESOURCE_RECOGNITION_BODY);
    query.bindValue(":size", RESOURCE_RECOGNITION_SIZE);
    query.bindValue(":hash", RESOURCE_RECOGNITION_HASH);

    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_HASH) {
            resource.recognition.bodyHash = query.value(1).toString().toStdString();
            resource.__isset.recognition = true;
            resource.recognition.__isset.bodyHash = true;
        }
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_SIZE) {
            resource.recognition.bodyHash = query.value(1).toLongLong();
            resource.__isset.recognition = true;
            resource.recognition.__isset.bodyHash = true;
        }
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_BODY) {
            resource.recognition.body = query.value(1).toString().toStdString();
            resource.__isset.recognition = true;
            resource.recognition.__isset.body = true;
        }
    }

    return true;
}


// Get a resource for a note by the resource data hash.  This is useful
// when going through a note and finding the resource for that note
qint32 ResourceTable::getLidByHashHex(QString noteGuid, QString hash) {
    NoteTable noteTable;
    qint32 notelid = noteTable.getLid(noteGuid);

    QSqlQuery query;
    query.prepare("Select lid from DataStore where data=:lid and key=:key");
    query.bindValue(":lid", notelid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.exec();
    while (query.next()) {
        QSqlQuery query2;
        qint32 lid = query.value(0).toInt();
        QByteArray b;
        b.append(hash);
        query2.prepare("Select lid from DataStore where data like :hash and key=:key and lid=:lid");
        query2.bindValue(":data", hash);
        query2.bindValue(":key", RESOURCE_DATA_HASH);
        query2.bindValue(":lid", lid);
        query2.exec();
        if (query2.next())
            return query2.value(0).toInt();
    }
    return 0;
}


// Get an ink note's data
bool ResourceTable::getInkNote(QByteArray &value, qint32 lid) {
    QString fileName = global.fileManager.getDbaDirPath()+QString::number(lid)+QString(".png");
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    value = f.readAll();
    if (value.size() > 0)
            return true;
    return false;
}


// Set/unset the index needed flag
void ResourceTable::setIndexNeeded(qint32 lid, bool indexNeeded) {
    QSqlQuery query;
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.exec();

    if (indexNeeded) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_INDEX_NEEDED);
        query.bindValue(":data", indexNeeded);
        query.exec();
    }
}


// Get a list of all resources that need indexing
qint32 ResourceTable::getIndexNeeded(QList<qint32> &lids) {
    QSqlQuery query;
    lids.empty();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}



// Get a list of all resource LIDs for a given note
bool ResourceTable::getResourceList(QList<qint32> &resourceList, qint32 noteLid) {

    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:notelid");
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":notelid", noteLid);
    query.exec();
    while (query.next()) {
        int resLid = query.value(0).toInt();
        resourceList.append(resLid);
    }
    if (resourceList.size() > 0)
        return true;
    else
        return false;
}


// Permanently delete a resource
void ResourceTable::expunge(qint32 lid) {
    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    // Delete the physical files
    QDir myDir(global.fileManager.getDbaDirPath());
    QString num = QString::number(lid);
    QStringList filter;
    filter.append(num+QString(".*"));
    QStringList list = myDir.entryList(filter, QDir::Files, QDir::NoSort);	// filter resource files
    for (int i=0; i<list.size(); i++) {
        myDir.remove(list[i]);
    }
}


// Permanently delete a resource
void ResourceTable::expunge(string guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


// Permanently delete a resource
void ResourceTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


// Update the existing Resource's hash
void ResourceTable::updateResourceHash(qint32 lid, QByteArray newhash) {
    QSqlQuery query;
    query.prepare("Update datastore set data=:hash where key=:key and lid=:lid");
    query.bindValue(":hash", newhash.toHex());
    query.bindValue(":key", RESOURCE_DATA_HASH);
    query.bindValue(":lid", lid);
    query.exec();
}






// Get a count of all resources in the database
qint32 ResourceTable::getCount() {
    QSqlQuery query;
    query.prepare("Select count(lid) from DataStore where key=:key;");
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    return 0;
}


// Get the count of inindexed resources
qint32 ResourceTable::getUnindexedCount() {
    QSqlQuery query;
    query.prepare("Select count(lid) from DataStore where key=:key and data='true'");
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    return 0;
}


// Add a stub resource.  This is a placeholder for a full resource that
// should be added later.
qint32 ResourceTable::addStub(qint32 resLid, qint32 noteLid) {
    QSqlQuery query;
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":data", noteLid);
    query.exec();
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":data", QString::number(resLid));
    query.exec();
}

// Get the owning note's LID for a resource.
qint32 ResourceTable::getNoteLid(qint32 resLid) {
    QSqlQuery query;
    query.prepare("Select data from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.exec();
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}


QByteArray ResourceTable::getDataHash(qint32 lid) {
        QSqlQuery query;
        query.prepare("Select data from datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_DATA_HASH);
        query.exec();
        if (query.next()) {
            return QByteArray::fromHex(query.value(0).toByteArray());
        }
        return QByteArray();

}


void ResourceTable::reindexAllResources() {
    QSqlQuery query;
    query.prepare("delete from datastore where key=:indexKey");
    query.bindValue(":indexKey", RESOURCE_INDEX_NEEDED);
    query.exec();

    query.prepare("insert into datastore (lid, key, data) select lid, :indexKey, 'true' from datastore where key=:key;");
    query.bindValue(":indexKey", RESOURCE_INDEX_NEEDED);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
}

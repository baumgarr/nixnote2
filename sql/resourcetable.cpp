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
#include "sql/nsqlquery.h"
#include "utilities/noteindexer.h"

#include <QSqlTableModel>

#include <iostream>
#include <fstream>

using namespace std;
extern Global global;

// Default constructor
ResourceTable::ResourceTable(DatabaseConnection *db)
{
    this->db = db;
}



// Given a resource's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void ResourceTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE() << "Entering ResourceTable::updateGuid()";

    db->lockForWrite();
    NSqlQuery query(db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", guid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    query.finish();
    db->unlock();

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
        expunge(lid);
        NSqlQuery query(db);
        // Delete the old record
        db->lockForWrite();
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
        db->unlock();

    } else {
        ConfigStore cs(db);
        lid = cs.incrementLidCounter();
    }

    add(lid, resource, false);

    QLOG_TRACE() << "Leaving ResourceTable::sync()";
}




// Given a resource's GUID, we return the LID
qint32 ResourceTable::getLid(QString noteGuid, QString guid) {

    NSqlQuery query(db);
    NoteTable n(db);
    db->lockForRead();
    qint32 noteLid = n.getLid(noteGuid);
    query.prepare("Select a.lid from DataStore a where a.data=:data and a.key=:key and a.lid = (select distinct b.lid from DataStore b where b.key=:key2 and b.data=:noteLid)");
    query.bindValue(":data", guid);
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":key2", RESOURCE_NOTE_LID);
    query.bindValue(":noteLid", noteLid);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
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
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:guid");
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":data", resourceGuid);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}


// Get the guid for a given resource lid
QString ResourceTable::getGuid(int lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    QString retval = "";
    if (query.next())
        retval = query.value(0).toString();
    query.finish();
    db->unlock();
    return retval;
}

// Return a resource structure given the LID
bool ResourceTable::get(Resource &resource, qint32 lid, bool withBinary) {

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0) {
        db->unlock();
        return false;
    }
    while (query.next()) {
        mapResource(query, resource);
    }
    query.finish();
    db->unlock();

    // Now read the binary data from the disk
    if (withBinary) {
        QString mimetype = resource.mime;
        MimeReference ref;
        QString filename;
        ResourceAttributes attributes;
        if (resource.attributes.isSet())
            attributes = resource.attributes;
        if (attributes.fileName.isSet())
            filename = attributes.fileName;
        QString fileExt = ref.getExtensionFromMime(mimetype, filename);
        QFile tfile(global.fileManager.getDbDirPath("/dba/"+QString::number(lid)) +fileExt );
        tfile.open(QIODevice::ReadOnly);
        QByteArray b = tfile.readAll();
        Data d;
        if (resource.data.isSet())
            d = resource.data;
        d.body = b;
        resource.data = d;
        tfile.close();
    }

    return true;
}



// Save a resource's map data.
void ResourceTable::mapResource(NSqlQuery &query, Resource &resource) {
    NoteTable ntable(db);
    Data d, rd, ad;
    ResourceAttributes attributes;
    if (resource.data.isSet())
        d = resource.data;
    if (resource.recognition.isSet())
        rd = resource.recognition;
    if (resource.alternateData.isSet())
        ad = resource.alternateData;
    if (resource.attributes.isSet())
        attributes = resource.attributes;
    qint32 key = query.value(0).toInt();
    switch (key) {
    case (RESOURCE_GUID):
        resource.guid = query.value(1).toString();
        break;
    case (RESOURCE_NOTE_LID):
        resource.noteGuid = ntable.getGuid(query.value(1).toInt());
        break;
    case (RESOURCE_DATA_BODY):
          break;
    case (RESOURCE_DATA_HASH):
        d.bodyHash = QByteArray::fromHex(query.value(1).toByteArray());
        resource.data = d;
        break;
    case (RESOURCE_DATA_SIZE):
        d.size = query.value(1).toInt();
        resource.data = d;
        break;
    case (RESOURCE_MIME):
        resource.mime = query.value(1).toString();
        break;
    case (RESOURCE_ACTIVE):
        resource.active = query.value(1).toBool();
        break;
    case (RESOURCE_HEIGHT):
        resource.height = query.value(1).toString().toInt();
        break;
    case (RESOURCE_WIDTH):
        resource.width = query.value(1).toString().toInt();
        break;
    case (RESOURCE_DURATION):
        resource.duration = query.value(1).toString().toInt();
        break;
    case (RESOURCE_RECOGNITION_BODY):
        rd.body = query.value(1).toByteArray();
        resource.recognition = rd;
        break;
    case (RESOURCE_RECOGNITION_HASH):
        rd.bodyHash = query.value(1).toByteArray();
        resource.recognition = rd;
        break;
    case (RESOURCE_RECOGNITION_SIZE):
        rd.size = query.value(1).toInt();
        resource.recognition = rd;
        break;
    case (RESOURCE_UPDATE_SEQUENCE_NUMBER):
        resource.duration = query.value(1).toString().toInt();
        break;
    case (RESOURCE_ALTERNATE_BODY):
        ad.body = query.value(1).toByteArray();
        resource.alternateData = ad;
        break;
    case (RESOURCE_ALTERNATE_HASH):
        ad.bodyHash = query.value(1).toByteArray();
        resource.alternateData = ad;
        break;
    case (RESOURCE_ALTERNATE_SIZE):
        ad.size = query.value(1).toInt();
        resource.alternateData = ad;
        break;
    case (RESOURCE_SOURCE_URL):
        attributes.sourceURL = query.value(1).toString();
        resource.attributes = attributes;
        break;
    case (RESOURCE_CAMERA_MAKE):
        attributes.cameraMake = query.value(1).toString();
        resource.attributes = attributes;
        break;
    case (RESOURCE_CAMERA_MODEL):
        attributes.cameraModel = query.value(1).toString();
        resource.attributes = attributes;
        break;
    case (RESOURCE_ALTITUDE):
        attributes.altitude = query.value(1).toString().toDouble();
        resource.attributes = attributes;
        break;
    case (RESOURCE_LONGITUDE):
        attributes.longitude = query.value(1).toString().toDouble();
        resource.attributes = attributes;
        break;
    case (RESOURCE_LATITUDE):
        attributes.latitude = query.value(1).toString().toDouble();
        resource.attributes = attributes;
        break;
    case (RESOURCE_RECO_TYPE):
        attributes.recoType = query.value(1).toString();
        resource.attributes = attributes;
        break;
    case (RESOURCE_ATTACHMENT):
        attributes.attachment = query.value(1).toBool();
        resource.attributes = attributes;
        break;
    case (RESOURCE_FILENAME):
        attributes.fileName = query.value(1).toString();
        resource.attributes = attributes;
        break;
    case (RESOURCE_CLIENT_WILL_INDEX):
        attributes.clientWillIndex = query.value(1).toBool();
        resource.attributes = attributes;
        break;
    case (RESOURCE_TIMESTAMP):
        attributes.timestamp = query.value(1).toDouble();
        resource.attributes = attributes;
        break;
    }
}




// Return a resource given the GUID
bool ResourceTable::get(Resource &resource, QString noteGuid, QString guid, bool withBinary) {
    qint32 lid = getLid(noteGuid, guid);
    return get(resource, lid, withBinary);
}



// Return a resource given the GUID as a std::string
bool ResourceTable::get(Resource &resource, string noteGuid, string guid, bool withBinary) {
    qint32 lid = getLid(noteGuid, guid);
    return get(resource, lid, withBinary);
}



// Return if a resource is dirty given its lid
bool ResourceTable::isDirty(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_ISDIRTY);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    db->unlock();
    return retval;
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
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = true;
    query.finish();
    db->unlock();
    return retval;
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
qint32 ResourceTable::add(qint32 l, Resource &t, bool isDirty, int noteLid) {
    ConfigStore cs(db);
    qint32 lid = l;
    if (lid <= 0)
        lid = cs.incrementLidCounter();
    else
        expunge(lid);

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    if (t.guid.isSet()) {
        QString guid = t.guid;
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_GUID);
        query.bindValue(":data", guid);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.bindValue(":data", true);
    query.exec();

    if (noteLid <=0) {
        NoteTable noteTable(db);
        noteLid = noteTable.getLid(t.noteGuid);
        if (noteLid <=0) {
            noteLid = noteTable.addStub(t.noteGuid);
        }
    }
    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":data", noteLid);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", RESOURCE_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (t.data.isSet()) {
        Data d = t.data;
        if (d.size.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_DATA_SIZE);
            qint32 size = d.size;
            query.bindValue(":data", size);
            query.exec();
        }

        if (d.bodyHash.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_DATA_HASH);
            QByteArray b;
            b.append(d.bodyHash);
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (d.body.isSet()) {
            QString mimetype = t.mime;
            QString filename;
            MimeReference ref;
            ResourceAttributes attributes;
            if (t.attributes.isSet())
                attributes = t.attributes;
            if (attributes.fileName.isSet())
                filename = attributes.fileName;
            QString fileExt = ref.getExtensionFromMime(mimetype, filename);
            QFile tfile(global.fileManager.getDbDirPath("/dba/"+QString::number(lid)) +fileExt );
            tfile.open(QIODevice::WriteOnly);
            if (d.size > 0)
                tfile.write(d.body);
            tfile.close();
        }
    }

    if (t.mime.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_MIME);
        QString mime = t.mime;
        query.bindValue(":data", mime);
        query.exec();
    }

    if (t.width.isSet()) {
        qint16 width = t.width;
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_WIDTH);
        query.bindValue(":data", width);
        query.exec();
    }

    if (t.height.isSet()) {
        qint16 height = t.height;
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_HEIGHT);
        query.bindValue(":data", height);
        query.exec();
    }

    if (t.duration.isSet()) {
        qint16 duration = t.duration;
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_DURATION);
        query.bindValue(":data", duration);
        query.exec();
    }

    if (t.active.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_ACTIVE);
        bool active = t.active;
        query.bindValue(":data", active);
        query.exec();
    }

    if (t.recognition.isSet()) {
        Data r = t.recognition;
        if (r.size.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_SIZE);
            qint32 size = r.size;
            query.bindValue(":data", size);
            query.exec();
        }

        if (r.bodyHash.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_HASH);
            QByteArray b;
            b.append(r.bodyHash);
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (r.body.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECOGNITION_BODY);
            QByteArray body = r.body;
            query.bindValue(":data", body);
            query.exec();
        }
    }

    if (t.updateSequenceNum.isSet()) {
        qint32 usn =t.updateSequenceNum;
        query.bindValue(":key", RESOURCE_UPDATE_SEQUENCE_NUMBER);
        query.bindValue(":data", usn);
        query.exec();
    }


    if (t.alternateData.isSet()) {
        Data ad = t.alternateData;
        if (ad.size.isSet()) {
            qint32 size = ad.size;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_SIZE);
            query.bindValue(":data", size);
            query.exec();
        }

        if (ad.bodyHash.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_HASH);
            QByteArray b;
            b.append(ad.bodyHash);
            query.bindValue(":data", b.toHex());
            query.exec();
        }

        if (ad.body.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTERNATE_BODY);
            QByteArray body = ad.body;
            query.bindValue(":data", body);
            query.exec();
        }
    }


    if (t.attributes.isSet()) {
        ResourceAttributes ra = t.attributes;
        if (ra.sourceURL.isSet()) {
            query.bindValue(":lid", lid);
            QString url = ra.sourceURL;
            query.bindValue(":key", RESOURCE_SOURCE_URL);
            query.bindValue(":data", url);
            query.exec();
        }

        if (ra.timestamp.isSet()) {
            qlonglong ts = ra.timestamp;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_TIMESTAMP);
            query.bindValue(":data", ts);
            query.exec();
        }

        if (ra.latitude.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_LATITUDE);
            double lat = ra.latitude;
            query.bindValue(":data", lat);
            query.exec();
        }

        if (ra.longitude.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_LONGITUDE);
            double lon = ra.longitude;
            query.bindValue(":data", lon);
            query.exec();
        }

        if (ra.altitude.isSet()) {
            double alt = ra.altitude;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ALTITUDE);
            query.bindValue(":data", alt);
            query.exec();
        }

        if (ra.cameraMake.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CAMERA_MAKE);
            QString cameramake = ra.cameraMake;
            query.bindValue(":data", cameramake);
            query.exec();
        }

        if (ra.cameraModel.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CAMERA_MODEL);
            QString model = ra.cameraModel;
            query.bindValue(":data", model);
            query.exec();
        }

        if (ra.clientWillIndex.isSet()) {
            bool cwi = ra.clientWillIndex;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_CLIENT_WILL_INDEX);
            query.bindValue(":data", cwi);
            query.exec();
        }

        if (ra.recoType.isSet()) {
            QString reco = ra.recoType;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_RECO_TYPE);
            query.bindValue(":data", reco);
            query.exec();
        }

        if (ra.fileName.isSet()) {
            QString filename = ra.fileName;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_FILENAME);
            query.bindValue(":data", filename);
            query.exec();
        }

        if (ra.attachment.isSet()) {
            bool attachment = ra.attachment;
            query.bindValue(":lid", lid);
            query.bindValue(":key", RESOURCE_ATTACHMENT);
            query.bindValue(":data", attachment);
            query.exec();
        }
    }
    query.finish();
    db->unlock();

    NoteIndexer indexer;
    indexer.indexResource(lid);
    return lid;
}


// Get the recognition data for a resource
bool ResourceTable::getResourceRecognition(Resource &resource, qint32 lid) {

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select key, data from DataStore where lid=:lid and (key=:body or key=:size or key=:hash)");
    query.bindValue(":lid", lid);
    query.bindValue(":body", RESOURCE_RECOGNITION_BODY);
    query.bindValue(":size", RESOURCE_RECOGNITION_SIZE);
    query.bindValue(":hash", RESOURCE_RECOGNITION_HASH);

    query.exec();
    if (query.size() == 0)
        return false;
    Data d;
    if (resource.recognition.isSet())
        d = resource.recognition;
    while (query.next()) {
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_HASH) {
            d.bodyHash = query.value(1).toByteArray();
        }
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_SIZE) {
            d.size = query.value(1).toLongLong();
        }
        if (query.value(0).toInt() == RESOURCE_RECOGNITION_BODY) {
            d.body = query.value(1).toByteArray();
        }
    }
    query.finish();
    db->unlock();

    resource.recognition = d;
    return true;
}


// Get a resource for a note by the resource data hash.  This is useful
// when going through a note and finding the resource for that note
qint32 ResourceTable::getLidByHashHex(QString noteGuid, QString hash) {
    NoteTable noteTable(db);
    qint32 notelid = noteTable.getLid(noteGuid);

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where data=:lid and key=:key");
    query.bindValue(":lid", notelid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.exec();
    while (query.next()) {
        NSqlQuery query2(db);
        qint32 lid = query.value(0).toInt();
        QByteArray b;
        b.append(hash);
        query2.prepare("Select lid from DataStore where data like :hash and key=:key and lid=:lid");
        query2.bindValue(":hash", hash);
        query2.bindValue(":key", RESOURCE_DATA_HASH);
        query2.bindValue(":lid", lid);
        query2.exec();
        if (query2.next()) {
            db->unlock();
            return query2.value(0).toInt();
        }
    }
    db->unlock();
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
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();

    NoteIndexer indexer;
    indexer.indexResource(lid);

    db->unlock();
}


// Get a list of all resources that need indexing
qint32 ResourceTable::getIndexNeeded(QList<qint32> &lids) {
    NSqlQuery query(db);
    lids.clear();
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}



// Get a list of all resource LIDs for a given note
bool ResourceTable::getResourceList(QList<qint32> &resourceList, qint32 noteLid) {

    resourceList.clear();
    db->lockForRead();
    NSqlQuery query(db);
    query.prepare("Select lid from DataStore where key=:key and data=:notelid");
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":notelid", noteLid);
    query.exec();
    while (query.next()) {
        int resLid = query.value(0).toInt();
        resourceList.append(resLid);
    }
    query.finish();
    db->unlock();

    if (resourceList.size() > 0)
        return true;
    else
        return false;
}


// Permanently delete a resource
void ResourceTable::expunge(qint32 lid) {

    if (!this->exists(lid)) {
        return;
    }
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();

    // Delete the physical files (resource)
    QDir myDir(global.fileManager.getDbaDirPath());
    QString num = QString::number(lid);
    QStringList filter;
    filter.append(num+QString(".*"));
    QStringList list = myDir.entryList(filter, QDir::Files, QDir::NoSort);	// filter resource files
    for (int i=0; i<list.size(); i++) {
        myDir.remove(list[i]);
    }

    // Delete the physical files (thumbnail)
    QDir myTDir(global.fileManager.getThumbnailDirPath());
    list = myTDir.entryList(filter, QDir::Files, QDir::NoSort);	// filter resource files
    for (int i=0; i<list.size(); i++) {
        myTDir.remove(list[i]);
    }
}



// Permanently delete a resource
void ResourceTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


// Permanently delete a resource
void ResourceTable::expungeByNote(qint32 notebookLid) {
    QList<qint32> lids;
    {
        NSqlQuery query(db);
        db->lockForRead();
        query.prepare("Select lid from datastore where data=:data and key=:key");
        query.bindValue(":key", RESOURCE_NOTE_LID);
        query.bindValue(":data", notebookLid);
        query.exec();
        db->unlock();
        while(query.next()) {
            qint32 lid = query.value(0).toInt();
            lids.append(lid);
        }
        query.finish();
    }

    for (int i=0; i<lids.size(); i++) {
        expunge(lids[i]);
    }
}


// Update the existing Resource's hash
void ResourceTable::updateResourceHash(qint32 lid, QByteArray newhash) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update datastore set data=:hash where key=:key and lid=:lid");
    query.bindValue(":hash", newhash.toHex());
    query.bindValue(":key", RESOURCE_DATA_HASH);
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();
}






// Get a count of all resources in the database
qint32 ResourceTable::getCount() {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select count(lid) from DataStore where key=:key;");
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}


// Get the count of inindexed resources
qint32 ResourceTable::getUnindexedCount() {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select count(lid) from DataStore where key=:key and data='true'");
    query.bindValue(":key", RESOURCE_INDEX_NEEDED);
    query.exec();
    qint32 retval =0;
    if (query.next())
        retval= query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}


// Add a stub resource.  This is a placeholder for a full resource that
// should be added later.
qint32 ResourceTable::addStub(qint32 resLid, qint32 noteLid) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.bindValue(":data", noteLid);
    query.exec();
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_GUID);
    query.bindValue(":data", QString::number(resLid));
    query.exec();
    query.finish();
    db->unlock();
    return resLid;
}

// Get the owning note's LID for a resource.
qint32 ResourceTable::getNoteLid(qint32 resLid) {
    NSqlQuery query(db);
    qint32 retval = 0;
    db->lockForRead();
    query.prepare("Select data from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", resLid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toInt();
    }
    db->unlock();
    return retval;
}



// Get a resource's HASH data
QByteArray ResourceTable::getDataHash(qint32 lid) {
        NSqlQuery query(db);
        db->lockForRead();
        query.prepare("Select data from datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", RESOURCE_DATA_HASH);
        query.exec();
        if (query.next()) {
            db->unlock();
            return QByteArray::fromHex(query.value(0).toByteArray());
        }
        db->unlock();
        return QByteArray();
}


// Mark all note resource as needing reindexed
void ResourceTable::reindexAllResources() {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from datastore where key=:indexKey");
    query.bindValue(":indexKey", RESOURCE_INDEX_NEEDED);
    query.exec();

    query.prepare("insert into datastore (lid, key, data) select lid, :indexKey, 'true' from datastore where key=:key;");
    query.bindValue(":indexKey", RESOURCE_INDEX_NEEDED);
    query.bindValue(":key", RESOURCE_GUID);
    query.exec();
    query.finish();
    db->unlock();
}



// Update a resource's owning note.  This is done when merging notes
void ResourceTable::updateNoteLid(qint32 resourceLid, qint32 newNoteLid) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update datastore set data=:newNoteLid where lid=:resourceLid and key=:key");
    query.bindValue(":newNoteLid", newNoteLid);
    query.bindValue(":lid", resourceLid);
    query.bindValue(":key", RESOURCE_NOTE_LID);
    query.exec();
    query.finish();
    db->unlock();
}


// Get a resource's map data
void ResourceTable::getResourceMap(QHash<QString, qint32> &map, QHash< qint32, Resource > &resourceMap, QString guid) {
    NoteTable ntable(db);
    qint32 lid = ntable.getLid(guid);
    this->getResourceMap(map, resourceMap, lid);
}



// Get a resource's map data
void ResourceTable::getResourceMap(QHash<QString, qint32> &map, QHash<qint32, Resource> &resourceMap, string guid) {
    NoteTable ntable(db);
    qint32 lid = ntable.getLid(guid);
    this->getResourceMap(map, resourceMap, lid);
}



// Get a resource's map data
void ResourceTable::getResourceMap(QHash<QString, qint32> &hashMap, QHash<qint32, Resource> &resourceMap, qint32 noteLid) {
    NoteTable ntable(db);
    QString noteGuid = ntable.getGuid(noteLid);
    NSqlQuery query(db);
    qint32 prevLid = -1;
    db->lockForRead();
    query.prepare("Select key, data, lid from datastore where lid in (select lid from datastore where key=:key2 and data=:notelid) order by lid");
    query.bindValue(":key2", RESOURCE_NOTE_LID);
    query.bindValue(":noteLid", noteLid);
    query.exec();
    hashMap.clear();
    resourceMap.clear();
    Resource *r = NULL;
    QString hash;
    while (query.next()) {
        qint32 lid = query.value(2).toInt();

        // Peek at the next record to see if we are at the end
        // If this is the last result set, we force a save of the
        // record
        if (!query.next()) {
            prevLid = lid;
            lid = -1;
        }
        query.previous();

        // If these don't match, we need to save the current
        // record we are building.
        if (prevLid != lid) {
            if (prevLid > 0) {
                if (hash != "") {
                    hashMap.insert(hash, prevLid);
                    if (r == NULL)
                        r = new Resource();
                    if (!r->noteGuid.isSet()) {
                        r->noteGuid = noteGuid;
                    }
                    resourceMap.insert(prevLid, *r);
                    delete r;
                }
            }
            r = new Resource();
            prevLid = lid;
            hash = "";
        }
        qint32 key = query.value(0).toInt();
        if (key == RESOURCE_DATA_HASH) {
            hash = query.value(1).toString();
        }
        mapResource(query, *r);
    }
    db->unlock();
    query.finish();

    if (r != NULL)
        delete r;
}


// Get all resources for a note
void ResourceTable::getAllResources(QList<Resource> &list, qint32 noteLid, bool fullLoad, bool withBinary) {
    //NoteTable ntable(db);
    //QString noteGuid = ntable.getGuid(noteLid);
    NSqlQuery query(db);
    db->lockForRead();
    QHash<qint32, Resource*> lidMap;
    if (fullLoad){
        query.prepare("Select key, data, lid from datastore where lid in (select lid from datastore where key=:key2 and data=:notelid) order by lid");
        query.bindValue(":key2", RESOURCE_NOTE_LID);
        query.bindValue(":noteLid", noteLid);
    } else {
        query.prepare("Select key, data, lid from datastore where key=:key and lid in (select lid from datastore where key=:key2 and data=:notelid) order by lid");
        query.bindValue(":key", RESOURCE_GUID);
        query.bindValue(":key2", RESOURCE_NOTE_LID);
        query.bindValue(":noteLid", noteLid);
    }
    Resource *r = NULL;
    query.exec();
    while (query.next()) {
        qint32 lid = query.value(2).toInt();
        if (!lidMap.contains(lid)) {
            r = new Resource();
            lidMap.insert(lid, r);
        } else {
            r = lidMap[lid];
        }
        mapResource(query, *r);
    }
    query.finish();
    db->unlock();

    // if we need binary data, read it in.  Then add to the list
    QHash<qint32, Resource*>::iterator i;
    list.clear();
    for (i=lidMap.begin(); i!=lidMap.end(); ++i) {
        if (withBinary && fullLoad) {
            Resource *r = i.value();
            qint32 lid = i.key();
            QString mimetype = r->mime;
            MimeReference ref;
            QString filename;
            ResourceAttributes attributes;
            if (r->attributes.isSet())
                attributes = r->attributes;
            if (attributes.fileName.isSet())
                filename = attributes.fileName;
            QString fileExt = ref.getExtensionFromMime(mimetype, filename);
            QFile tfile(global.fileManager.getDbDirPath("/dba/"+QString::number(lid)) +fileExt );
            if (!tfile.open(QIODevice::ReadOnly)) {
                QDir dir(global.fileManager.getDbaDirPath());
                QStringList filterList;
                filterList.append(QString::number(lid)+".*");
                QStringList list= dir.entryList(filterList, QDir::Files);
                if (list.size() > 0) {
                    tfile.setFileName(global.fileManager.getDbaDirPath()+list[0]);
                    tfile.open(QIODevice::ReadOnly);
                }
            }
            QByteArray b = tfile.readAll();
            Data d;
            if (r->data.isSet())
                d = r->data;
            d.body = b;
            r->data = d;
            tfile.close();
        }
        list.append(*i.value());
    }
}

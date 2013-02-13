#ifndef RESOURCETABLE_H
#define RESOURCETABLE_H

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define RESOURCE_GUID                    6000
#define RESOURCE_NOTE_LID                6001
#define RESOURCE_DATA_BODY               6002
#define RESOURCE_DATA_HASH               6003
#define RESOURCE_DATA_SIZE               6004
#define RESOURCE_MIME                    6005
#define RESOURCE_ACTIVE                  6006
#define RESOURCE_HEIGHT                  6007
#define RESOURCE_WIDTH                   6008
#define RESOURCE_DURATION                6009
#define RESOURCE_RECOGNITION_BODY        6010
#define RESOURCE_RECOGNITION_SIZE        6011
#define RESOURCE_RECOGNITION_HASH        6012
#define RESOURCE_UPDATE_SEQUENCE_NUMBER  6013
#define RESOURCE_ALTERNATE_BODY          6014
#define RESOURCE_ALTERNATE_SIZE          6015
#define RESOURCE_ALTERNATE_HASH          6016
#define RESOURCE_SOURCE_URL              6017
#define RESOURCE_CAMERA_MAKE             6018
#define RESOURCE_CAMERA_MODEL            6019
#define RESOURCE_ALTITUDE                6020
#define RESOURCE_LONGITUDE               6021
#define RESOURCE_LATITUDE                6022
#define RESOURCE_RECO_TYPE               6023
#define RESOURCE_ATTACHMENT              6024
#define RESOURCE_FILENAME                6025
#define RESOURCE_CLIENT_WILL_INDEX       6026
#define RESOURCE_ISDIRTY                 6027
#define RESOURCE_TIMESTAMP               6028
#define RESOURCE_INKNOTE                 6029

#define RESOURCE_INDEX_NEEDED            6999


using namespace evernote::edam  ;
using namespace std;


class ResourceTable
{

private:

public:
    ResourceTable();                             // Constructor
    qint32 getLid(QString noteGuid, QString guid);            // given a guid, return the lid
    qint32 getLid(string noteGuid, string guid);             // Given a guid, return the lid
    qint32 getLid(string resourceGuid);
    qint32 getLid(QString resourceGuid);
    QString getGuid(int lid);                   // Given a lid, get the guid
    void updateGuid(qint32 lid, Guid &guid);    // Update a resource's guid
    void sync(Resource &resource);                    // Sync a resource with a new record
    void sync(qint32 lid, Resource &resource);           // Sync a resource with a new record
    qint32 add(qint32 lid, Resource &t, bool isDirty); // Add a new resource
    bool get(Resource &resource, qint32 lid);           // Get a resource given a lid
    bool get(Resource &resource, QString noteGuid, QString guid);      // get a resource given a guid
    bool get(Resource &resource, string noteGuid, string guid);       // get a resource given a guid
    bool isDirty(qint32 lid);                  // Check if a resource is dirty
    bool isDirty(QString noteGuid, QString guid);             // Check if a resource is dirty
    bool isDirty(string noteGuid, string guid);              // Check if a resource is dirty
    bool exists(qint32 lid);                                   // Does this resource exist?
    bool exists(QString noteGuid, QString guid);              // Does this resource exist?
    bool exists(string noteGuid, string guid);               // Does this resource exist?
    bool getResourceRecognition(Resource &resource, qint32 lid);
    qint32 getLidByHashHex(QString noteGuid, QString hash);
    bool getInkNote(QByteArray &value, qint32 lid);
    void setIndexNeeded(qint32 lid, bool indexNeeded);    // flag if a resource needs reindexing
    void expunge(int lid);                                // erase a resource
    void expunge(QString guid);                           // erase a resource
    void expunge(string guid);                            // erase a resource
    qint32 getIndexNeeded(QList<qint32> &lids);           // Get a list of all resources needing indexing
    bool getResourceList(QList<qint32> &resourceList, qint32 noteLid);  // Get resources for a note
    void updateResourceHash(qint32 lid, QString newhash);
    qint32 getCount();                                     // count of all resources
    qint32 getUnindexedCount();                            // count of unindexed resources
};


#endif // RESOURCETABLE_H

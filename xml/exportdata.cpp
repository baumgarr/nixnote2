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


#include "exportdata.h"
#include "global.h"
#include "sql/usertable.h"
#include "sql/tagtable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/linkednotebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/notebooktable.h"
#include "sql/searchtable.h"

#include <QProgressDialog>

extern Global global;


ExportData::ExportData(bool backup, QObject *parent) :
    QObject(parent)
{
    this->backup = backup;
    lastError = 0;
    errorMessage = "";
    lids.empty();
}



void ExportData::backupData(QString filename) {
    quitNow = false;
    QFile xmlFile(filename);
    if (!xmlFile.open(QIODevice::WriteOnly)) {
        lastError = 16;
        errorMessage = tr("Cannot open file.");
        return;
    }
    progress = new QProgressDialog();
    progress->setAutoClose(false);
    progress->setWindowModality(Qt::ApplicationModal);
    connect(progress, SIGNAL(canceled()), this, SLOT(abortBackup()));
    progress->setWindowTitle(tr("Export"));
    writer = new QXmlStreamWriter(&xmlFile);
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");
    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE NixNote-Export>");
    writer->writeStartElement("nixnote-export");
    writer->writeAttribute("version", "2");
    if (backup)
        writer->writeAttribute("exportType", "backup");
    else
        writer->writeAttribute("exportType", "export");
    writer->writeAttribute("application", "NixNote");
    writer->writeAttribute("applicationVersion", "2.x");
    if (backup) {
        NoteTable noteTable(global.db);
        noteTable.getAll(this->lids);
        progress->setWindowTitle(tr("Backup"));
        writer->writeStartElement("Synchronization");
        UserTable userTable(global.db);
        qlonglong lastSyncDate = userTable.getLastSyncDate();
        qint32 number = userTable.getLastSyncNumber();
        createNode("UpdateSequenceNumber", QString::number(number));
        createNode("LastSequenceDate", QString::number(lastSyncDate));
        writer->writeEndElement();

        writeNotebooks();
        writeTags();
        writeSavedSearches();
    }
    writeNotes();
    writer->writeEndElement();
    writer->writeEndDocument();
    progress->hide();
    xmlFile.close();
}



void ExportData::writeTags() {
    QList<qint32> tags;
    TagTable ttable(global.db);
    QList<qint32> dirtyTags;
    ttable.getAllDirty(dirtyTags);
    ttable.getAll(tags);

    progress->setLabelText(tr("Tags"));
    progress->setMaximum(lids.size());
    progress->show();
    for (int i=0; i<tags.size() && !quitNow; i++) {
        progress->setValue(i+1);
        QCoreApplication::processEvents();
        Tag tag;
        if (ttable.get(tag, tags[i])) {
            writer->writeStartElement("Tag");
            if (tag.__isset.guid)
                createNode("Guid", tag.guid);
            if (tag.__isset.name)
                createNode("Name", tag.name);
            if (tag.__isset.parentGuid)
                createNode("ParentGuid", tag.parentGuid);
            if (tag.__isset.updateSequenceNum)
                createNode("UpdateSequenceNumber", tag.updateSequenceNum);

            createNode("Dirty", dirtyTags.contains(tags[i]));
            writer->writeEndElement();
        }
    }
}



void ExportData::writeNotebooks() {
    QList<qint32> lids;
    NotebookTable table(global.db);
    QList<qint32> dirtyLids;
    table.getAllDirty(dirtyLids);
    table.getAll(lids);

    progress->setMaximum(lids.size());
    progress->setLabelText(tr("Notebooks"));
    progress->setValue(0);
    progress->show();

    for (int i=0; i<lids.size() && !quitNow; i++) {
        Notebook book;
        progress->setValue(i+1);
        QCoreApplication::processEvents();

        if (table.get(book, lids[i])) {
            writer->writeStartElement("Notebook");
            if (book.__isset.guid)
                createNode("Guid", book.guid);
            if (book.__isset.name)
                createNode("Name", book.name);
            if (book.__isset.updateSequenceNum)
                createNode("UpdateSequenceNumber",book.updateSequenceNum);
            if (book.__isset.defaultNotebook)
                createNode("DefaultNotebook", book.defaultNotebook);
            if (book.__isset.published)
                createNode("Published", book.published);
            if (book.__isset.serviceCreated)
                createTimestampNode("ServiceCreated", book.serviceCreated);
            if (book.__isset.serviceUpdated)
                createTimestampNode("ServiceUpdated", book.serviceUpdated);
            if (book.__isset.updateSequenceNum)
                createNode("UpdateSequenceNumber", book.updateSequenceNum);
            createNode("Dirty",dirtyLids.contains(lids[i]));

            if (book.__isset.businessNotebook) {
                writer->writeStartElement("BusinessNotebook");
                if (book.businessNotebook.__isset.notebookDescription)
                    createNode("NotebookDescription", book.businessNotebook.notebookDescription);
                if (book.businessNotebook.__isset.privilege)
                    createNode("NotebookDescription", book.businessNotebook.privilege);
                if (book.businessNotebook.__isset.recommended)
                    createNode("NotebookDescription", book.businessNotebook.recommended);
                writer->writeEndElement();
            }

            if (book.__isset.publishing) {
                writer->writeStartElement("Publishing");
                if (book.publishing.__isset.uri)
                    createNode("Uri", book.publishing.uri);
                if (book.publishing.__isset.publicDescription)
                    createNode("PublicDescription", book.publishing.publicDescription);
                if (book.publishing.__isset.ascending)
                    createNode("Ascending", book.publishing.ascending);
                if (book.publishing.__isset.order)
                    createNode("Uri", QString::number(book.publishing.order));
                writer->writeEndElement();
            }

            if (book.__isset.sharedNotebookIds && book.sharedNotebookIds.size() > 0) {
                writer->writeStartElement("SharedNotebookIds");
                for (unsigned int i=0; i<book.sharedNotebookIds.size(); i++) {
                    createNode("Id", QString::number(book.sharedNotebookIds.at(i)));
                }
                writer->writeEndElement();
            }
            if (book.__isset.contact) {
                writeUser(book.contact);
            }
            writer->writeEndElement();
        }
    }
}



void ExportData::writeUser(User user) {
    writer->writeStartElement("User");
    if (user.__isset.id)
        createNode("Id", user.id);
    if (user.__isset.username)
        createNode("Username", user.username);
    if (user.__isset.email)
        createNode("Username", user.email);
    if (user.__isset.name)
        createNode("Name", user.name);
    if (user.__isset.timezone)
        createNode("Timezone", user.timezone);
    if (user.__isset.privilege)
        createNode("Privilege", user.privilege);
    if (user.__isset.created)
        createTimestampNode("Created", user.created);
    if (user.__isset.updated)
        createTimestampNode("Created", user.updated);
    if (user.__isset.deleted)
        createTimestampNode("Deleted", user.deleted);
    if (user.__isset.active)
        createNode("Active", user.active);
    if (user.__isset.shardId)
        createNode("Timezone", user.shardId);
    if (user.__isset.attributes) {
        writer->writeStartElement("UserAttributes");
        if (user.attributes.__isset.defaultLocationName)
            createNode("DefaultLocationName", user.attributes.defaultLocationName);
        if (user.attributes.__isset.defaultLatitude)
            createNode("DefaultLatitude", user.attributes.defaultLatitude);
        if (user.attributes.__isset.defaultLongitude)
            createNode("DefaultLongitude", user.attributes.defaultLongitude);
        if (user.attributes.__isset.preactivation)
            createNode("Preactivation", user.attributes.preactivation);
        if (user.attributes.__isset.incomingEmailAddress)
            createNode("IncomingEmailAddress", user.attributes.incomingEmailAddress);
        if (user.attributes.__isset.comments)
            createNode("comments", user.attributes.comments);
        if (user.attributes.__isset.dateAgreedToTermsOfService)
            createTimestampNode("DateAgreedToTermsOfService", user.attributes.dateAgreedToTermsOfService);
        if (user.attributes.__isset.maxReferrals)
            createNode("MaxReferrals", user.attributes.maxReferrals);
        if (user.attributes.__isset.referralCount)
            createNode("ReferralCount", user.attributes.referralCount);
        if (user.attributes.__isset.refererCode)
            createNode("ReferrerCode", user.attributes.refererCode);
        if (user.attributes.__isset.sentEmailDate)
            createTimestampNode("SentEmailDate", user.attributes.sentEmailDate);
        if (user.attributes.__isset.sentEmailCount)
            createNode("SentEmailCount", user.attributes.sentEmailCount);
        if (user.attributes.__isset.dailyEmailLimit)
            createNode("DailyEmailLimit", user.attributes.dailyEmailLimit);
        if (user.attributes.__isset.emailOptOutDate)
            createTimestampNode("EmailOptOutDate", user.attributes.emailOptOutDate);
        if (user.attributes.__isset.partnerEmailOptInDate)
            createTimestampNode("PartnerEmailOptInDate", user.attributes.partnerEmailOptInDate);
        if (user.attributes.__isset.preferredCountry)
            createNode("PreferredCountry", user.attributes.preferredCountry);
        if (user.attributes.__isset.preferredLanguage)
            createNode("PreferredLanguage", user.attributes.preferredLanguage);
        if (user.attributes.__isset.clipFullPage)
            createNode("ClipFullPage", user.attributes.clipFullPage);
        if (user.attributes.__isset.twitterUserName)
            createNode("TwitterUserName", user.attributes.twitterUserName);
        if (user.attributes.__isset.twitterId)
            createNode("TwitterId", user.attributes.twitterId);
        if (user.attributes.__isset.groupName)
            createNode("GroupName", user.attributes.groupName);
        if (user.attributes.__isset.recognitionLanguage)
            createNode("recognitionLanguage", user.attributes.recognitionLanguage);
        if (user.attributes.__isset.referralProof)
            createNode("ReferralProof", user.attributes.referralProof);
        if (user.attributes.__isset.educationalDiscount)
            createNode("EducationalDiscount", user.attributes.educationalDiscount);
        if (user.attributes.__isset.businessAddress)
            createNode("BusinessAddress", user.attributes.businessAddress);
        if (user.attributes.__isset.hideSponsorBilling)
            createNode("HideSponsorBilling", user.attributes.hideSponsorBilling);
        if (user.attributes.__isset.taxExempt)
            createNode("TaxExempt", user.attributes.taxExempt);
        if (user.attributes.__isset.useEmailAutoFiling)
            createNode("UseEmailAutoFiling", user.attributes.useEmailAutoFiling);
        if (user.attributes.__isset.reminderEmailConfig)
            createNode("ReminderEmailConfig", user.attributes.reminderEmailConfig);
        writer->writeEndElement();
    }
    if (user.__isset.premiumInfo) {
        writer->writeStartElement("PremiumInfo");
        if (user.premiumInfo.__isset.premiumExpirationDate)
            createTimestampNode("PremiumExpirationDate",user.premiumInfo.premiumExpirationDate);
        if (user.premiumInfo.__isset.premiumUpgradable)
            createNode("PremiumUpgradable", user.premiumInfo.premiumUpgradable);
        if (user.premiumInfo.__isset.sponsoredGroupName)
            createNode("SponsorGroupName", user.premiumInfo.sponsoredGroupName);
        if (user.premiumInfo.__isset.sponsoredGroupRole)
            createNode("SponsorGroupRole", user.premiumInfo.sponsoredGroupRole);
        writer->writeEndElement();
    }
    if (user.__isset.businessUserInfo) {
        writer->writeStartElement("BusinessUserInfo");
        if (user.businessUserInfo.__isset.businessId)
            createNode("BusenessId", user.businessUserInfo.businessId);
        if (user.businessUserInfo.__isset.businessName)
            createNode("BusinessName", user.businessUserInfo.businessName);
        if (user.businessUserInfo.__isset.email)
            createNode("Email", user.businessUserInfo.email);
        if (user.businessUserInfo.__isset.role)
            createNode("Role", user.businessUserInfo.role);
        writer->writeEndElement();

    }
    writer->writeEndElement();
}


void ExportData::writeData(QString name, Data data) {
    writer->writeStartElement(name);
    if (data.__isset.body)
        createBinaryNode("Body", data.body);
    if (data.__isset.bodyHash) {
        createBinaryNode("BodyHash", data.bodyHash);
        QByteArray ba;
        ba.append(data.bodyHash.c_str(), data.bodyHash.size());
        QLOG_DEBUG() << ba.toHex();
        QByteArray b2 = QByteArray::fromHex(ba);
    }
    if (data.__isset.size)
        createNode("Size", data.size);
    writer->writeEndElement();
}

void ExportData::writeSavedSearches() {
    QList<qint32> lids;
    SearchTable table(global.db);
    QList<qint32> dirtyLids;
    table.getAllDirty(dirtyLids);
    table.getAll(lids);
    progress->setMaximum(lids.size());
    progress->setLabelText(tr("Searches"));
    progress->setValue(0);
    progress->show();


    for (int i=0; i<lids.size() && !quitNow; i++) {
        SavedSearch s;
        progress->setValue(i+1);
        QCoreApplication::processEvents();
        table.get(s,lids[i]);
        writer->writeStartElement("SavedSearch");
        if (s.__isset.guid)
            createNode("Guid", s.guid);
        if (s.__isset.name)
            createNode("Name", s.name);
        if (s.__isset.query)
            createNode("Query", s.query);
        if (s.__isset.format)
            createNode("Format", s.format);
        if (s.__isset.updateSequenceNum)
            createNode("UpdateSequenceNumber", s.updateSequenceNum);
        if (s.__isset.scope) {
            writer->writeStartElement("Scope");
            if (s.scope.__isset.includeAccount)
                createNode("IncludeAccount", s.scope.includeAccount);
            if (s.scope.__isset.includeBusinessLinkedNotebooks)
                createNode("IncludeBusinessLinkedNotebooks", s.scope.includeBusinessLinkedNotebooks);
            if (s.scope.__isset.includePersonalLinkedNotebooks)
                createNode("IncludePersonalLinkedNotebooks", s.scope.includePersonalLinkedNotebooks);
            writer->writeEndElement();
        }
        writer->writeEndElement();
    }
}


void ExportData::writeLinkedNotebooks() {
    QList<qint32> lids;
    LinkedNotebookTable table(global.db);
    table.getAll(lids);

    progress->setMaximum(lids.size());
    progress->setLabelText(tr("Linked Notebooks"));
    progress->setValue(0);
    progress->show();

    for (int i=0; i<lids.size() && !quitNow; i++) {
        progress->setValue(i+1);
        QCoreApplication::processEvents();
        LinkedNotebook s;
        table.get(s,lids[i]);
        writer->writeStartElement("LinkedNotebook");
        if (s.__isset.guid)
            createNode("Guid", s.guid);
        if (s.__isset.businessId)
            createNode("BusinessId", s.businessId);
        if (s.__isset.shardId)
            createNode("ShardId", s.shardId);
        if (s.__isset.shareKey)
            createNode("ShareKey", s.shareKey);
        if (s.__isset.updateSequenceNum)
            createNode("UpdateSequenceNumber", s.updateSequenceNum);
        if (s.__isset.shareName)
            createNode("ShareName", s.shareName);
        if (s.__isset.stack)
            createNode("Stack", s.stack);
        if (s.__isset.uri)
            createNode("Uri", s.uri);
        if (s.__isset.username)
            createNode("UserName", s.username);
        writer->writeEndElement();
    }
}



void ExportData::writeSharedNotebooks() {
    QList<qint32> lids;
    SharedNotebookTable table(global.db);
    table.getAll(lids);

    progress->setMaximum(lids.size());
    progress->setLabelText(tr("Shared Notebooks"));
    progress->setValue(0);
    progress->show();

    for (int i=0; i<lids.size() && !quitNow; i++) {
        progress->setValue(i+1);
        QCoreApplication::processEvents();
        SharedNotebook s;
        table.get(s,lids[i]);
        writer->writeStartElement("SharedNotebook");
        if (s.__isset.allowPreview)
            createNode("AllowPreview", s.allowPreview);
        if (s.__isset.email)
            createNode("Email", s.email);
        if (s.__isset.id)
            createNode("Id", s.id);
        if (s.__isset.notebookGuid)
            createNode("NotebookGuid", s.notebookGuid);
        if (s.__isset.notebookModifiable)
            createNode("NotebookModifiable", s.notebookModifiable);
        if (s.__isset.privilege)
            createNode("Privilege", s.privilege);
        if (s.__isset.recipientSettings) {
            writer->writeStartElement("RecipientSettings");
            if (s.recipientSettings.__isset.reminderNotifyEmail)
                createNode("ReminderNotifyEmail", s.recipientSettings.reminderNotifyEmail);
            if (s.recipientSettings.__isset.reminderNotifyInApp)
                createNode("ReminderNotifyInApp", s.recipientSettings.reminderNotifyInApp);
            writer->writeEndElement();
        }
        if (s.__isset.username)
            createNode("Username", s.username);
        if (s.__isset.username)
            createNode("UserId", s.userId);
        if (s.__isset.requireLogin)
            createNode("RequireLogin", s.requireLogin);
        if (s.__isset.serviceCreated)
            createTimestampNode("ServiceCreated", s.serviceCreated);
        if (s.__isset.serviceUpdated)
            createTimestampNode("ServiceUpdated", s.serviceUpdated);
        writer->writeEndElement();
    }

}



void ExportData::writeNotes() {
    NoteTable table(global.db);
    QList<qint32> dirtyLids;
    table.getAllDirty(dirtyLids);
    progress->setMaximum(lids.size());
    progress->setLabelText(tr("Notes"));
    progress->setValue(0);
    QCoreApplication::processEvents();

    for (int i=0; i<lids.size() && !quitNow; i++) {
        progress->setValue(i+1);
        QCoreApplication::processEvents();
        Note n;
        table.get(n,lids[i], true,true);
        writer->writeStartElement("Note");
        if (n.__isset.guid)
            createNode("Guid", n.guid);
        if (n.__isset.title)
            createNode("Title", n.title);
        if (n.__isset.content) {
            writer->writeStartElement("Content");
            writer->writeCDATA(QString::fromStdString(n.content));
            writer->writeEndElement();
        }
        if (n.__isset.contentHash)
            createBinaryNode("ContentHash", n.contentHash);
        if (n.__isset.contentLength)
            createNode("ContentLength", n.contentLength);
        if (n.__isset.created)
            createTimestampNode("Created", n.created);
        if (n.__isset.updated)
            createTimestampNode("Updated", n.updated);
        if (n.__isset.deleted)
            createTimestampNode("Deleted", n.deleted);
        if (n.__isset.active)
            createNode("Active", n.active);
        if (n.__isset.updateSequenceNum)
            createNode("UpdateSequenceNumber", n.updateSequenceNum);
        if (n.__isset.notebookGuid)
            createNode("NotebookGuid", n.notebookGuid);
        if (n.__isset.tagGuids && n.__isset.tagNames && n.tagNames.size() == n.tagGuids.size()) {
            for (unsigned int j=0; j<n.tagGuids.size(); j++) {
                writer->writeStartElement("Tag");
                createNode("Guid", n.tagGuids[j]);
                createNode("Name", n.tagNames[j]);
                writer->writeEndElement();
            }

        }
        if (n.__isset.resources) {
            for (unsigned int j=0; j<n.resources.size(); j++) {
                writeResource(n.resources[j]);
            }
        }
        if (n.__isset.attributes) {
            writer->writeStartElement("Attributes");
            if (n.attributes.__isset.subjectDate)
                createTimestampNode("SubjectDate", n.attributes.subjectDate);
            if (n.attributes.__isset.latitude)
                createNode("Latitude", n.attributes.latitude);
            if (n.attributes.__isset.longitude)
                createNode("Longitude",n.attributes.longitude);
            if (n.attributes.__isset.altitude)
                createNode("Altitude", n.attributes.altitude);
            if (n.attributes.__isset.author)
                createNode("Author", n.attributes.author);
            if (n.attributes.__isset.source)
                createNode("Source", n.attributes.source);
            if (n.attributes.__isset.sourceApplication)
                createNode("SourceApplication", n.attributes.sourceApplication);
            if (n.attributes.__isset.sourceURL)
                createNode("SourceUrl", n.attributes.sourceURL);
            if (n.attributes.__isset.shareDate)
                createTimestampNode("ShareDate", n.attributes.shareDate);
            if (n.attributes.__isset.reminderOrder)
                createNode("ReminderOrder",n.attributes.reminderOrder);
            if (n.attributes.__isset.reminderDoneTime)
                createNode("ReminderDoneTime", n.attributes.reminderDoneTime);
            if (n.attributes.__isset.reminderTime)
                createNode("ReminderTime", n.attributes.reminderTime);
            if (n.attributes.__isset.placeName)
                createNode("PlaceName", n.attributes.placeName);
            if (n.attributes.__isset.contentClass)
                createNode("ContentClass",n.attributes.contentClass);
            if (n.attributes.__isset.lastEditedBy)
                createNode("LastEditedBy", n.attributes.lastEditedBy);
            if (n.attributes.__isset.creatorId)
                createNode("CreatorId", n.attributes.creatorId);
            if (n.attributes.__isset.lastEditorId)
                createNode("LastEditorId", n.attributes.lastEditorId);
            writer->writeEndElement();
        }
        createNode("Dirty", dirtyLids.contains(lids[i]));
        writer->writeEndElement();
    }
}



void ExportData::writeResource(Resource r) {
    writer->writeStartElement("NoteResource");
    if (r.__isset.guid)
        createNode("Guid", r.guid);
    if (r.__isset.noteGuid)
        createNode("NoteGuid", r.noteGuid);
    if (r.__isset.data)
        writeData("Data", r.data);
    if (r.__isset.mime)
        createNode("Mime", r.mime);
    if (r.__isset.width)
        createNode("Width", r.width);
    if (r.__isset.height)
        createNode("Height",r.height);
    if (r.__isset.duration)
        createNode("Duration", r.duration);
    if (r.__isset.active)
        createNode("Active", r.active);
    if (r.__isset.recognition)  {
        writeData("Recognition", r.recognition);
    }
    if (r.__isset.attributes) {
        writer->writeStartElement("ResourceAttributes");
        if (r.attributes.__isset.sourceURL)
            createNode("SourceUrl", r.attributes.sourceURL);
        if (r.attributes.__isset.timestamp)
            createNode("Timestamp", r.attributes.timestamp);
        if (r.attributes.__isset.latitude)
            createNode("Latitude", r.attributes.latitude);
        if (r.attributes.__isset.longitude)
            createNode("Longitude", r.attributes.longitude);
        if (r.attributes.__isset.altitude)
            createNode("Altitude", r.attributes.altitude);
        if (r.attributes.__isset.cameraMake)
            createNode("CameraMake", r.attributes.cameraMake);
        if (r.attributes.__isset.cameraModel)
            createNode("CameraModel", r.attributes.cameraModel);
        if (r.attributes.__isset.recoType)
            createNode("RecoType", r.attributes.recoType);
        if (r.attributes.__isset.fileName)
            createNode("FileName", r.attributes.fileName);
        if (r.attributes.__isset.attachment)
            createNode("Attachment", r.attributes.attachment);
        writer->writeEndElement();
    }
    if (r.__isset.updateSequenceNum)
        createNode("UpdateSequenceNumber", r.updateSequenceNum);
    if (r.__isset.alternateData)
        writeData("AlternateData", r.alternateData);
    writer->writeEndElement();
}


void ExportData::createNode(QString nodeName, QString value) {
    writer->writeTextElement(nodeName, value);
    return;
}






void ExportData::createNode(QString nodeName, bool value) {
    if (value)
        writer->writeTextElement(nodeName, "true");
    else
        writer->writeTextElement(nodeName, "false");
    return;
}





void ExportData::createNode(QString nodeName, QBool value) {
    if (value)
        writer->writeTextElement(nodeName, "true");
    else
        writer->writeTextElement(nodeName, "false");
    return;
}




void ExportData::createBinaryNode(QString nodeName, string value) {
    QByteArray ba;
    ba.append(value.data(), value.length());
    QString sa;
    sa.append(ba.toHex());
    createNode(nodeName, sa);
    return;
}

void ExportData::createNode(QString nodename, string value) {
    createNode(nodename, QString::fromStdString(value));
}


void ExportData::createLongLongNode(QString nodename, qlonglong value) {
    createNode(nodename, QString::number(value));
}



void ExportData::createNode(QString nodename, qint32 value) {
    createNode(nodename, QString::number(value));
}



void ExportData::createTimestampNode(QString nodename, Timestamp value) {
    createNode(nodename, QString::number(value));
}


void ExportData::createNode(QString nodename, double value) {
    createNode(nodename, QString::number(value));
}


void ExportData::createNode(QString nodename, int64_t value) {
    createNode(nodename, QString::number(value));
}


void ExportData::abortBackup() {
    quitNow = true;
    this->errorMessage = tr("Backup Canceled");
    this->lastError = 1;
    progress->close();
}

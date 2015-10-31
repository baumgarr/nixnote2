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
        writeLinkedNotebooks();
        writeSharedNotebooks();
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
            if (tag.guid.isSet())
                createNode("Guid", tag.guid);
            if (tag.name.isSet())
                createNode("Name", tag.name);
            if (tag.parentGuid.isSet())
                createNode("ParentGuid", tag.parentGuid);
            if (tag.updateSequenceNum.isSet())
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
            if (book.guid.isSet())
                createNode("Guid", book.guid);
            if (book.name.isSet())
                createNode("Name", book.name);
            if (book.updateSequenceNum.isSet())
                createNode("UpdateSequenceNumber",book.updateSequenceNum);
            if (book.defaultNotebook.isSet())
                createNode("DefaultNotebook", book.defaultNotebook);
            if (book.published.isSet())
                createNode("Published", book.published);
            if (book.serviceCreated.isSet())
                createTimestampNode("ServiceCreated", book.serviceCreated);
            if (book.serviceUpdated.isSet())
                createTimestampNode("ServiceUpdated", book.serviceUpdated);
            if (book.updateSequenceNum.isSet())
                createNode("UpdateSequenceNumber", book.updateSequenceNum);
            createNode("Dirty",dirtyLids.contains(lids[i]));

            if (book.businessNotebook.isSet()) {
                writer->writeStartElement("BusinessNotebook");
                if (book.businessNotebook.value().notebookDescription.isSet())
                    createNode("NotebookDescription", book.businessNotebook.value().notebookDescription);
                if (book.businessNotebook.value().privilege.isSet())
                    createNode("NotebookDescription", book.businessNotebook.value().privilege);
                if (book.businessNotebook.value().recommended.isSet())
                    createNode("NotebookDescription", book.businessNotebook.value().recommended);
                writer->writeEndElement();
            }

            if (book.publishing.isSet()) {
                writer->writeStartElement("Publishing");
                if (book.publishing.value().uri.isSet())
                    createNode("Uri", book.publishing.value().uri);
                if (book.publishing.value().publicDescription.isSet())
                    createNode("PublicDescription", book.publishing.value().publicDescription);
                if (book.publishing.value().ascending.isSet())
                    createNode("Ascending", book.publishing.value().ascending);
                if (book.publishing.value().order.isSet())
                    createNode("Uri", QString::number(book.publishing.value().order));
                writer->writeEndElement();
            }

            if (book.sharedNotebookIds.isSet() && book.sharedNotebookIds.value().size() > 0) {
                writer->writeStartElement("SharedNotebookIds");
                for (int i=0; i<book.sharedNotebookIds.value().size(); i++) {
                    createNode("Id", QString::number(book.sharedNotebookIds.value().at(i)));
                }
                writer->writeEndElement();
            }
            if (book.contact.isSet()) {
                writeUser(book.contact);
            }
            writer->writeEndElement();
        }
    }
}



void ExportData::writeUser(User user) {
    writer->writeStartElement("User");
    if (user.id.isSet())
        createNode("Id", user.id);
    if (user.username.isSet())
        createNode("Username", user.username);
    if (user.email.isSet())
        createNode("Username", user.email);
    if (user.name.isSet())
        createNode("Name", user.name);
    if (user.timezone.isSet())
        createNode("Timezone", user.timezone);
    if (user.privilege.isSet())
        createNode("Privilege", user.privilege);
    if (user.created.isSet())
        createTimestampNode("Created", user.created);
    if (user.updated.isSet())
        createTimestampNode("Created", user.updated);
    if (user.deleted.isSet())
        createTimestampNode("Deleted", user.deleted);
    if (user.active.isSet())
        createNode("Active", user.active);
    if (user.shardId.isSet())
        createNode("Timezone", user.shardId);
    if (user.attributes.isSet()) {
        writer->writeStartElement("UserAttributes");
        if (user.attributes.value().defaultLocationName.isSet())
            createNode("DefaultLocationName", user.attributes.value().defaultLocationName);
        if (user.attributes.value().defaultLatitude.isSet())
            createNode("DefaultLatitude", user.attributes.value().defaultLatitude);
        if (user.attributes.value().defaultLongitude.isSet())
            createNode("DefaultLongitude", user.attributes.value().defaultLongitude);
        if (user.attributes.value().preactivation.isSet())
            createNode("Preactivation", user.attributes.value().preactivation);
        if (user.attributes.value().incomingEmailAddress.isSet())
            createNode("IncomingEmailAddress", user.attributes.value().incomingEmailAddress);
        if (user.attributes.value().comments.isSet())
            createNode("comments", user.attributes.value().comments);
        if (user.attributes.value().dateAgreedToTermsOfService.isSet())
            createTimestampNode("DateAgreedToTermsOfService", user.attributes.value().dateAgreedToTermsOfService);
        if (user.attributes.value().maxReferrals.isSet())
            createNode("MaxReferrals", user.attributes.value().maxReferrals);
        if (user.attributes.value().referralCount.isSet())
            createNode("ReferralCount", user.attributes.value().referralCount);
        if (user.attributes.value().refererCode.isSet())
            createNode("ReferrerCode", user.attributes.value().refererCode);
        if (user.attributes.value().sentEmailDate.isSet())
            createTimestampNode("SentEmailDate", user.attributes.value().sentEmailDate);
        if (user.attributes.value().sentEmailCount.isSet())
            createNode("SentEmailCount", user.attributes.value().sentEmailCount);
        if (user.attributes.value().dailyEmailLimit.isSet())
            createNode("DailyEmailLimit", user.attributes.value().dailyEmailLimit);
        if (user.attributes.value().emailOptOutDate.isSet())
            createTimestampNode("EmailOptOutDate", user.attributes.value().emailOptOutDate);
        if (user.attributes.value().partnerEmailOptInDate.isSet())
            createTimestampNode("PartnerEmailOptInDate", user.attributes.value().partnerEmailOptInDate);
        if (user.attributes.value().preferredCountry.isSet())
            createNode("PreferredCountry", user.attributes.value().preferredCountry);
        if (user.attributes.value().preferredLanguage.isSet())
            createNode("PreferredLanguage", user.attributes.value().preferredLanguage);
        if (user.attributes.value().clipFullPage.isSet())
            createNode("ClipFullPage", user.attributes.value().clipFullPage);
        if (user.attributes.value().twitterUserName.isSet())
            createNode("TwitterUserName", user.attributes.value().twitterUserName);
        if (user.attributes.value().twitterId.isSet())
            createNode("TwitterId", user.attributes.value().twitterId);
        if (user.attributes.value().groupName.isSet())
            createNode("GroupName", user.attributes.value().groupName);
        if (user.attributes.value().recognitionLanguage.isSet())
            createNode("recognitionLanguage", user.attributes.value().recognitionLanguage);
        if (user.attributes.value().referralProof.isSet())
            createNode("ReferralProof", user.attributes.value().referralProof);
        if (user.attributes.value().educationalDiscount.isSet())
            createNode("EducationalDiscount", user.attributes.value().educationalDiscount);
        if (user.attributes.value().businessAddress.isSet())
            createNode("BusinessAddress", user.attributes.value().businessAddress);
        if (user.attributes.value().hideSponsorBilling.isSet())
            createNode("HideSponsorBilling", user.attributes.value().hideSponsorBilling);
        if (user.attributes.value().taxExempt.isSet())
            createNode("TaxExempt", user.attributes.value().taxExempt);
        if (user.attributes.value().useEmailAutoFiling.isSet())
            createNode("UseEmailAutoFiling", user.attributes.value().useEmailAutoFiling);
        if (user.attributes.value().reminderEmailConfig.isSet())
            createNode("ReminderEmailConfig", user.attributes.value().reminderEmailConfig);
        writer->writeEndElement();
    }
    if (user.premiumInfo.isSet()) {
        writer->writeStartElement("PremiumInfo");
        if (user.premiumInfo.value().premiumExpirationDate.isSet())
            createTimestampNode("PremiumExpirationDate",user.premiumInfo.value().premiumExpirationDate);
        if (user.premiumInfo.value().premiumUpgradable.isSet())
            createNode("PremiumUpgradable", user.premiumInfo.value().premiumUpgradable);
        if (user.premiumInfo.value().sponsoredGroupName.isSet())
            createNode("SponsorGroupName", user.premiumInfo.value().sponsoredGroupName);
        if (user.premiumInfo.value().sponsoredGroupRole.isSet())
            createNode("SponsorGroupRole", user.premiumInfo.value().sponsoredGroupRole);
        writer->writeEndElement();
    }
    if (user.businessUserInfo.isSet()) {
        writer->writeStartElement("BusinessUserInfo");
        if (user.businessUserInfo.value().businessId.isSet())
            createNode("BusenessId", user.businessUserInfo.value().businessId);
        if (user.businessUserInfo.value().businessName.isSet())
            createNode("BusinessName", user.businessUserInfo.value().businessName);
        if (user.businessUserInfo.value().email.isSet())
            createNode("Email", user.businessUserInfo.value().email);
        if (user.businessUserInfo.value().role.isSet())
            createNode("Role", user.businessUserInfo.value().role);
        writer->writeEndElement();

    }
    writer->writeEndElement();
}


void ExportData::writeData(QString name, Data data) {
    writer->writeStartElement(name);
    if (data.body.isSet())
        createNode("Body", data.body.value());
    if (data.bodyHash.isSet()) {
        createNode("BodyHash", data.bodyHash);
        QByteArray ba;
        ba.append(data.bodyHash);
    }
    if (data.size.isSet())
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
        if (s.guid.isSet())
            createNode("Guid", s.guid);
        if (s.name.isSet())
            createNode("Name", s.name);
        if (s.query.isSet())
            createNode("Query", s.query);
        if (s.format.isSet())
            createNode("Format", s.format);
        if (s.updateSequenceNum.isSet())
            createNode("UpdateSequenceNumber", s.updateSequenceNum);
        if (s.scope.isSet()) {
            writer->writeStartElement("Scope");
            if (s.scope.value().includeAccount.isSet())
                createNode("IncludeAccount", s.scope.value().includeAccount);
            if (s.scope.value().includeBusinessLinkedNotebooks.isSet())
                createNode("IncludeBusinessLinkedNotebooks", s.scope.value().includeBusinessLinkedNotebooks);
            if (s.scope.value().includePersonalLinkedNotebooks.isSet())
                createNode("IncludePersonalLinkedNotebooks", s.scope.value().includePersonalLinkedNotebooks);
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
        if (s.guid.isSet())
            createNode("Guid", s.guid);
        if (s.businessId.isSet())
            createNode("BusinessId", s.businessId);
        if (s.shardId.isSet())
            createNode("ShardId", s.shardId);
        if (s.shareKey.isSet())
            createNode("ShareKey", s.shareKey);
        if (s.updateSequenceNum.isSet())
            createNode("UpdateSequenceNumber", s.updateSequenceNum);
        if (s.shareName.isSet())
            createNode("ShareName", s.shareName);
        if (s.stack.isSet())
            createNode("Stack", s.stack);
        if (s.uri.isSet())
            createNode("Uri", s.uri);
        if (s.username.isSet())
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
        QStringList users;
        table.getShareUsers(users, lids[i]);
        for (int j=0; j<users.size(); j++) {

            table.get(s,lids[i], users[j]);
            writer->writeStartElement("SharedNotebook");
            if (s.allowPreview.isSet())
                createNode("AllowPreview", s.allowPreview);
            if (s.email.isSet())
                createNode("Email", s.email);
            if (s.id.isSet())
                createNode("Id", QString::number(s.id));
            if (s.notebookGuid.isSet())
                createNode("NotebookGuid", s.notebookGuid);
            if (s.notebookModifiable.isSet())
                createNode("NotebookModifiable", s.notebookModifiable);
            if (s.privilege.isSet())
                createNode("Privilege", s.privilege);
            if (s.recipientSettings.isSet()) {
                writer->writeStartElement("RecipientSettings");
                if (s.recipientSettings.value().reminderNotifyEmail.isSet())
                    createNode("ReminderNotifyEmail", s.recipientSettings.value().reminderNotifyEmail);
                if (s.recipientSettings.value().reminderNotifyInApp.isSet())
                    createNode("ReminderNotifyInApp", s.recipientSettings.value().reminderNotifyInApp);
                writer->writeEndElement();
            }
            if (s.username.isSet())
                createNode("Username", s.username);
            if (s.userId.isSet())
                createNode("UserId", s.userId);
            if (s.requireLogin.isSet())
                createNode("RequireLogin", s.requireLogin);
            if (s.serviceCreated.isSet())
                createTimestampNode("ServiceCreated", s.serviceCreated);
            if (s.serviceUpdated.isSet())
                createTimestampNode("ServiceUpdated", s.serviceUpdated);
            writer->writeEndElement();
        }
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
        if (n.guid.isSet())
            createNode("Guid", n.guid);
        if (n.title.isSet())
            createNode("Title", n.title);
        if (n.content.isSet()) {
            writer->writeStartElement("Content");
            writer->writeCDATA(n.content);
            writer->writeEndElement();
        }
        if (n.contentHash.isSet())
            createNode("ContentHash", n.contentHash);
        if (n.contentLength.isSet())
            createNode("ContentLength", n.contentLength);
        if (n.created.isSet())
            createTimestampNode("Created", n.created);
        if (n.updated.isSet())
            createTimestampNode("Updated", n.updated);
        if (n.deleted.isSet())
            createTimestampNode("Deleted", n.deleted);
        if (n.active.isSet())
            createNode("Active", n.active);
        if (n.updateSequenceNum.isSet())
            createNode("UpdateSequenceNumber", n.updateSequenceNum);
        if (n.notebookGuid.isSet())
            createNode("NotebookGuid", n.notebookGuid);
        if (n.tagGuids.isSet() && n.tagNames.isSet() && n.tagNames.value().size() == n.tagGuids.value().size()) {
            for (int j=0; j<n.tagGuids.value().size(); j++) {
                writer->writeStartElement("Tag");
                createNode("Guid", n.tagGuids.value()[j]);
                createNode("Name", n.tagNames.value()[j]);
                writer->writeEndElement();
            }

        }
        if (n.resources.isSet()) {
            for (int j=0; j<n.resources.value().size(); j++) {
                writeResource(n.resources.value()[j]);
            }
        }
        if (n.attributes.isSet()) {
            writer->writeStartElement("Attributes");
            if (n.attributes.value().subjectDate.isSet())
                createTimestampNode("SubjectDate", n.attributes.value().subjectDate);
            if (n.attributes.value().latitude.isSet())
                createNode("Latitude", n.attributes.value().latitude);
            if (n.attributes.value().longitude.isSet())
                createNode("Longitude",n.attributes.value().longitude);
            if (n.attributes.value().altitude.isSet())
                createNode("Altitude", n.attributes.value().altitude);
            if (n.attributes.value().author.isSet())
                createNode("Author", n.attributes.value().author);
            if (n.attributes.value().source.isSet())
                createNode("Source", n.attributes.value().source);
            if (n.attributes.value().sourceApplication.isSet())
                createNode("SourceApplication", n.attributes.value().sourceApplication);
            if (n.attributes.value().sourceURL.isSet())
                createNode("SourceUrl", n.attributes.value().sourceURL);
            if (n.attributes.value().shareDate.isSet())
                createTimestampNode("ShareDate", n.attributes.value().shareDate);
            if (n.attributes.value().reminderOrder.isSet())
                createNode("ReminderOrder",QString::number(n.attributes.value().reminderOrder));
            if (n.attributes.value().reminderDoneTime.isSet())
                createNode("ReminderDoneTime", QString::number(n.attributes.value().reminderDoneTime));
            if (n.attributes.value().reminderTime.isSet())
                createNode("ReminderTime", QString::number(n.attributes.value().reminderTime));
            if (n.attributes.value().placeName.isSet())
                createNode("PlaceName", n.attributes.value().placeName);
            if (n.attributes.value().contentClass.isSet())
                createNode("ContentClass",n.attributes.value().contentClass);
            if (n.attributes.value().lastEditedBy.isSet())
                createNode("LastEditedBy", n.attributes.value().lastEditedBy);
            if (n.attributes.value().creatorId.isSet())
                createNode("CreatorId", n.attributes.value().creatorId);
            if (n.attributes.value().lastEditorId.isSet())
                createNode("LastEditorId", n.attributes.value().lastEditorId);
            writer->writeEndElement();
        }
        createNode("Dirty", dirtyLids.contains(lids[i]));
        writer->writeEndElement();
    }
}



void ExportData::writeResource(Resource r) {
    writer->writeStartElement("NoteResource");
    if (r.guid.isSet())
        createNode("Guid", r.guid);
    if (r.noteGuid.isSet())
        createNode("NoteGuid", r.noteGuid);
    if (r.data.isSet())
        writeData("Data", r.data);
    if (r.mime.isSet())
        createNode("Mime", r.mime);
    if (r.width.isSet())
        createNode("Width", r.width);
    if (r.height.isSet())
        createNode("Height",r.height);
    if (r.duration.isSet())
        createNode("Duration", r.duration);
    if (r.active.isSet())
        createNode("Active", r.active);
    if (r.recognition.isSet())  {
        writeData("Recognition", r.recognition);
    }
    if (r.attributes.isSet()) {
        writer->writeStartElement("ResourceAttributes");
        if (r.attributes.value().sourceURL.isSet())
            createNode("SourceUrl", r.attributes.value().sourceURL);
        if (r.attributes.value().timestamp.isSet())
            createNode("Timestamp", QString::number(r.attributes.value().timestamp));
        if (r.attributes.value().latitude.isSet())
            createNode("Latitude", r.attributes.value().latitude);
        if (r.attributes.value().longitude.isSet())
            createNode("Longitude", r.attributes.value().longitude);
        if (r.attributes.value().altitude.isSet())
            createNode("Altitude", r.attributes.value().altitude);
        if (r.attributes.value().cameraMake.isSet())
            createNode("CameraMake", r.attributes.value().cameraMake);
        if (r.attributes.value().cameraModel.isSet())
            createNode("CameraModel", r.attributes.value().cameraModel);
        if (r.attributes.value().recoType.isSet())
            createNode("RecoType", r.attributes.value().recoType);
        if (r.attributes.value().fileName.isSet())
            createNode("FileName", r.attributes.value().fileName);
        if (r.attributes.value().attachment.isSet())
            createNode("Attachment", r.attributes.value().attachment);
        writer->writeEndElement();
    }
    if (r.updateSequenceNum.isSet())
        createNode("UpdateSequenceNumber", r.updateSequenceNum);
    if (r.alternateData.isSet())
        writeData("AlternateData", r.alternateData);
    writer->writeEndElement();
}


void ExportData::createNode(QString nodeName, QString value) {
    writer->writeTextElement(nodeName, value);
    return;
}



void ExportData::createNode(QString nodeName, QByteArray value) {
    writer->writeTextElement(nodeName, value.toHex());
    return;
}



void ExportData::createNode(QString nodeName, bool value) {
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

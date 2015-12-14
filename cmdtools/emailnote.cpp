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


#include "emailnote.h"
#include <QtXml>
#include <QString>
#include "global.h"
#include "email/mimehtml.h"
#include "utilities/mimereference.h"
#include "email/mimeinlinefile.h"
#include "sql/resourcetable.h"
#include "sql/notetable.h"
#include "html/noteformatter.h"

extern Global global;


EmailNote::EmailNote(QObject *parent) :
    QObject(parent)
{
    ccSelf = false;
}

QString EmailNote::wrap() {

    QString returnValue;
    QXmlStreamWriter *writer = new QXmlStreamWriter(&returnValue);
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");
    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE NixNote-Query>");
    writer->writeStartElement("nixnote-email");
    writer->writeAttribute("version", "2");
    writer->writeAttribute("application", "NixNote");
    writer->writeAttribute("applicationVersion", "2.x");
    writer->writeStartElement("EmailNote");
    writer->writeTextElement("id", QString::number(lid));
    writer->writeTextElement("subject", subject);
    writer->writeTextElement("To", to);
    writer->writeTextElement("Cc", cc);
    writer->writeTextElement("Bcc", bcc);
    writer->writeStartElement("Note");
    writer->writeCDATA(note);
    writer->writeEndElement();
    if (ccSelf)
        writer->writeTextElement("CcSelf", "Yes");
    else
        writer->writeTextElement("CcSelf", "No");
    writer->writeEndElement();
    writer->writeEndElement();
    writer->writeEndDocument();
    return returnValue;
}



void EmailNote::unwrap(QString data) {
    lastError = 0;
    QXmlStreamReader reader(data);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError()) {
            errorMessage = reader.errorString();
            QLOG_ERROR() << "************************* ERROR READING IMPORT " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader.name().toString().toLower() == "emailnote" && !reader.isEndElement()) {
            reader.readNext();
            while(reader.name().toString().toLower() != "emailnote") {
                if (reader.name().toString().toLower() == "to" && reader.isStartElement()) {
                    reader.readNext();
                    to = reader.text().toString();
                } else if (reader.name().toString().toLower() == "cc" && reader.isStartElement()) {
                    reader.readNext();
                    cc = reader.text().toString();
                } else if (reader.name().toString().toLower() == "id" && reader.isStartElement()) {
                    reader.readNext();
                    lid = reader.text().toString().toInt();
                } else if (reader.name().toString().toLower() == "subject" && reader.isStartElement()) {
                    reader.readNext();
                    subject = reader.text().toString();
                } else if (reader.name().toString().toLower() == "bcc" && reader.isStartElement()) {
                    reader.readNext();
                    bcc = reader.text().toString();
                } else if (reader.name().toString().toLower() == "note" && reader.isStartElement()) {
                    reader.readNext();
                    note = reader.text().toString();
                } else if (reader.name().toString().toLower() == "ccself" && reader.isStartElement()) {
                    reader.readNext();
                    QString value = reader.text().toString();
                    if (value.toLower()=="yes")
                        this->ccSelf=true;
                    else
                        this->ccSelf=false;
                } else
                    reader.readNext();
            }
        }
    }
}



QStringList EmailNote::tokenizeString(QString value) {
    QStringList values =  value.split(QRegExp(",|;|\\s+"), QString::SkipEmptyParts);

    // There is probably an easier way to do this with regular expressions, but
    // I am horrible at regular expressions.
    for (int i=0; i<values.size(); i++) {
        values[i] = values[i].trimmed();
    }
    return values;
}



// Prepare the email for sending.  This function scans through
// the email for images & attachments.  The resulting
// MimeMessage has all of the email contents.
void EmailNote::prepareEmailMessage(MimeMessage *message, QString note, QString body) {
    MimeHtml *text = new MimeHtml();

    // Prepare the massage the same as if we were printing it.
    QString contents = this->stripContentsForPrint(body);
    //QString textContents = editor->page()->currentFrame()->toPlainText();
    QStringList images;
    QStringList attachments;

    // Now, go thgough & reformat all the img tags.
    int cidCount=0;
    QLOG_DEBUG() << contents;
    int pos = contents.indexOf("src=\"/");
    while (pos>=0) {
        QString localFile = contents.mid(pos+5);
        int endPos = localFile.indexOf("\"");
        localFile = localFile.mid(0,endPos);
        images.append(localFile);
        endPos = pos+endPos;
        QString part1 = contents.mid(0,pos);
        QString part2 = contents.mid(endPos+14);
        cidCount++;
        contents = part1 + "src='cid:file" +QString::number(cidCount) +"'" + part2;

        pos = contents.indexOf("src=\"/", pos+5);
    }

    // next, look for all the attachments
    pos = contents.indexOf("href=\"nnres:");
    while (pos >0 -1) {
        QString localFile = contents.mid(pos+12);
        int endPos = localFile.indexOf("\"");
        localFile = localFile.mid(0,endPos);
        attachments.append(localFile);
        cidCount++;
        pos = contents.indexOf("href=\"nnres:", pos+5);
    }

    // If the user adds a note, then prepend it to the beginning.
    if (note.trimmed() != "") {
        int pos = contents.indexOf("<body");
        int endPos = contents.indexOf(">", pos);
        contents.insert(endPos+1,  Qt::escape(note)+"<p><p><hr><p>");
    }
    text->setHtml(contents);
    message->addPart(text);


    // Add all the images
    for (int i=0; i<images.size(); i++) {
        MimeReference mimeRef;
        QString localFile = images[i];
        QString mime = mimeRef.getMimeFromFileName(localFile);
        MimeInlineFile *file = new MimeInlineFile(new QFile(localFile));
        QString lidFile = localFile.mid(localFile.lastIndexOf(QDir::separator())+1);
        qint32 lid = lidFile.mid(0,lidFile.lastIndexOf(".")).toInt();
        ResourceTable rtable(global.db);
        Resource r;
        ResourceAttributes ra;
        if (rtable.get(r, lid, false) && r.attributes.isSet()) {
            ra = r.attributes;
            if (ra.fileName.isSet())
                file->setContentName(ra.fileName);
        }
        file->setContentId("file"+QString::number(i+1));
        file->setContentType(mime);
        message->addPart(file);
    }

    // Add all the attachments
    for (int i=0; i<attachments.size(); i++) {
        MimeReference mimeRef;
        QString localFile = attachments[i];
        QString mime = mimeRef.getMimeFromFileName(localFile);
        MimeInlineFile *file = new MimeInlineFile(new QFile(localFile));
        QString lidFile = localFile.mid(localFile.lastIndexOf(QDir::separator())+1);
        qint32 lid = lidFile.mid(0,lidFile.lastIndexOf(".")).toInt();
        ResourceTable rtable(global.db);
        Resource r;
        ResourceAttributes ra;
        if (rtable.get(r, lid, false) && r.attributes.isSet()) {
            ra = r.attributes;
            if (ra.fileName.isSet())
                file->setContentName(ra.fileName);
        }
        file->setContentType(mime);
        message->addPart(file);
    }
    return;
}



// Strip the contents from the current webview in preparation for printing.
QString EmailNote::stripContentsForPrint(QString contents) {
    int pos = contents.indexOf("<object");
    while (pos != -1) {
        int endPos = contents.indexOf(">", pos);
        QString lidString = contents.mid(contents.indexOf("lid=", pos)+5);
        lidString = lidString.mid(0,lidString.indexOf("\" "));
        contents = contents.mid(0,pos) + "<img src=\"file://" +
                global.fileManager.getTmpDirPath() + lidString +
                QString("-print.png\" width=\"10%\" height=\"10%\"></img>")+contents.mid(endPos+1);

        pos = contents.indexOf("<object", endPos);
    }
    return contents.replace("src=\"file:////", "src=\"/");
}



int EmailNote::sendEmail() {
    global.settings->beginGroup("Email");
    QString server = global.settings->value("smtpServer", "").toString();
    int port = global.settings->value("smtpPort", 25).toInt();
    QString smtpConnectionType = global.settings->value("smtpConnectionType", "TcpConnection").toString();
    QString userid = global.settings->value("userid", "").toString();
    QString password = global.settings->value("password", "").toString();
    QString senderEmail = global.settings->value("senderEmail", "").toString();
    QString senderName = global.settings->value("senderName", "").toString();
    global.settings->endGroup();

    if (senderEmail.trimmed() == "" || server.trimmed() == "") {
        errorMessage = tr("SMTP Server has not been setup.\nPlease specify server settings in the Preferences menu.");
        return 8;
    }

    QStringList toAddresses = tokenizeString(to);
    QStringList ccAddresses = tokenizeString(cc);
    QStringList bccAddresses = tokenizeString(bcc);

    if (senderName.trimmed() == "")
        senderName = senderEmail;

    SmtpClient::ConnectionType type = SmtpClient::TcpConnection;
    if (smtpConnectionType == "SslConnection")
        type = SmtpClient::SslConnection;
    if (smtpConnectionType == "TlsConnection")
        type = SmtpClient::TlsConnection;

    SmtpClient smtp(server, port, type);
    smtp.setResponseTimeout(-1);

    // We need to set the username (your email address) and password
    // for smtp authentification.
    smtp.setUser(userid);
    smtp.setPassword(password);

    // Now we create a MimeMessage object. This is the email.
    MimeMessage message;

    EmailAddress sender(senderEmail, senderName);
    message.setSender(&sender);

    for (int i=0; i<toAddresses.size(); i++) {
        EmailAddress *to = new EmailAddress(toAddresses[i], toAddresses[i]);
        message.addRecipient(to);
    }

    for (int i=0; i<ccAddresses.size(); i++) {
        EmailAddress *cc = new EmailAddress(ccAddresses[i], ccAddresses[i]);
        message.addRecipient(cc);
    }


    if (ccSelf) {
        EmailAddress *cc = new EmailAddress(senderEmail, senderName);
        message.addRecipient(cc);
    }

    for (int i=0; i<bccAddresses.size(); i++) {
        EmailAddress *bcc = new EmailAddress(bccAddresses[i], bccAddresses[i]);
        message.addRecipient(bcc);
    }

    // Set the subject
    message.setSubject(subject.trimmed());

    // Build the note content
    NoteTable ntable(global.db);
    Note n;
    ntable.get(n,lid,false,false);
    NoteFormatter formatter;
    formatter.setNote(n, false);
    prepareEmailMessage(&message, note, formatter.rebuildNoteHTML());

    // Send the actual message.
    if (!smtp.connectToHost()) {
        QLOG_ERROR()<< "Failed to connect to host!";
        errorMessage = tr("Connection Error: Unable to connect to host.");
        return 8;
    }

    if (!smtp.login()) {
        QLOG_ERROR() << "Failed to login!";
        errorMessage =  tr("Login Error : Unable to login.");
        return 8;
    }

    if (!smtp.sendMail(message)) {
        errorMessage = tr("Send Error : Unable to send email.");
        QLOG_ERROR() << "Failed to send mail!";
        return 8;
    }

    smtp.quit();
    errorMessage = tr("Message Sent");
    return 0;
}

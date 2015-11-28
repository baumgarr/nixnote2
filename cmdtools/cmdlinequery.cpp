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


#include "cmdlinequery.h"
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QStringList>

#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "global.h"
#include "sql/nsqlquery.h"


extern Global global;

CmdLineQuery::CmdLineQuery(QObject *parent) :
    QObject(parent)
{
    stdout=true;
    printHeaders=true;
}


void CmdLineQuery::write(QList<qint32> lids, QString filename) {
    QString format = "%i%n%t%g%c";
    if (this->outputFormat != "")
        format = outputFormat;
    QString delimiter = "|";
    if (this->delimiter != "")
        delimiter = this->delimiter;
    QFile *outputFile;
    if (filename != "") {
        outputFile = new QFile(filename);
        stdout=false;
        outputFile->open(QIODevice::WriteOnly);
        if (!outputFile->isOpen()) {
            return;
        }
        out = new QTextStream(outputFile);
    }

    QStringList formats = format.split("%");

    if (printHeaders) {
        QString line;
        for (int j=1; j<formats.size(); j++) {
            if (formats[j].startsWith("i"))
                line = line + lineBuilder(tr("ID"), formats[j], 7);
            if (formats[j].startsWith("t")) {
                line = line + lineBuilder(tr("Title"), formats[j], 15);
            }
            if (formats[j].startsWith("n")) {
                line = line + lineBuilder(tr("Notebook"), formats[j], 15);
            }
            if (formats[j].startsWith("g")) {
                line = line + lineBuilder(tr("Tags"), formats[j], 20);
            }
            if (formats[j].startsWith("c")) {
                line = line + lineBuilder(tr("Created"), formats[j], 20);
            }
            if (formats[j].startsWith("u")) {
                line = line + lineBuilder(tr("Updated"), formats[j], 20);
            }
            if (formats[j].startsWith("e")) {
                line = line + lineBuilder(tr("Sync"), formats[j], 3);
            }
            if (formats[j].startsWith("s")) {
                line = line + lineBuilder(tr("Source URL"), formats[j], 20);
            }
            if (formats[j].startsWith("a")) {
                line = line + lineBuilder(tr("Author"), formats[j], 15);
            }
            if (formats[j].startsWith("x")) {
                line = line + lineBuilder(tr("Todo"), formats[j], 4);
            }
            if (formats[j].startsWith("r")) {
                line = line + lineBuilder(tr("Reminder Due"), formats[j], 20);
            }
            if (formats[j].startsWith("v")) {
                line = line + lineBuilder(tr("Reminder Done"), formats[j], 20);
            }
            if (j<formats.size()-1)
                line = line + delimiter;
        }
        writeLine(line+QString("\n"));
        line = "";
        for (int j=1; j<formats.size(); j++) {
            if (formats[j].startsWith("i"))
                line = line + lineBuilder("-", formats[j], 7, '-');
            if (formats[j].startsWith("t")) {
                line = line + lineBuilder("-", formats[j], 15, '-');
            }
            if (formats[j].startsWith("n")) {
                line = line + lineBuilder("-", formats[j], 15,'-');
            }
            if (formats[j].startsWith("g")) {
                line = line + lineBuilder("-", formats[j], 20, '-');
            }
            if (formats[j].startsWith("c")) {
                line = line + lineBuilder("-", formats[j], 20, '-');
            }
            if (formats[j].startsWith("u")) {
                line = line + lineBuilder("-", formats[j], 20, '-');
            }
            if (formats[j].startsWith("e")) {
                line = line + lineBuilder("-", formats[j], 3, '-');
            }
            if (formats[j].startsWith("s")) {
                line = line + lineBuilder("-", formats[j], 20,'-');
            }
            if (formats[j].startsWith("a")) {
                line = line + lineBuilder("-", formats[j], 15, '-');
            }
            if (formats[j].startsWith("x")) {
                line = line + lineBuilder("-", formats[j], 4,'-');
            }
            if (formats[j].startsWith("r")) {
                line = line + lineBuilder("-", formats[j], 20, '-');
            }
            if (formats[j].startsWith("v")) {
                line = line + lineBuilder("-", formats[j], 20, '-');
            }
            if (j<formats.size()-1)
                line = line + delimiter;
        }
        writeLine(line+QString("\n"));
    }

    NoteTable notetable(global.db);
    for (int i=0; i<lids.size(); i++) {
        QString line;
        Note n;
        if (notetable.get(n,lids[i],false,false)) {
            for (int j=1; j<formats.size(); j++) {
                NSqlQuery query(global.db);
                QString tags;
                QString notebook;
                QString title;
                QDateTime updated;
                QDateTime created;
                QString synchronized;
                QString sourceUrl;
                QString author;
                QString hasTodo;
                QDateTime reminderTime;
                QDateTime reminderDoneTime;
                query.prepare("select notebook, title, dateCreated, dateUpdated, tags, isdirty, sourceUrl, author, hasTodo, reminderTime, reminderDoneTime from NoteTable where lid=:lid");
                query.bindValue(":lid",lids[i]);
                query.exec();
                if (query.next()) {
                    qlonglong dateValue;
                    bool boolValue;
                    notebook = query.value(0).toString();
                    title = query.value(1).toString();
                    dateValue = query.value(2).toLongLong()/1000;
                    created.setTime_t(dateValue);
                    dateValue = query.value(3).toLongLong()/1000;
                    updated.setTime_t(dateValue);
                    tags = query.value(4).toString();
                    boolValue = query.value(5).toBool();
                    if (boolValue)
                        synchronized=tr("No");
                    else
                        synchronized=tr("Yes");
                    sourceUrl = query.value(6).toString();
                    author = query.value(7).toString();
                    boolValue = query.value(8).toBool();
                    if (boolValue)
                        hasTodo=tr("Yes");
                    else
                        hasTodo=tr("No");
                    dateValue = query.value(9).toLongLong()/1000;
                    reminderTime.setTime_t(dateValue);
                    dateValue = query.value(10).toLongLong()/1000;
                    reminderDoneTime.setTime_t(dateValue);
                }
                if (formats[j].startsWith("i"))
                    line = line + lineBuilder(QString::number(lids[i]), formats[j], 7);
                if (formats[j].startsWith("t")) {
                    line = line + lineBuilder(title, formats[j], 15);
                }
                if (formats[j].startsWith("n")) {
                    line = line + lineBuilder(notebook, formats[j], 15);
                }
                if (formats[j].startsWith("g")) {
                    line = line + lineBuilder(tags, formats[j], 20);
                }
                if (formats[j].startsWith("c")) {
                    QString date = created.toString(global.dateFormat + QString(" ") +global.timeFormat);
                    line = line + lineBuilder(date, formats[j], 20);
                }
                if (formats[j].startsWith("u")) {
                    QString date = updated.toString(global.dateFormat + QString(" ") +global.timeFormat);
                    line = line + lineBuilder(date, formats[j], 20);
                }
                if (formats[j].startsWith("e")) {
                    line = line + lineBuilder(synchronized, formats[j], 3);
                }
                if (formats[j].startsWith("s")) {
                    line = line + lineBuilder(sourceUrl, formats[j], 20);
                }
                if (formats[j].startsWith("a")) {
                    line = line + lineBuilder(author, formats[j], 15);
                }
                if (formats[j].startsWith("x")) {
                    line = line + lineBuilder(hasTodo, formats[j], 3);
                }
                if (formats[j].startsWith("r")) {
                    QString date="";
                    if (reminderTime.toMSecsSinceEpoch() > 0)
                        date = reminderTime.toString(global.dateFormat + QString(" ") +global.timeFormat);
                    line = line + lineBuilder(date, formats[j], 20);
                }
                if (formats[j].startsWith("v")) {
                    QString date="";
                    if (reminderDoneTime.toMSecsSinceEpoch() > 0)
                        date = reminderDoneTime.toString(global.dateFormat + QString(" ") +global.timeFormat);
                    line = line + lineBuilder(date, formats[j], 20);
                }
                if (j<formats.size()-1)
                    line = line + delimiter;
            }
            writeLine(line+QString("\n"));
        }
    }

    if (!stdout && outputFile != NULL && outputFile->isOpen())
        outputFile->close();
}



QString CmdLineQuery::lineBuilder(QString value, QString format, int defaultPadding, QChar padChar) {
    QString formatString = format;
    int padLen = defaultPadding;
    bool truncate = false;
    if (formatString.contains(":")) {
        truncate=true;
        formatString.chop(1);
    }
    formatString = formatString.mid(1);
    int newPadding = formatString.toInt();
    if (newPadding>0)
        padLen = newPadding;
    if (padLen>0)
        return value.leftJustified(padLen, padChar, truncate);
    else
        return value;
}

void CmdLineQuery::writeLine(QString line) {
    if (stdout) {
        std::cout << line.toStdString();
        return;
    }
    *out << line;
}


QString CmdLineQuery::wrap() {

    QString returnValue;
    QXmlStreamWriter *writer = new QXmlStreamWriter(&returnValue);
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");
    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE NixNote-Query>");
    writer->writeStartElement("nixnote-query");
    writer->writeAttribute("version", "2");
    writer->writeAttribute("application", "NixNote");
    writer->writeAttribute("applicationVersion", "2.x");
    writer->writeStartElement("Query");
    writer->writeTextElement("ReturnUuid", this->returnUuid);
    writer->writeTextElement("QueryString", query);
    writer->writeTextElement("Delimiter", delimiter);
    writer->writeTextElement("OutputFormat", outputFormat);
    if (printHeaders)
        writer->writeTextElement("PrintHeaders", "Yes");
    else
        writer->writeTextElement("PrintHeaders", "No");
    writer->writeEndElement();
    writer->writeEndElement();
    writer->writeEndDocument();
    return returnValue;
}

void CmdLineQuery::unwrap(QString data) {
    lastError = 0;
    QLOG_DEBUG() << data;
    QXmlStreamReader reader(data);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError()) {
            errorMessage = reader.errorString();
            QLOG_ERROR() << "************************* ERROR READING IMPORT " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader.name().toString().toLower() == "query" && !reader.isEndElement()) {
            reader.readNext();
            while(reader.name().toString().toLower() != "query") {
                if (reader.name().toString().toLower() == "returnuuid" && reader.isStartElement()) {
                    reader.readNext();
                    returnUuid = reader.text().toString();
                } else if (reader.name().toString().toLower() == "querystring" && reader.isStartElement()) {
                    reader.readNext();
                    query = reader.text().toString();
                } else if (reader.name().toString().toLower() == "delimiter" && reader.isStartElement()) {
                    reader.readNext();
                    delimiter = reader.text().toString();
                } else if (reader.name().toString().toLower() == "outputformat" && reader.isStartElement()) {
                    reader.readNext();
                    outputFormat = reader.text().toString();
                } else if (reader.name().toString().toLower() == "printheaders" && reader.isStartElement()) {
                    reader.readNext();
                    QString value = reader.text().toString();
                    if (value.toLower()=="yes")
                        this->printHeaders=true;
                    else
                        this->printHeaders=false;
                } else
                    reader.readNext();
            }
        }
    }
}

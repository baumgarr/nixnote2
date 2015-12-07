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


#include "global.h"
#include <QXmlStreamReader>
#include "extractnotetext.h"

extern Global global;

ExtractNoteText::ExtractNoteText(QObject *parent) :
    QObject(parent)
{
}



QString ExtractNoteText::wrap() {

    QString returnValue;
    QXmlStreamWriter *writer = new QXmlStreamWriter(&returnValue);
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");
    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE NixNote-NoteTextExtract>");
    writer->writeStartElement("nixnote-text-extract");
    writer->writeAttribute("version", "2");
    writer->writeAttribute("application", "NixNote");
    writer->writeAttribute("applicationVersion", "2.x");
    writer->writeStartElement("NoteExtract");
    writer->writeTextElement("ReturnUuid", this->returnUuid);
    if (this->text.trimmed() != "")
        writer->writeTextElement("ReturnText", this->text);
    writer->writeTextElement("Lid", QString::number(this->lid));
    writer->writeEndElement();
    writer->writeEndElement();
    writer->writeEndDocument();
    return returnValue;
}

void ExtractNoteText::unwrap(QString data) {
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
        if (reader.name().toString().toLower() == "nixnote-text-extract" && !reader.isEndElement()) {
            reader.readNext();
            while(reader.name().toString().toLower() != "nixnote-text-extract") {
                if (reader.name().toString().toLower() == "returnuuid" && reader.isStartElement()) {
                    reader.readNext();
                    returnUuid = reader.text().toString();
                } else if (reader.name().toString().toLower() == "lid" && reader.isStartElement()) {
                    reader.readNext();
                    lid = reader.text().toString().toInt();
                } else if (reader.name().toString().toLower() == "returntext" && reader.isStartElement()) {
                    reader.readNext();
                    text = reader.text().toString();
                } else
                    reader.readNext();
            }
        }
    }
}


QString ExtractNoteText::stripTags(QString content) {
    // Start looking through the note
    qint32 startPos =content.indexOf(QChar('<'));
    qint32 endPos = content.indexOf(QChar('>'),startPos)+1;
    content.remove(startPos,endPos-startPos);

    // Remove encrypted text
    while (content.contains("<en-crypt")) {
        startPos = content.indexOf("<en-crypt");
        endPos = content.indexOf("</en-crypt>") + 11;
        content = content.mid(0,startPos)+content.mid(endPos);
    }

    // Remove any XML tags
    while (content.contains(QChar('<'))) {
        startPos = content.indexOf(QChar('<'));
        endPos = content.indexOf(QChar('>'),startPos)+1;
        content.remove(startPos,endPos-startPos);
    };

    // Get the content as an HTML doc.
    QTextDocument textDocument;
    textDocument.setHtml(content);
    return textDocument.toPlainText();

    return "";
}

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

#include "noteformatter.h"
#include "filters/ensearch.h"
#include "sql/resourcetable.h"
#include "sql/notebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/linkednotebooktable.h"
#include "global.h"
#include "filters/filtercriteria.h"
#include "utilities/mimereference.h"

#include <QFileIconProvider>
#include <QIcon>
#include <QList>

//#include "<QPainter>

#include <iostream>
using namespace std;

extern Global global;

/* Constructor. */
NoteFormatter::NoteFormatter(QObject *parent) :
    QObject(parent)
{
}


  /*
  Set the note we are currently formatting.  The pdfPreview
  is an indication if we should generate a picture for the
  attachment rather than doing it as an attachment.
  */
void NoteFormatter::setNote(Note n, bool pdfPreview) {
    this->pdfPreview = pdfPreview;
    this->note = n;
    content = "";
    this->enableHighlight = true;
    readOnly = false;
    inkNote = false;
    if (note.__isset.attributes && note.attributes.__isset.contentClass && note.attributes.contentClass != "")
        readOnly = true;

}


/* Return the formatted content */
QString NoteFormatter::getPage() {
    return this->content;
}




/* If we have search criteria, then we highlight the text matching
  those results in the note. */
void NoteFormatter::setHighlight() {
    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (criteria->isSearchStringSet())
        enableHighlight = true;
    else
        enableHighlight = false;
}

/* If we are here because we are viewing note history, then we
  set the flag here.  Note history is almost the same as a regular
  note, but there are some differences. */
void NoteFormatter::setNoteHistory(bool value) {
    this->noteHistory = value;
}


/* Take the ENML note and transform it into HTML that WebKit will
  not complain about */
QByteArray NoteFormatter::rebuildNoteHTML() {

    QLOG_DEBUG() << "Rebuilding Note: " << QString::fromStdString(note.guid) << " : " <<
                    QString::fromStdString(note.title);

    formatError = false;
    readOnly = false;

    // First try to read the document.  If it fails we need to clean it up
    content.append(QString::fromStdString(note.content));
    QDomDocument doc;
    QString emsg;
    bool goodReturn = doc.setContent(content, &emsg);

    if (!goodReturn) {
        QLOG_DEBUG() << "Error with initial document: " << emsg << " running through tidy";
        // Run it through "tidy".  It is a program which will fix any invalid XML
        // and give us the results back through stdout.  In a perfect world this
        // wouldn't be needed, but I've seen times where the ENML is bad for some reason.
        QProcess tidyProcess;
        tidyProcess.start("tidy -xml -raw -q -e", QIODevice::ReadWrite|QIODevice::Unbuffered);
        QLOG_DEBUG() << "Starting tidy " << tidyProcess.waitForStarted();
        QByteArray b;
        b.append(QString::fromStdString(note.content));
        tidyProcess.write(b);
        tidyProcess.closeWriteChannel();
        QLOG_DEBUG() << "Stopping tidy " << tidyProcess.waitForFinished() << " Return Code: " << tidyProcess.state();
        QLOG_DEBUG() << "Tidy Errors:" << tidyProcess.readAllStandardError();
        content = tidyProcess.readAllStandardOutput();

        // If the content is null, then we had a problem.  Just risk going forward without tidy cleanup
        if (content.size() == 0)
            content = b;
        doc.setContent(content);
    }

    // Remove all the temporary file names
    tempFiles.clear();
    modifyTags(doc);

    // If we have search criteria, then do the highlighting
    if (enableHighlight)
        doc = addHighlight(doc);

    // Finish up and return the HTML to the user
    QDomElement docElem = doc.documentElement();
    docElem.setTagName("body");
    content = doc.toByteArray(3);
        qint32 index = content.indexOf("<body");
    content.remove(0,index);
    content.prepend("<style type=\"text/css\">.en-crypt-temp { border-collapse:collapse; border-style:solid; border-color:blue; padding:0.0mm 0.0mm 0.0mm 0.0mm; }</style>");
    content.prepend("<style type=\"text/css\">en-hilight { background-color: rgb(255,255,0) }</style>");
    content.prepend("<style> img { height:auto; width:auto; max-height:auto; max-width:100%; }</style>");
    content.prepend("<head><meta http-equiv=\"content-type\" content=\"text-html; charset=utf-8\"></head>");
    content.prepend("<html>");
    content.append("</html>");

    if (!formatError && !readOnly) {
        NotebookTable ntable;
        qint32 notebookLid = ntable.getLid(note.notebookGuid);
        if (ntable.isReadOnly(notebookLid))
            readOnly = true;
    }
    if (note.__isset.active && !note.active)
        readOnly = true;
    return content;
}


/*
  This will go through and modify some of the ENML tags and turn
  them into HTML tags.  Things like en-media & en-crypt have no
  HTML values, so we turn them into HTML.
  */
void NoteFormatter::modifyTags(QDomDocument &doc) {
    QLOG_TRACE() << "Entering NeverNote.modifyTags";
    tempFiles.clear();
    QDomElement docElem = doc.documentElement();

    // Modify en-media tags
    QDomNodeList anchors = docElem.elementsByTagName("en-media");
    qint32 enMediaCount = anchors.length();
    for (qint32 i=enMediaCount-1; i>=0; --i) {
            QDomElement enmedia = anchors.at(i).toElement();
            if (enmedia.hasAttribute("type")) {
                    QDomAttr attr = enmedia.attributeNode("type");
                    QDomAttr hash = enmedia.attributeNode("hash");
                    QStringList type = attr.nodeValue().split("/");
                    if (type.size() >= 2) {
                        QString appl = type[1];
                        if (type[0] == "image")
                            modifyImageTags(doc, docElem, enmedia, hash);
                        else
                            modifyApplicationTags(doc, docElem, enmedia, hash, appl);
                    }
            }
    }

    // Modify todo tags
    anchors = docElem.elementsByTagName("en-todo");
    qint32 enTodoCount = anchors.length();
    for (qint32 i=enTodoCount-1; i>=0; i--) {
            QDomElement enmedia = anchors.at(i).toElement();
            modifyTodoTags(enmedia);
    }

    anchors = docElem.elementsByTagName("en-crypt");
    qint32 enCryptLen = anchors.length();
    for (qint32 i=enCryptLen-1; i>=0; i--) {
            QDomElement enmedia(anchors.at(i).toElement());
            enmedia.setAttribute("contentEditable","false");
            enmedia.setAttribute("src", QString("file://")+global.fileManager.getImageDirPath("encrypt.png"));
            enmedia.setAttribute("en-tag","en-crypt");
            enmedia.setAttribute("alt", enmedia.text());
            global.cryptCounter++;
            enmedia.setAttribute("id", "crypt"+QString().number(global.cryptCounter));
            QString encryptedText = enmedia.text();

            // If the encryption string contains crlf at the end, remove them because they mess up the javascript.
            if (encryptedText.endsWith("\n"))
                    encryptedText.truncate(encryptedText.length()-1);
            if (encryptedText.endsWith("\r"))
                    encryptedText.truncate(encryptedText.length()-1);

            // Add the commands
            QString hint = enmedia.attribute("hint");
            hint = hint.replace("'","&apos;");
            enmedia.setAttribute("onClick", "window.jsbridge.decryptText('crypt"+QString().number(global.cryptCounter)+"', '"+encryptedText+"', '"+hint+"');");
            enmedia.setAttribute("onMouseOver", "style.cursor='hand'");
            enmedia.setTagName("img");
            enmedia.removeChild(enmedia.firstChild());   // Remove the actual encrypted text
    }


    // Modify link tags
    anchors = docElem.elementsByTagName("a");
    enCryptLen = anchors.length();
    for (unsigned int i=0; i<anchors.length(); i++) {
            QDomElement element = anchors.at(i).toElement();
            if (!element.attribute("href").toLower().startsWith("latex://"))
                    element.setAttribute("title", element.attribute("href"));
            else {
                    element.setAttribute("title", element.attribute("title").toLower().replace("http://latex.codecogs.com/gif.latex?",""));
            }
    }

    QLOG_TRACE() << "Leaving NeverNote.modifyTags";
}


/* Add the highlight to the document.  This currently isn't supported */
QDomDocument NoteFormatter::addHighlight(QDomDocument &doc) {
    QLOG_DEBUG() << "*** UNSUPPORTED *** NoteFormatter.addHighlight()";
    //if (enSearch.hilightWords == null || enSearch.hilightWords.size() == 0)
        return doc;
    //XMLInsertHilight hilight(doc, enSearch.hilightWords);
    //    return hilight.getDoc();
}



/* This function works the same as the addHighlight, but instead of highlighting
  text in a note, it highlights the text in an image. */
void NoteFormatter::addImageHighlight(qint32 resLid, QFile &f) {
    if (enSearch.hilightWords.size() == 0)
        return;

    // Get the image resource recognition data.  This tells where to highlight the image
    ResourceTable resTable;
    Resource recoResource;
    resTable.getResourceRecognition(recoResource, resLid);
    if (!recoResource.__isset.recognition || !recoResource.data.__isset.size || !recoResource.data.__isset.body ||
            recoResource.data.size == 0) {
        return;
    }

    // Now we have the recognition data.  We need to go through it
    QByteArray recoData;
    recoData.append(QString::fromStdString(recoResource.recognition.body));
    QString xml(recoData);

    // Get a painter for the image.  This the background for what we are painting on (in other words,
    // the initial image
    QPixmap pix(f.fileName());
    QPixmap highlightedPix(pix.size());
    QPainter p(&highlightedPix);
    p.drawPixmap(0,0,pix);

    // Create a transparent pixmap.  The only non transparent piece is teh
    // highlight that will be overlaid on the old image
    QPixmap overlayPix(pix.size());
    overlayPix.fill(Qt::transparent);
    QPainter p2(&overlayPix);
    p2.setBackgroundMode(Qt::TransparentMode);
    p2.setRenderHint(QPainter::Antialiasing,true);
    QColor yellow(Qt::yellow);
    p2.setBrush(yellow);

    // Now, we have the image.  We need to go through all the recognition data to highlight
    // what we've found.
    QDomDocument doc;
    doc.setContent(xml);

    // Go through the "item" nodes
    QDomNodeList anchors = doc.elementsByTagName("item");
    for (unsigned int i=0; i<anchors.length(); i++) {
        QDomElement element = anchors.at(i).toElement();
        int x = element.attribute("x").toInt();
        int y = element.attribute("y").toInt();
        int w = element.attribute("w").toInt();
        int h = element.attribute("h").toInt();

        // Get all children ("t" nodes)
        QDomNodeList children = element.childNodes();
        for (unsigned int j=0; j<children.length(); j++) {
            QDomElement child = children.at(j).toElement();
            if (child.nodeName().toLower() == "t") {
                QString text = child.text();
                int weight = child.attribute("w").toInt(); // Image weight
                if (weight > MINIMUM_RECOGNITION_WEIGHT) {

                    // Check to see if this word matches something we're looking for
                    for (int k=0; k<enSearch.hilightWords.size(); k++) {
                        QString searchWord = enSearch.hilightWords[k].toLower();
                        if (searchWord.endsWith("*"))
                            searchWord.chop(1);
                        if (text.toLower().contains(searchWord))
                            p2.drawRect(x,y,w,h);
                    }
                }
            }
        }
    }
    p2.end();

    // Paint the highlight onto the background & save over the original
    p2.setOpacity(0.4);
    p2.drawPixmap(0,0,overlayPix);
    p.end();
    highlightedPix.save(f.fileName());
}



/* Modify an image tag.  Basically we turn it back into a picture, write out the file, and
  modify the ENML */
void NoteFormatter::modifyImageTags(QDomDocument &doc, QDomElement &docElement, QDomElement &enMedia, QDomAttr &hash) {
    QLOG_DEBUG() << "Entering NoteFormatter::modifyImageTags";
    QString mimetype = enMedia.attribute("type");
    ResourceTable resourceTable;
    qint32 resLid = resourceTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    Resource r;
    if (resLid>0) {
        //QFile tfile(global.fileManager.getResDirPath(QString::number(resLid)+type));
        resourceTable.get(r, resLid);
        MimeReference ref;
        QString filename;
        if (r.__isset.attributes && r.attributes.__isset.fileName)
            filename = QString::fromStdString(r.attributes.fileName);
        QString type = ref.getExtensionFromMime(mimetype, filename);

        if (r.__isset.data && r.data.__isset.body && r.data.body.length() > 0) {
            enMedia.setAttribute("src", "file:///"+global.fileManager.getDbDirPath(QString("dba/") +QString::number(resLid) +type));
            // Check if this is a LaTeX image
            if (r.__isset.attributes && r.attributes.__isset.sourceURL &&
                QString::fromStdString(r.attributes.sourceURL).toLower().startsWith("http://latex.codecogs.com/gif.latex?")) {
                QDomElement newText(doc.createElement("a"));
                enMedia.setAttribute("en-tag", "en-latex");
                newText.setAttribute("onMouseOver", "style.cursor='pointer'");
                newText.setAttribute("title", QString::fromStdString(r.attributes.sourceURL));
                newText.setAttribute("href", "latex://"+QString::number(resLid));
                enMedia.parentNode().replaceChild(newText, enMedia);
                newText.appendChild(enMedia);
            }
            enMedia.setAttribute("onContextMenu", "window.browserWindow.imageContextMenu('"
                                 +QString::number(resLid) +"', '"
                                 +QString::number(resLid) +type  +"');");

        }
    } else {
        resourceError = true;
        readOnly = true;
    }

    // Reset the tags to something that WebKit will understand
    enMedia.setAttribute("en-tag", "en-media");
    enMedia.setNodeValue("");
    enMedia.setAttribute("lid", resLid);
    enMedia.setTagName("img");
}



// Modify the en-media tag into an attachment
void NoteFormatter::modifyApplicationTags(QDomDocument &doc, QDomElement &docElem, QDomElement &enmedia, QDomAttr &hash, QString appl) {
    QLOG_TRACE() <<  "Entering NeverNote.modifyApplicationTags";
    if (appl.toLower() == "vnd.evernote.ink") {
            inkNote = true;
            readOnly = true;
            buildInkNote(enmedia, hash);
            QLOG_DEBUG() << doc.toString();
            return;
    }
    ResourceTable resTable;
    QString contextFileName;
    qint32 resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    Resource r;
    resTable.get(r, resLid);
    if (!r.__isset.data)
        resourceError = true;
    else {
        if (r.mime == "application/pdf" && pdfPreview) {
            modifyPdfTags(resLid, enmedia);
            return;
        }
        QString fileDetails = "";
        MimeReference ref;
        if (r.__isset.attributes && r.attributes.__isset.fileName)
            fileDetails = QString::fromStdString(r.attributes.fileName);
        fileDetails = ref.getExtensionFromMime(QString::fromStdString(r.mime), fileDetails);


        if (fileDetails != "") {
            if (!noteHistory) {
                enmedia.setAttribute("href", QString("nnres:") +QString::number(resLid)
                                     +global.attachmentNameDelimeter +fileDetails);
                contextFileName = global.fileManager.getTmpDirPath(QString::number(resLid) +global.attachmentNameDelimeter + fileDetails);
            } else {
                enmedia.setAttribute("href", QString("nnres:") +QString::number(resLid) +QString("-")+ QString::number(note.updateSequenceNum)
                                     +global.attachmentNameDelimeter +fileDetails);
                contextFileName = global.fileManager.getTmpDirPath(QString::number(resLid) +QString("-")+ QString(note.updateSequenceNum)
                                                                   +global.attachmentNameDelimeter + fileDetails);
            }
        } else {
            if (!noteHistory) {
                enmedia.setAttribute("href", "nnres:" +QString::number(resLid) +QString("-") +QString(note.updateSequenceNum)
                                     +global.attachmentNameDelimeter +appl);
                contextFileName = global.fileManager.getTmpDirPath(QString::number(resLid) +QString("-")
                                                                   +QString(note.updateSequenceNum)
                                                                   +global.attachmentNameDelimeter + appl);
            } else {
                enmedia.setAttribute("href", "nnres:" +QString::number(resLid)
                                     +global.attachmentNameDelimeter +appl);
                contextFileName = global.fileManager.getTmpDirPath(QString::number(resLid)
                                                                   +global.attachmentNameDelimeter + appl);
            }
        }

        // Setup the context menu.  This is useful if we want to do a "save as" or such
        contextFileName = contextFileName.replace("\\", "/");
        enmedia.setAttribute("onContextMenu", "window.browserWindow.resourceContextMenu('" +contextFileName +"');");
        enmedia.setAttribute("en-tag", "en-media");
        enmedia.setAttribute("lid", QString::number(resLid));
        enmedia.setTagName("a");

        QDomElement newText = doc.createElement("img");
        QFile tfile(contextFileName);
        tfile.open(QIODevice::WriteOnly);
        tfile.close();

        // Build an icon of the image
        QString fileExt;
        if (r.__isset.attributes && r.attributes.__isset.fileName)
            fileExt = QString::fromStdString(r.attributes.fileName);
        else
            fileExt = appl;
        fileExt = ref.getExtensionFromMime(r.mime, r.attributes.fileName);
        QString icon = findIcon(resLid, r, fileExt);
        newText.setAttribute("src", "file:///"+icon);
        if (r.__isset.attributes && r.attributes.__isset.fileName)
            newText.setAttribute("title",QString::fromStdString(r.attributes.fileName));
        newText.setAttribute("en-tag", "temporary");
        enmedia.removeChild(enmedia.firstChild());
        enmedia.appendChild(newText);
        QLOG_TRACE() << "Leaving NeverNote.modifyApplicationTags";
    }
}



// Build an icon for any attachments
QString NoteFormatter::findIcon(qint32 lid, Resource r, QString appl) {

    // First get the icon for this type of file
    QString fileName = global.fileManager.getDbaDirPath(QString::number(lid) +appl);
    QIcon icon = QFileIconProvider().icon(QFileInfo(fileName));

    // Build a string name for the display
    QString displayName;
    if (r.__isset.attributes && r.attributes.__isset.fileName)
        displayName = QString::fromStdString(r.attributes.fileName);
    else
        displayName =  appl.toUpper() +" " +QString(tr("File"));

    // Setup the painter
    QPainter p;

    // Setup the font
    QFont font=p.font() ;
    font.setPointSize ( 8 );
    font.setFamily("Arial");
    QFontMetrics fm(font);
    int width =  fm.width(displayName);
    if (width < 40)  // steup a minimum width
        width = 40;
    width=width+50;  // Add 10 px for padding & 40 for the icon

    // Start drawing a new pixmap for  the image in the note
    QPoint textPoint(40,15);
    QPoint sizePoint(40,29);
    QPixmap pixmap(width,37);
    pixmap.fill();

    p.begin(&pixmap);
    p.setFont(font);
    p.drawPixmap(QPoint(3,3), icon.pixmap(QSize(30,40)));

    // Write out the attributes of the file
    p.drawText(textPoint, displayName);

    QString unit = QString(tr("Bytes"));
    qint32 size = QFileInfo(fileName).size();
    if (size > 1024) {
        size = size/1024;
        unit = QString(tr("KB"));
    }
    if (size > 1024) {
        size = size/1024;
        unit= QString("MB");
    }
    p.drawText(sizePoint, QString::number(size).trimmed() +" " +unit);
    p.drawRect(0,0,width-1,37-1);   // Draw a rectangle around the image.
    p.end();

    // Now that it is drawn, we write it out to a temporary file
    QString tmpFile = global.fileManager.getTmpDirPath(QString::number(lid) + QString("_icon.png"));
    pixmap.save(tmpFile, "png");
    return tmpFile;
}




// Modify the en-to tag into an input field
void NoteFormatter::modifyTodoTags(QDomElement &todo) {
    QLOG_TRACE() << "Entering NeverNote.modifyTodoTags";
    todo.setAttribute("type", "checkbox");
    QString checked = todo.attribute("checked");
    todo.removeAttribute("checked");
    if (checked.toLower() == "true")
            todo.setAttribute("checked", "");
    else
            todo.setAttribute("unchecked","");
    todo.setAttribute("value", checked);
    todo.setAttribute("onClick", "value=checked;window.jambi.contentChanged(); ");
    todo.setAttribute("onMouseOver", "style.cursor='hand'");
    todo.setTagName("input");
    QLOG_TRACE() << "Leaving NeverNote.modifyTodoTags";
}





/* If we have an ink note, then we need to pull the image and display it */
bool NoteFormatter::buildInkNote(QDomElement &docElem, QDomAttr &hash) {

    ResourceTable resTable;
    qint32 resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    if (resLid <= 0)
        return false;
    docElem.setAttribute("en-tag", "en-media");
    docElem.setAttribute("lid", QString::number(resLid));
    docElem.setAttribute("type", "application/vnd.evernote.ink");
    QString filename = QString("file:///") +global.fileManager.getDbaDirPath()+QString::number(resLid)+QString(".png");
    docElem.setAttribute("src", filename);
    docElem.setTagName("img");

    return true;
}



void NoteFormatter::modifyPdfTags(qint32 resLid, QDomElement &enmedia) {
    enmedia.setTagName("object");
    //enmedia.setAttribute("type", "nixnote/pdf");
    enmedia.setAttribute("width", "100%");
    enmedia.setAttribute("height", "100%");
    enmedia.setAttribute("lid", resLid);
}

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

#include <QFileSystemModel>
#include <QFileIconProvider>
#include <poppler-qt4.h>
#include <QIcon>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <QList>


#include <iostream>
using namespace std;

extern Global global;

/* Constructor. */
NoteFormatter::NoteFormatter(QObject *parent) :
    QObject(parent)
{
    thumbnail = false;
    this->setNoteHistory(false);
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

    formatError = false;
    readOnly = false;

    QWebPage page;
    QEventLoop loop;
    QString html = preHtmlFormat(QString::fromStdString(note.content));
    html.replace("<en-note", "<body");
    html.replace("</en-note>", "</body>");
    //html.prepend("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></head>");
    QByteArray htmlPage;
    htmlPage.append(html);
    page.mainFrame()->setContent(htmlPage);
//    page.mainFrame()->setContent(htmlPage, "application/xhtml+xml");
    QObject::connect(&page, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));

    modifyTags(page);

    // Finish up and return the HTML to the user
    //QDomElement docElem = doc.documentElement();
    //docElem.setTagName("body");

    note.content = page.mainFrame()->toHtml().toStdString();
    content.clear();
    content.append(QString::fromStdString(note.content));

    qint32 index = content.indexOf("<body");
    content.remove(0,index);
    content.prepend("<style>img { height:auto; width:auto; max-height:auto; max-width:100%; }</style>");
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



    // Run it through "tidy".  It is a program which will fix any invalid HTML
    // and give us the results back through stdout.  In a perfect world this
    // wouldn't be needed, but WebKit doesn't always give back good HTML.
//    QProcess tidyProcess;
//    content.clear();
//    content.append(QString::fromStdString(note.content));
//    tidyProcess.start("tidy -raw -asxhtml -m -u -utf8 ", QIODevice::ReadWrite|QIODevice::Unbuffered);
//    QLOG_DEBUG() << "Starting tidy " << tidyProcess.waitForStarted();
//    tidyProcess.waitForStarted();
//    tidyProcess.write(content);
//    tidyProcess.closeWriteChannel();
//    tidyProcess.waitForFinished();
//    QLOG_DEBUG() << "Stopping tidy " << tidyProcess.waitForFinished() << " Return Code: " << tidyProcess.state();
//    QLOG_DEBUG() << "Tidy Errors:" << tidyProcess.readAllStandardError();
//    content.clear();
//    content.append(tidyProcess.readAllStandardOutput());

//    QDomDocument doc;
//    doc.setContent(content);
//    content = doc.toByteArray(-1);

    QLOG_DEBUG() << content;
    return content;
}



// This is to turn the <en-media/> tags into <en-media></en-media> tags because
// QWebPage tends to miss the /> tag and it can cause some text to be missed
QString NoteFormatter::preHtmlFormat(QString note) {
    int pos;
    QString content = note;
    pos = content.indexOf("<en-media");
    while (pos > 0) {
        int endPos = content.indexOf(">", pos);
        int tagEndPos = content.indexOf("/>", pos);

        // Check the next /> end tag.  If it is before the end
        // of the current tag or if it doesn't exist then we
        // need to fix the end of the img
        if (tagEndPos <= 0 || tagEndPos < endPos) {
            content = content.mid(0, endPos) + QByteArray("></en-media>") +content.mid(endPos+1);
        }
        pos = content.indexOf("<en-media", pos+1);
    }
    return content;
}




/*
  This will go through and modify some of the ENML tags and turn
  them into HTML tags.  Things like en-media & en-crypt have no
  HTML values, so we turn them into HTML.
  */
void NoteFormatter::modifyTags(QWebPage &doc) {
    tempFiles.clear();

    // Modify en-media tags
    QWebElementCollection anchors = doc.mainFrame()->findAllElements("en-media");
    foreach (QWebElement enmedia, anchors) {
        if (enmedia.hasAttribute("type")) {
            QString attr = enmedia.attribute("type");
            QString hash = enmedia.attribute("hash");
            QStringList type = attr.split("/");
            if (type.size() >= 2) {
                QString appl = type[1];
                if (type[0] == "image")
                    modifyImageTags(enmedia, hash);
                else
                    modifyApplicationTags(enmedia, hash, appl);
            }
        }
    }

    // Modify todo tags
    anchors = doc.mainFrame()->findAllElements("en-todo");
    qint32 enTodoCount = anchors.count();
    for (qint32 i=enTodoCount-1; i>=0; i--) {
            QWebElement enmedia = anchors.at(i);
            modifyTodoTags(enmedia);
    }

    anchors = doc.mainFrame()->findAllElements("en-crypt");
    qint32 enCryptLen = anchors.count();
    for (qint32 i=enCryptLen-1; i>=0; i--) {
        QWebElement enmedia = anchors.at(i);
        enmedia.setAttribute("contentEditable","false");
        enmedia.setAttribute("src", QString("file://")+global.fileManager.getImageDirPath("encrypt.png"));
        enmedia.setAttribute("en-tag","en-crypt");
        enmedia.setAttribute("alt", enmedia.toInnerXml());
        global.cryptCounter++;
        enmedia.setAttribute("id", "crypt"+QString().number(global.cryptCounter));
        QString encryptedText = enmedia.toInnerXml();

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
        enmedia.setInnerXml("");
        QString k = enmedia.toOuterXml();
        k.replace("<en-crypt", "<img");
        k.replace("img>", "<en-crypt");
        enmedia.setOuterXml(k);
    }


    // Modify link tags
    anchors = doc.mainFrame()->findAllElements("a");
    enCryptLen = anchors.count();
    for (qint32 i=0; i<anchors.count(); i++) {
        QWebElement element = anchors.at(i);
        if (!element.attribute("href").toLower().startsWith("latex://"))
            element.setAttribute("title", element.attribute("href"));
        else {
            element.setAttribute("title", element.attribute("title").toLower().replace("http://latex.codecogs.com/gif.latex?",""));
        }
    }

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
void NoteFormatter::modifyImageTags(QWebElement &enMedia, QString &hash) {
    QString mimetype = enMedia.attribute("type");
    QLOG_DEBUG() << enMedia.toOuterXml();
    ResourceTable resourceTable;
    qint32 resLid = resourceTable.getLidByHashHex(QString::fromStdString(note.guid), hash);
    Resource r;
    if (resLid>0) {
        resourceTable.get(r, resLid);
            QLOG_DEBUG() << enMedia.toOuterXml();
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
                enMedia.appendInside("<img/>");
                QWebElement newText = enMedia.lastChild();
                enMedia.setAttribute("en-tag", "en-latex");
                newText.setAttribute("onMouseOver", "style.cursor='pointer'");
                newText.setAttribute("title", QString::fromStdString(r.attributes.sourceURL));
                newText.setAttribute("href", "latex://"+QString::number(resLid));
                //enMedia.replace(newText);
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
    enMedia.setPlainText("");
    enMedia.setAttribute("lid", QString::number(resLid));
    // rename the <enmedia> tag to <img>
    QLOG_DEBUG() << enMedia.toOuterXml();

    enMedia.setOuterXml(enMedia.toOuterXml().replace("<en-media","<img"));
    enMedia.setOuterXml(enMedia.toOuterXml().replace("</en-media>","</img>"));
    QLOG_DEBUG() << enMedia.toOuterXml();
}



// Modify the en-media tag into an attachment
void NoteFormatter::modifyApplicationTags(QWebElement &enmedia, QString &hash, QString appl) {
    if (appl.toLower() == "vnd.evernote.ink") {
            inkNote = true;
            readOnly = true;
            buildInkNote(enmedia, hash);
            return;
    }
    ResourceTable resTable;
    QString contextFileName;
    qint32 resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash);
    Resource r;
    resTable.get(r, resLid);
    if (!r.__isset.data)
        resourceError = true;
    else {

        // If we are running the formatter and we are not generating a thumbnail
        if (r.mime == "application/pdf" && pdfPreview && !thumbnail) {
           modifyPdfTags(resLid, enmedia);
           return;
        }

        // If we are running the formatter so we can generate a thumbnail and it is a PDF
        if (r.mime == "application/pdf" && pdfPreview && thumbnail) {
            QString printImageFile = global.fileManager.getTmpDirPath() + QString::number(resLid) +QString("-print.jpg");
            QString file = global.fileManager.getDbaDirPath() + QString::number(resLid) +".pdf";
            Poppler::Document *doc;
            doc = Poppler::Document::load(file);
            if (doc == NULL)
                return;

            QImage *image = new QImage(doc->page(0)->renderToImage());
            image->save(printImageFile,"jpg");
            delete image;

            enmedia.setAttribute("src", printImageFile);
            enmedia.removeAttribute("hash");
            enmedia.removeAttribute("type");
            enmedia.setOuterXml(enmedia.toOuterXml().replace("<en-media","<img"));
            enmedia.setOuterXml(enmedia.toOuterXml().replace("</en-media>","</img>"));
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
                contextFileName = global.fileManager.getTmpDirPath("")+QString::number(resLid) +global.attachmentNameDelimeter + fileDetails;
            } else {
                enmedia.setAttribute("href", QString("nnres:") +QString::number(resLid) +QString("-")+ QString::number(note.updateSequenceNum)
                                     +global.attachmentNameDelimeter +fileDetails);
                contextFileName = global.fileManager.getTmpDirPath("")+QString::number(resLid) +QString("-")+ QString(note.updateSequenceNum)
                                                                   +global.attachmentNameDelimeter + fileDetails;
            }
        } else {
            if (!noteHistory) {
                enmedia.setAttribute("href", "nnres:" +QString::number(resLid) +QString("-") +QString(note.updateSequenceNum)
                                     +global.attachmentNameDelimeter +appl);
                contextFileName = global.fileManager.getTmpDirPath("") +QString::number(resLid) +QString("-")
                                                                   +QString(note.updateSequenceNum)
                                                                   +global.attachmentNameDelimeter + appl;
            } else {
                enmedia.setAttribute("href", "nnres:" +QString::number(resLid)
                                     +global.attachmentNameDelimeter +appl);
                contextFileName = global.fileManager.getTmpDirPath("") +QString::number(resLid)
                                                                   +global.attachmentNameDelimeter + appl;
            }
        }

        // Setup the context menu.  This is useful if we want to do a "save as" or such
        contextFileName = contextFileName.replace("\\", "/");
        enmedia.setAttribute("onContextMenu", "window.browserWindow.resourceContextMenu('" +contextFileName +"');");
        enmedia.setAttribute("en-tag", "en-media");
        enmedia.setAttribute("lid", QString::number(resLid));

        enmedia.appendInside("<img/>");
        QWebElement newText = enmedia.lastChild();

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
        //Rename the tag to a <a> link
        enmedia.setOuterXml(enmedia.toOuterXml().replace("<en-media","<a"));
        enmedia.setOuterXml(enmedia.toOuterXml().replace("</en-media>","</a>"));
    }
}



// Build an icon for any attachments
QString NoteFormatter::findIcon(qint32 lid, Resource r, QString appl) {

    // First get the icon for this type of file
    QString fileName = global.fileManager.getDbaDirPath(QString::number(lid) +appl);
    QIcon icon;
    QFileInfo info(fileName);
    QFileIconProvider provider;
    icon = provider.icon(info);

    // Build a string name for the display
    QString displayName;
    if (r.__isset.attributes && r.attributes.__isset.fileName)
        displayName = QString::fromStdString(r.attributes.fileName);
    else
        displayName =  appl.toUpper() +" " +QString(tr("File"));

    // Setup the painter
    QPainter p;

    // Setup the font
    QFont font; // =p.font() ;
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
void NoteFormatter::modifyTodoTags(QWebElement &todo) {
    todo.setAttribute("type", "checkbox");

    // Checks the en-to tag wheter or not the todo-item is checked or not
    // and sets up the HTML to keep storing the information in value
    QString checked = todo.attribute("checked");
    if (checked.toLower() == "true")
        todo.setAttribute("checked", "checked");
    else
        todo.removeAttribute("checked");

    todo.setAttribute("onClick", "if(!checked) removeAttribute('checked'); else setAttribute('checked', 'checked'); editorWindow.editAlert();");
    todo.setAttribute("style", "cursor: hand;");
    todo.setOuterXml(todo.toOuterXml().replace("en-todo","input"));
}





/* If we have an ink note, then we need to pull the image and display it */
bool NoteFormatter::buildInkNote(QWebElement &docElem, QString &hash) {

    ResourceTable resTable;
    qint32 resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash);
    if (resLid <= 0)
        return false;
    docElem.setAttribute("en-tag", "en-media");
    docElem.setAttribute("lid", QString::number(resLid));
    docElem.setAttribute("type", "application/vnd.evernote.ink");
    QString filename = QString("file:///") +global.fileManager.getDbaDirPath()+QString::number(resLid)+QString(".png");
    docElem.setAttribute("src", filename);
    QString k  = docElem.toOuterXml();
    k.replace("<en-media", "<img");
    k.replace("enmedia>", "img>");;
    docElem.setOuterXml(k);

    return true;
}



void NoteFormatter::modifyPdfTags(qint32 resLid, QWebElement &enmedia) {
    enmedia.setAttribute("width", "100%");
    enmedia.setAttribute("height", "100%");
    enmedia.setAttribute("lid", QString::number(resLid));
    QString x = enmedia.toOuterXml();
    x.replace("en-media", "object");
    enmedia.setOuterXml(x);
    x = enmedia.toOuterXml();
    QLOG_DEBUG() << x;
}

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
#include "sql/resourcetable.h"
#include "sql/notebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/linkednotebooktable.h"
#include "global.h"
#include "filters/filtercriteria.h"
#include "filters/filterengine.h"
#include "utilities/mimereference.h"

#include <QFileSystemModel>
#include <QFileIconProvider>
#if QT_VERSION < 0x050000
#include <poppler-qt4.h>
#else
#include <poppler-qt5.h>
#endif
#include <QIcon>
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
    //this->enableHighlight = true;
    readOnly = false;
    inkNote = false;
    NoteAttributes attributes;
    if (note.attributes.isSet())
        attributes = note.attributes;
    QString contentClass;
    if (attributes.contentClass.isSet())
        contentClass = attributes.contentClass;
    if (contentClass != "") {
        QLOG_DEBUG() << "Content class not empty.  Setting read-only.";
        readOnly = true;
    }
}


/* Return the formatted content */
QString NoteFormatter::getPage() {
    return this->content;
}




/* If we have search criteria, then we highlight the text matching
  those results in the note. */
//void NoteFormatter::setHighlight() {
//    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
//    if (criteria->isSearchStringSet())
//        enableHighlight = true;
//    else
//        enableHighlight = false;
//}

/* If we are here because we are viewing note history, then we
  set the flag here.  Note history is almost the same as a regular
  note, but there are some differences. */
void NoteFormatter::setNoteHistory(bool value) {
    this->noteHistory = value;
}


/* Take the ENML note and transform it into HTML that WebKit will
  not complain about */
QByteArray NoteFormatter::rebuildNoteHTML() {
    QLOG_TRACE() << "Rebuilding HTML";

    ResourceTable resTable(global.db);
    resTable.getResourceMap(hashMap, resourceMap, note.guid);

    formatError = false;
    readOnly = false;

    QWebPage page;
    QEventLoop loop;
    QLOG_TRACE() << "Before preHTMLFormat";
    QString html = "<body></body>";
    if (note.content.isSet())
        html = preHtmlFormat(note.content);
    html.replace("<en-note", "<body");
    html.replace("</en-note>", "</body>");
    QByteArray htmlPage;
    htmlPage.append(html);
    QLOG_TRACE() << "About to set content";
    page.mainFrame()->setContent(htmlPage);
    QObject::connect(&page, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));

    QLOG_TRACE() << "Starting to modify tags";
    modifyTags(page);
    QLOG_TRACE() << "Done modifying tags";
    note.content = page.mainFrame()->toHtml();
    content.clear();
    content.append(note.content);

    qint32 index = content.indexOf("<body");
    content.remove(0,index);
    content.prepend("<style>img { height:auto; width:auto; max-height:auto; max-width:100%; }</style>");
    content.prepend("<head><meta http-equiv=\"content-type\" content=\"text-html; charset=utf-8\"></head>");
    content.prepend("<html>");
    content.append("</html>");

    if (!formatError && !readOnly) {
        NotebookTable ntable(global.db);
        qint32 notebookLid = ntable.getLid(note.notebookGuid);
        if (ntable.isReadOnly(notebookLid)) {
            QLOG_DEBUG() << "Notebook is read-only.  Marking note read-only.";
            readOnly = true;
        }
    }
    if (note.active.isSet() && !note.active) {
        QLOG_DEBUG() << "Note is inactive.  Setting to read-only.";
        readOnly = true;
    }
    QLOG_TRACE() << "Done rebuiling HTML";
    return content;
}



// This is to turn the <en-media/> tags into <en-media></en-media> tags because
// QWebPage tends to miss the /> tag and it can cause some text to be missed
QString NoteFormatter::preHtmlFormat(QString note) {
    int pos;

    // Correct <br></br> because Webkit messes it up.
    QString content = note.replace("<br></br>", "<br/>");

    pos = content.indexOf("<en-media");
    while (pos != -1) {
        int endPos = content.indexOf(">", pos);
        int tagEndPos = content.indexOf("/>", pos);

        // Check the next /> end tag.  If it is before the end
        // of the current tag or if it doesn't exist then we
        // need to fix the end of the img
        if (tagEndPos == -1 || tagEndPos < endPos) {
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
    QLOG_TRACE() << "Searching for all en-media tags;";
    QWebElementCollection anchors = doc.mainFrame()->findAllElements("en-media");
    QLOG_TRACE() << "Search complete: " << anchors.toList().size();
    foreach (QWebElement enmedia, anchors) {
        if (enmedia.hasAttribute("type")) {
            QString attr = enmedia.attribute("type");
            QString hash = enmedia.attribute("hash");
            QStringList type = attr.split("/");
            if (type.size() >= 2) {
                QString appl = type[1];
                QLOG_TRACE() << "En-Media tag type: " << type[0];
                if (type[0] == "image")
                    modifyImageTags(enmedia, hash);
                else
                    modifyApplicationTags(enmedia, hash, appl);
                QLOG_TRACE() << "Type modified";
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
        QString hint = enmedia.attribute("hint");
        QString cipher = enmedia.attribute("cipher", "RC2");
        QString length = enmedia.attribute("length","64");

        enmedia.setAttribute("contentEditable","false");
        enmedia.setAttribute("src", QString("file://")+global.fileManager.getImageDirPath("encrypt.png"));
        enmedia.setAttribute("en-tag","en-crypt");
        enmedia.setAttribute("cipher", cipher);
        enmedia.setAttribute("length", length);
        enmedia.setAttribute("hint", hint);
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
        hint = hint.replace("'","&apos;");
        enmedia.setAttribute("onClick", "window.browserWindow.decryptText('crypt"+
                             QString().number(global.cryptCounter)+
                             "', '"+encryptedText+"', '"+
                             hint +"', '" +
                             cipher+ "', " +
                             length +
                             ");");
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
        if (!element.attribute("href").toLower().startsWith("http://latex.codecogs.com/gif.latex?")) {
            element.setAttribute("title", element.attribute("href"));
        } else {
            QString formula = element.attribute("href").toLower().replace("http://latex.codecogs.com/gif.latex?","");
            element.setAttribute("title", formula);
            QString resLid = element.firstChild().attribute("lid","");
            element.setAttribute("href", "latex:///"+resLid);
        }
    }

}





/* This function works the same as the addHighlight, but instead of highlighting
  text in a note, it highlights the text in an image. */
QString NoteFormatter::addImageHighlight(qint32 resLid, QString imgfile) {
    if (highlightWords.size() == 0)
        return "";

    // Get the image resource recognition data.  This tells where to highlight the image
    ResourceTable resTable(global.db);
    Resource recoResource;
    resTable.getResourceRecognition(recoResource, resLid);
    Data recognition;
    if (recoResource.recognition.isSet())
        recognition = recoResource.recognition;
    if (!recognition.size.isSet() || !recognition.body.isSet() ||
            recognition.size == 0) {
        return "";
    }

    QString filename = global.fileManager.getTmpDirPath() + QString::number(resLid) + ".png";
    // Now we have the recognition data.  We need to go through it
    QByteArray recoData;
    if (recognition.body.isSet())
        recoData = recognition.body;
    QString xml(recoData);

    // Create a transparent pixmap.  The only non transparent piece is teh
    // highlight that will be overlaid on the old image
    imgfile = imgfile.replace("file:///", "");
    QPixmap originalFile(imgfile);
    QPixmap overlayPix(originalFile.size());
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
                if (weight >= global.getMinimumRecognitionWeight()) {

                    // Check to see if this word matches something we're looking for
                    for (int k=0; k<highlightWords.size(); k++) {
                        QString searchWord = highlightWords[k].toLower();
                        if (searchWord.endsWith("*"))
                            searchWord.chop(1);
                        if (text.toLower().contains(searchWord)) {
                            p2.drawRect(x,y,w,h);
                        }
                    }
                }
            }
        }
    }

    // Paint the highlight onto the background & save over the original
    p2.setOpacity(0.4);
    p2.drawPixmap(0,0,overlayPix);
    p2.end();

    // Create the actual overlay.  We do this in two steps to avoid
    // constantly painting the same area
    QPixmap finalPix(originalFile.size());
    finalPix.fill(Qt::transparent);
    QPainter p3(&finalPix);
    p3.setBackgroundMode(Qt::TransparentMode);
    p3.setRenderHint(QPainter::Antialiasing,true);
    p3.drawPixmap(0,0,originalFile);
    p3.setOpacity(0.4);
    p3.drawPixmap(0,0,overlayPix);
    p3.end();
    finalPix.save(filename);

    return "this.src='file://"+filename+"';";
}



/* Modify an image tag.  Basically we turn it back into a picture, write out the file, and
  modify the ENML */
void NoteFormatter::modifyImageTags(QWebElement &enMedia, QString &hash) {
    QString mimetype = enMedia.attribute("type");
    qint32 resLid = 0;
    resLid = hashMap[hash];
    QString highlightString = "";
    if (resLid>0) {
        QLOG_TRACE() << "Getting resource";
        Resource r = resourceMap[resLid];
        QLOG_TRACE() << "resource retrieved";
        MimeReference ref;
        QString filename;
        ResourceAttributes attributes;
        if (r.attributes.isSet())
            attributes = r.attributes;
        if (attributes.fileName.isSet())
            filename = attributes.fileName;
        QString type = ref.getExtensionFromMime(mimetype, filename);

        Data data;
        if (r.data.isSet())
            data = r.data;
        if (data.size.isSet() && data.size > 0) {
            QString imgfile = "file:///"+global.fileManager.getDbDirPath(QString("dba/") +QString::number(resLid) +type);
            enMedia.setAttribute("src", imgfile);
            // Check if this is a LaTeX image
            ResourceAttributes attributes;
            if (r.attributes.isSet())
                attributes = r.attributes;
            QString sourceUrl = "";
            if (attributes.sourceURL.isSet())
                sourceUrl = attributes.sourceURL;
            if (sourceUrl.toLower().startsWith("http://latex.codecogs.com/gif.latex?")) {
                enMedia.appendInside("<img/>");
                QWebElement newText = enMedia.lastChild();
                enMedia.setAttribute("en-tag", "en-latex");
                newText.setAttribute("onMouseOver", "style.cursor='pointer'");
                sourceUrl.replace("http://latex.codecogs.com/gif.latex?","");
                newText.setAttribute("title", sourceUrl);
                newText.setAttribute("href", "latex:///"+QString::number(resLid));
            }
            enMedia.setAttribute("onContextMenu", "window.browserWindow.imageContextMenu('"
                                 +QString::number(resLid) +"', '"
                                 +QString::number(resLid) +type  +"');");
            highlightString = addImageHighlight(resLid, imgfile);
            if (highlightString != "")
                enMedia.setAttribute("onload", highlightString);

        }
    } else {
        resourceError = true;
        QLOG_DEBUG() << "Resource error.  Setting note to read-only.";
        readOnly = true;
    }

    // Reset the tags to something that WebKit will understand
    enMedia.setAttribute("en-tag", "en-media");
    enMedia.setPlainText("");
    enMedia.setAttribute("lid", QString::number(resLid));

    // rename the <enmedia> tag to <img>
    enMedia.setOuterXml(enMedia.toOuterXml().replace("<en-media","<img"));
    enMedia.setOuterXml(enMedia.toOuterXml().replace("</en-media>","</img>"));
}



// Modify the en-media tag into an attachment
void NoteFormatter::modifyApplicationTags(QWebElement &enmedia, QString &hash, QString appl) {
    if (appl.toLower() == "vnd.evernote.ink") {
            QLOG_DEBUG() << "Note is ink-note.  Setting to read-only.";
            inkNote = true;
            readOnly = true;
            buildInkNote(enmedia, hash);
            return;
    }

    ResourceTable resTable(global.db);
    QString contextFileName;
    qint32 resLid = resTable.getLidByHashHex(note.guid, hash);
    Resource r;
    resTable.get(r, resLid, false);
    if (!r.data.isSet())
        resourceError = true;
    else {
        // If we are running the formatter and we are not generating a thumbnail
        QString mimetype = "";
        if (r.mime.isSet())
            mimetype = r.mime;


        // Check that we don't have a locked PDF.  If we do, then disable PDF previews.
        if (mimetype == "application/pdf") {
            QString file = global.fileManager.getDbaDirPath() + QString::number(resLid) +".pdf";
            Poppler::Document *doc = Poppler::Document::load(file);
            if (doc->isLocked())
                pdfPreview = false;
        }

        if (mimetype == "application/pdf" && pdfPreview && !thumbnail) {
           modifyPdfTags(resLid, enmedia);
           return;
        }

        // If we are running the formatter so we can generate a thumbnail and it is a PDF
        if (mimetype == "application/pdf" && pdfPreview && thumbnail) {
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
        ResourceAttributes attributes;
        if (r.attributes.isSet())
            attributes = r.attributes;
        if (attributes.fileName.isSet())
            fileDetails = ref.getExtensionFromMime(r.mime, fileDetails);

        enmedia.setAttribute("href", QString("nnres:") +global.fileManager.getDbaDirPath()+QString::number(resLid)
                             +fileDetails);
        contextFileName = global.fileManager.getTmpDirPath("")+QString::number(resLid) +global.attachmentNameDelimeter + fileDetails;

        // Setup the context menu.  This is useful if we want to do a "save as" or such
        contextFileName = contextFileName.replace("\\", "/");
        enmedia.setAttribute("onContextMenu", "window.browserWindow.resourceContextMenu('" +contextFileName +"');");
        enmedia.setAttribute("en-tag", "en-media");
        enmedia.setAttribute("lid", QString::number(resLid));

        enmedia.appendInside("<img/>");
        QWebElement newText = enmedia.lastChild();

        // Build an icon of the image
        QString fileExt;
        if (attributes.fileName.isSet())
            fileExt = attributes.fileName;
        else
            fileExt = appl;
        QString fn;
        QString mime;
        if (attributes.fileName.isSet())
            fn = attributes.fileName;
        if (r.mime.isSet())
            mime = r.mime;
        fileExt = ref.getExtensionFromMime(mime, fn);
        QString icon = findIcon(resLid, r, fileExt);
        newText.setAttribute("src", "file:///"+icon);
        if (attributes.fileName.isSet())
            newText.setAttribute("title",attributes.fileName);
        newText.setAttribute("en-tag", "temporary");
        //Rename the tag to a <a> link
        enmedia.setOuterXml(enmedia.toOuterXml().replace("<en-media","<a"));
        enmedia.setOuterXml(enmedia.toOuterXml().replace("</en-media>","</a>"));
    }
}



// Build an icon for any attachments
QString NoteFormatter::findIcon(qint32 lid, Resource r, QString appl) {

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    // First get the icon for this type of file
    resourceHighlight = false;
    if (criteria->isSearchStringSet() && criteria->getSearchString() != "") {
        FilterEngine engine;
        resourceHighlight = engine.resourceContains(lid, criteria->getSearchString(), NULL);
    }

    QString fileName = global.fileManager.getDbaDirPath(QString::number(lid) +appl);
    QIcon icon;
    QFileInfo info(fileName);
    QFileIconProvider provider;
    icon = provider.icon(info);

    // Build a string name for the display
    QString displayName;
    ResourceAttributes attributes;
    if (r.attributes.isSet())
        attributes = r.attributes;
    if (attributes.fileName.isSet())
        displayName = attributes.fileName;
    else
        displayName =  appl.toUpper() +" " +QString(tr("File"));

    // Setup the painter
    QPainter p;

    // Setup the font
    QFont font; // =p.font() ;
    global.getGuiFont(font);
//    font.setFamily("Arial");
    QFontMetrics fm(font);
    int width =  fm.width(displayName);
    if (width < 40)  // steup a minimum width
        width = 40;
    width=width+50;  // Add 10 px for padding & 40 for the icon

    // Start drawing a new pixmap for  the image in the note
    QPoint textPoint(40,15);
    QPoint sizePoint(40,29);
    QPixmap pixmap(width,37);
    if (resourceHighlight) {
        pixmap.fill(Qt::yellow);
    } else
        pixmap.fill();

    p.begin(&pixmap);
    p.setFont(font);
    p.drawPixmap(QPoint(3,3), icon.pixmap(QSize(30,40)));

    // Write out the attributes of the file
    p.drawText(textPoint, displayName);

    QString unit = QString(tr("Bytes"));
    qint64 size = QFileInfo(fileName).size();
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

    ResourceTable resTable(global.db);
    qint32 resLid = resTable.getLidByHashHex(note.guid, hash);
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
}


void NoteFormatter::setHighlightText(QString text) {
    QStringList temp = text.split(" ");
    for (int i=0; i<temp.size(); i++) {
        if (temp[i].trimmed() != "")
            highlightWords.append(temp[i]);
    }
}

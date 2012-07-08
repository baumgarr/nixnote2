#include "noteformatter.h"
#include "filters/ensearch.h"
#include "sql/resourcetable.h"
#include "global.h"

#include <QFileIconProvider>
#include <QIcon>

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
    this->enableHighlight = false;
    readOnly = true;

}


/* Return the formatted content */
QString NoteFormatter::getPage() {
    return this->content;
}




/* If we have search criteria, then we highlight the text matching
  those results in the note. */
void NoteFormatter::setHighlight(EnSearch search) {
    //  If we don't have any criteria, then we don't highlight
    if (search.hilightWords.size() == 0) {
        enableHighlight = false;
    } else {
        enSearch = search;
        enableHighlight = true;
    }
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
    //QLOG_DEBUG() << "Entering NoteFormatter::rebuildNoteHTML";
    //QLOG_DEBUG() << "NoteGuid :: " << QString::fromStdString(note.guid);
    //QLOG_DEBUG() << "Note Text\n" << QString::fromStdString(note.content);




    // Run it through "tidy".  It is a program which will fix any invalid XML
    // and give us the results back through stdout.  In a perfect world this
    // wouldn't be needed, but I've seen times where the ENML is bad for some reason.
    //QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcess tidyProcess;
    tidyProcess.start("tidy -xml -raw", QIODevice::ReadWrite|QIODevice::Unbuffered);
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

    // Remove all the temporary file names
    tempFiles.clear();
    QDomDocument doc;
    doc.setContent(content);
    modifyTags(doc);

    // If we have search criteria, then do the highlighting
    if (enableHighlight)
        doc = addHighlight(doc);

    // Finish up and return the HTML to the user
    QDomElement docElem = doc.documentElement();
    docElem.setTagName("body");
    content = doc.toByteArray(3);
    int index = content.indexOf("<body");
    content.remove(0,index);
    content = "<html><head><meta http-equiv=\"content-type\" content=\"text-html; charset=utf-8\"></head>" +content +"</html>";

    //QLOG_DEBUG() << "Contents:\n***********\n\n" << content << "\n\n\n";
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
    int enMediaCount = anchors.length();
    for (int i=enMediaCount-1; i>=0; --i) {
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
    int enTodoCount = anchors.length();
    for (int i=enTodoCount-1; i>=0; i--) {
            QDomElement enmedia = anchors.at(i).toElement();
            modifyTodoTags(enmedia);
    }

    // Modify en-crypt tags
    anchors = docElem.elementsByTagName("en-crypt");
    int enCryptLen = anchors.length();
    for (int i=enCryptLen-1; i>=0; i--) {
            QDomElement enmedia(anchors.at(i).toElement());
            enmedia.setAttribute("contentEditable","false");
            enmedia.setAttribute("src", QString("file://")+global.fileManager.getImageDirPath("encrypt.png"));
            enmedia.setAttribute("en-tag","en-crypt");
            //enmedia.setAttribute("alt", enmedia.text());
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
            enmedia.setAttribute("onClick", "window.jambi.decryptText('crypt"+QString().number(global.cryptCounter)+"', '"+encryptedText+"', '"+hint+"');");
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
void NoteFormatter::addImageHighlight(int resLid, QFile &f) {
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
    QString type = enMedia.attribute("type");
    if (type.startsWith("image/")) {
        type.remove(0,6);
        if (type == "jpeg")
            type = "jpg";
        type.prepend(".");
    } else {
        type="";
    }

    ResourceTable resourceTable;
    int resLid = resourceTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    Resource r;
    if (resLid>0) {
        //QFile tfile(global.fileManager.getResDirPath(QString::number(resLid)+type));
        resourceTable.get(r, resLid);

        if (r.__isset.data && r.data.__isset.body && r.data.body.length() > 0) {
            /*tfile.open(QIODevice::WriteOnly);
            QByteArray b;
            b.append(QString::fromStdString(r.data.body));
            tfile.write(b);
            tfile.close();
        */
            // If we need to highlight some text
            //this->addImageHighlight(resLid, tfile);

            // Technically we should do a file::// to have a proper URL, but for some reason
            // QWebPage hates them & won't generate a thumbnail properly when they exist
            //enMedia.setAttribute("src", QUrl.fromLocalFile(tfile.fileName()).toString());
           // enMedia.setAttribute("src", tfile.fileName());
            enMedia.setAttribute("src", "file:///"+global.fileManager.getDbDirPath(QString("dba/") +QString::number(resLid) +type));
            // Check if this is a LaTeX image
            if (r.__isset.attributes && r.attributes.__isset.sourceURL &&
                !QString::fromStdString(r.attributes.sourceURL).toLower().startsWith("http://latex.codecogs.com/gif.latex?")) {
                    //enMedia.setAttribute("onContextMenu", "window.jambi.imageContextMenu('" +tfile.fileName()  +"');");
            } else {
                QDomElement newText(doc.createElement("a"));
                enMedia.setAttribute("en-tag", "en-latex");
                newText.setAttribute("anMouseOver", "style.cursor='hand'");
                newText.setAttribute("title", QString::fromStdString(r.attributes.sourceURL));
            //    newText.setAttribute("href", "latex://"+tfile.fileName());
                enMedia.parentNode().replaceChild(newText, enMedia);
                newText.appendChild(enMedia);
            }

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
        if (buildInkNote(doc, docElem, enmedia, hash, appl))
            return;
    }
    ResourceTable resTable;
    QString contextFileName;
    int resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    Resource r;
    resTable.get(r, resLid);
    if (!r.__isset.data)
        resourceError = true;
    else {
        QString fileDetails = "";
        if (r.__isset.attributes && r.attributes.__isset.fileName) {
            fileDetails = QString::fromStdString(r.attributes.fileName);

            if (fileDetails != "") {
                if (!noteHistory) {
                    enmedia.setAttribute("href", QString("nnres://") +QString::fromStdString(r.guid)
                                            +global.attachmentNameDelimeter +fileDetails);
                    contextFileName = global.fileManager.getTmpDirPath(QString(resLid) +global.attachmentNameDelimeter + fileDetails);
                } else {
                    enmedia.setAttribute("href", QString("nnres://") +QString::fromStdString(r.guid) + QString(note.updateSequenceNum)
                                            +global.attachmentNameDelimeter +fileDetails);
                    contextFileName = global.fileManager.getTmpDirPath(QString(resLid) + QString(note.updateSequenceNum)
                                            +global.attachmentNameDelimeter + fileDetails);
                }
            } else {
                if (!noteHistory) {
                    enmedia.setAttribute("href", "nnres://" +QString::fromStdString(r.guid) +QString(note.updateSequenceNum)
                                                            +global.attachmentNameDelimeter +appl);
                    contextFileName = global.fileManager.getTmpDirPath(QString(resLid) +QString("-")
                                                            +QString(note.updateSequenceNum)
                                                            +global.attachmentNameDelimeter + appl);
                } else {
                    enmedia.setAttribute("href", "nnres://" +QString::fromStdString(r.guid)
                                                            +global.attachmentNameDelimeter +appl);
                    contextFileName = global.fileManager.getTmpDirPath(QString(QString(resLid))
                                                            +global.attachmentNameDelimeter + appl);
                }
            }
        }

        // Setup the context menu.  This is useful if we want to do a "save as" or such
        contextFileName = contextFileName.replace("\\", "/");
        enmedia.setAttribute("onContextMenu", "window.jambi.resourceContextMenu('" +contextFileName +"');");
        enmedia.setAttribute("en-tag", "en-media");
        enmedia.setAttribute("lid", QString::number(resLid));
        enmedia.setTagName("a");
        QDomElement newText = doc.createElement("img");
        QFile tfile(contextFileName);
        tfile.open(QIODevice::WriteOnly);
        tfile.close();

        // Build an icon of the image
        QString icon = findIcon(resLid, r, appl);
        newText.setAttribute("src", "file:///"+icon);
        newText.setAttribute("title", fileDetails);
        enmedia.removeChild(enmedia.firstChild());
        enmedia.appendChild(newText);
        QLOG_TRACE() << "Leaving NeverNote.modifyApplicationTags";
    }
}



// Build an icon for any attachments
QString NoteFormatter::findIcon(int lid, Resource r, QString appl) {

    // First get the icon for this type of file
    QString fileName = global.fileManager.getDbaDirPath(QString::number(lid) +QString(".") +appl);
    QIcon icon = QFileIconProvider().icon(QFileInfo(fileName));

    // Start drawing a new pixmap for  the image in the note
    QPoint textPoint(40,15);
    QPoint sizePoint(40,29);
    QPixmap pixmap(90,37);
    pixmap.fill();

    // Begin painting
    QPainter p;
    p.begin(&pixmap);
    p.drawPixmap(QPoint(3,3), icon.pixmap(QSize(30,40)));

    // Setup the font
    QFont font=p.font() ;
    font.setPointSize ( 8 );
    font.setFamily("Arial");
    p.setFont(font);

    // Write out the attributes of the file
    if (r.__isset.attributes && r.attributes.__isset.fileName)
        p.drawText(textPoint, QString::fromStdString(r.attributes.fileName));
    else
        p.drawText(textPoint, appl.toUpper() +" " +QString(tr("File")));

    QString unit = QString(tr("Bytes"));
    int size = QFileInfo(fileName).size();
    if (size > 1024) {
        size = size/1024;
        unit = QString(tr("KB"));
    }
    if (size > 1024) {
        size = size/1024;
        unit= QString("MB");
    }
    p.drawText(sizePoint, QString::number(size).trimmed() +" " +unit);
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
bool NoteFormatter::buildInkNote(QDomDocument &doc, QDomElement &docElem, QDomElement &enmedia, QDomAttr &hash, QString appl) {
    ResourceTable resTable;
    int resLid = resTable.getLidByHashHex(QString::fromStdString(note.guid), hash.value());
    Resource r;
    resTable.get(r, resLid);

    // If we can't find the resource, then fall back to the old method.  We'll return & show
    // an error later
    if (!r.__isset.data)
        return false;

    // If there isn't some type of error, continue on.
    if (!resourceError) {

        // Get a list of images in the database.  We'll use these to bulid the page.
       QByteArray data;
       resTable.getInkNote(data, resLid);

        // If no pictures are found, go back to & just show the icon
        if (data.size() == 0)
            return false;

        // We have pictures, so append them to the page.  This really isn't proper since
        // we leave the en-media tag in place, but since we can't edit the page it doesn't
        // hurt anything.
        QFile f(global.fileManager.getTmpDirPath(QString(resLid)+"-inknote" +".png"));
        f.open(QFile::WriteOnly);
        f.write(data);
        f.close();

        QDomElement newImage(doc.createElement("img"));
        //newImage.setAttribute("src", QUrl.fromLocalFile(f.fileName()));
        newImage.setAttribute("src", "InkNote");
        enmedia.appendChild(newImage);
        return true;
    }
    return false;
}

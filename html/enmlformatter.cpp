#include "enmlformatter.h"

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
EnmlFormatter::EnmlFormatter(QObject *parent) :
    QObject(parent)
{
    //doc = new QDomDocument();
    formattingError = false;
}

/* Return the formatted content */
QString EnmlFormatter::getEnml() {
    return this->content;
}


void EnmlFormatter::setHtml(QString h) {
    content.clear();
    content.append(h);
}

/* Take the ENML note and transform it into HTML that WebKit will
  not complain about */
QByteArray EnmlFormatter::rebuildNoteEnml() {
    resources.clear();


//    // Run it through "tidy".  It is a program which will fix any invalid HTML
//    // and give us the results back through stdout.  In a perfect world this
//    // wouldn't be needed, but WebKit doesn't always give back good HTML.

    QProcess tidyProcess;
    tidyProcess.start("tidy -raw -q -asxhtml -utf8 ", QIODevice::ReadWrite|QIODevice::Unbuffered);
    QLOG_DEBUG() << "Starting tidy " << tidyProcess.waitForStarted();
    tidyProcess.write(content);
    tidyProcess.closeWriteChannel();
    QLOG_DEBUG() << "Stopping tidy " << tidyProcess.waitForFinished() << " Return Code: " << tidyProcess.state();
    QLOG_DEBUG() << "Tidy Errors:" << tidyProcess.readAllStandardError();
//    QLOG_DEBUG() << "Tidy Stdout:" << tidyProcess.readAllStandardOutput();
    content = tidyProcess.readAllStandardOutput();


//    // If we have search criteria, then do the highlighting
//    removeHighlight(doc);

//    // Finish up and return the HTML to the user
    qint32 index = content.indexOf("<body");
    index = content.indexOf(">", index)+1;
    content.remove(0,index);
    index = content.indexOf("</body");
    content.truncate(index);
    QByteArray b;
    b.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    b.append("<!DOCTYPE en-note SYSTEM 'http://xml.evernote.com/pub/enml2.dtd'>");
    b.append("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\" >");
    b.append(content);
    b.append("</en-note>");
    content.clear();
    content = b;

    // Remove all the temporary file names
    doc.setContent(content);
    scanTags();

    return content;
}


// Start looking through the tree.
void EnmlFormatter::scanTags() {
    if (doc.hasChildNodes()) {
        parseNodes(doc.childNodes());
    }
    QLOG_DEBUG() << "Doc:" << doc.toString();
    return;
}


// Parse the children
void EnmlFormatter::parseNodes(const QDomNodeList &nodes) {
    for (qint32 i=0; i<nodes.size(); i++) {
        if (nodes.at(i).hasChildNodes())
            parseNodes(nodes.at(i).childNodes());
        fixNode(nodes.at(i));
    }
}


// Fix the contents of the node back to ENML.
void EnmlFormatter::fixNode(const QDomNode &node) {
    QDomElement scanChecked = node.toElement();
    if (scanChecked.hasAttribute("checked")) {
        if (scanChecked.attribute("checked").toLower() != "true")
            scanChecked.setAttribute("checked", "false");
    }
    if (node.nodeName().toLower() == "#comment" || node.nodeName().toLower() == "script") {
        node.parentNode().removeChild(node);
    }
    if (node.nodeName().toLower() == "input") {
        QDomElement e = node.toElement();
        e.setTagName("en-todo");
        QString value = e.attribute("value");
        if (value.trimmed() == "")
            value = "false";
        e.removeAttribute("value");
        e.removeAttribute("unchecked");
        e.setAttribute("checked", value);
        e.removeAttribute("onclick");
        e.removeAttribute("onmouseover");
        e.removeAttribute("type");
    }

    if (node.nodeName().toLower() == "a") {
        fixLinkNode(node);
    }

    // Restore image resources
    if (node.nodeName().toLower() == "img") {
        QDomElement e = node.toElement();
        QString enType = e.attribute("en-tag");

        // Check if we have an en-crypt tag.  Change it from an img to en-crypt
        if (enType.toLower() == "en-crypt") {

            QString encrypted = e.attribute("alt");

            QDomText crypt = doc.createTextNode(encrypted);
            e.appendChild(crypt);

            e.removeAttribute("v:shapes");
            e.removeAttribute("en-tag");
            e.removeAttribute("contenteditable");
            e.removeAttribute("alt");
            e.removeAttribute("src");
            e.removeAttribute("id");
            e.removeAttribute("onclick");
            e.removeAttribute("onmouseover");
            e.setTagName("en-crypt");
            e.parentNode().removeChild(e);
            return;
        }

        // Check if we have a temporary image.  If so, remove it
        if (enType.toLower() ==  "temporary") {;
            QDomNode parent = e.parentNode();
            parent.removeChild(e);
            return;
        }

        // Check if we have a LaTeX image.  Remove the parent link tag
        if (enType.toLower() ==  "en-latex") {
            enType = "en-media";
            QDomNode parent = e.parentNode();
            parent.removeChild(e);
            parent.parentNode().replaceChild(e, parent);
        }


        // If we've gotten this far, we have an en-media tag
        e.setTagName(enType);
        QLOG_DEBUG() << enType;
        QLOG_DEBUG() << doc.toString();
        resources.append(e.attribute("lid").toInt());
        e.removeAttribute("guid");
        e.removeAttribute("src");
        e.removeAttribute("en-new");
        e.removeAttribute("en-tag");
    }

    // Tags like <ul><ul><li>1</li></ul></ul> are technically valid, but Evernote
    // expects that a <ul> tag only has a <li>, so we will need to change them
    // to this:  <ul><li><ul><li>1</li></ul></li></ul>
//    if (node.nodeName().equalsIgnoreCase("ul")) {
//        QDomNode firstChild = node.firstChild();
//        QDomElement childElement = firstChild.toElement();
//        if (childElement.nodeName().equalsIgnoreCase("ul")) {
//            QDomElement newElement = doc.createElement("li");
//            node.insertBefore(newElement, firstChild);
//            node.removeChild(firstChild);
//            newElement.appendChild(firstChild);
//        }
//    }

    if (node.nodeName().toLower() == "en-hilight") {
        QDomElement e = node.toElement();
        QDomText newText = doc.createTextNode(e.text());
        e.parentNode().replaceChild(newText,e);
    }

    if (node.nodeName().toLower() == "span") {
        QDomElement e = node.toElement();
        if (e.attribute("class").toLower() == "en-hilight" || e.attribute("class").toLower() == "en-spell") {
            QDomText newText = doc.createTextNode(e.text());
            e.parentNode().replaceChild(newText,e);
        }
        if (e.attribute("pdfnavigationtable") == "true") {
            node.parentNode().removeChild(node);
        }
    }

    // Fix up encryption tag
    if (node.nodeName().toLower() == "en-crypt-temp") {
        QDomElement e = node.toElement();
        e.setTagName("en-crypt");
        QString crypt = e.attribute("value");
        e.removeAttribute("value");
        QDomText cryptValue = doc.createTextNode(crypt);
        e.appendChild(cryptValue);
    }
}



QDomNode EnmlFormatter::fixLinkNode(const QDomNode &node) {
    QDomElement e = node.toElement();
    QString enTag = e.attribute("en-tag");
    if (enTag.toLower() == "en-media") {
        e.setTagName("en-media");
        e.removeAttribute("en-type");
        e.removeAttribute("en-tag");
        e.removeAttribute("en-new");
        resources.append(e.attribute("lid").toInt());
        e.removeAttribute("href");
        e.removeAttribute("guid");
        e.setNodeValue("");
        e.removeChild(e.firstChildElement());
    }
    return e;
}

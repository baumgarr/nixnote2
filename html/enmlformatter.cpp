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

#include "enmlformatter.h"

#include "filters/ensearch.h"
#include "sql/resourcetable.h"
#include "global.h"

#include <QFileIconProvider>
#include <QIcon>
#include <QMessageBox>

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

        //QLOG_DEBUG() << "Before:\n"<< content<<"\n\n";

//    // Run it through "tidy".  It is a program which will fix any invalid HTML
//    // and give us the results back through stdout.  In a perfect world this
//    // wouldn't be needed, but WebKit doesn't always give back good HTML.

    QProcess tidyProcess;
    tidyProcess.start("tidy -raw -q -asxhtml -utf8 ", QIODevice::ReadWrite|QIODevice::Unbuffered);
    QLOG_DEBUG() << "Starting tidy " << tidyProcess.waitForStarted();
    tidyProcess.waitForStarted();
    tidyProcess.write(content);
    tidyProcess.closeWriteChannel();
    tidyProcess.waitForFinished();
    QLOG_DEBUG() << "Stopping tidy " << tidyProcess.waitForFinished() << " Return Code: " << tidyProcess.state();
    QLOG_DEBUG() << "Tidy Errors:" << tidyProcess.readAllStandardError();

    //QLOG_DEBUG() << "\n\nafter\n" << content <<"\n\n";
    if (content == "") {
        formattingError = true;
        return "";
    }

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
    //QLOG_DEBUG() << "Doc:" << doc.toString();
    content = doc.toByteArray();
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
    if (node.nodeName().toLower() == "#comment" || node.nodeName().toLower() == "script") {
        node.parentNode().removeChild(node);
    }
    if (node.nodeName().toLower() == "input") {
        QDomElement e = node.toElement();
        e.setTagName("en-todo");

        if (e.hasAttribute("checked"))
            e.setAttribute("checked", "true");
        else
            e.removeAttribute("checked");

        cleanupElementAttributes(e);
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
        resources.append(e.attribute("lid").toInt());

        cleanupElementAttributes(e);
    }

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

    // we may have a good plugin
    if (node.nodeName().toLower() == "object") {
        QDomElement e = node.toElement();
        QString type = e.attribute("type", "");
        if (type == "application/pdf") {
            qint32 lid = e.attribute("lid", "0").toInt();
            if (lid>0)
                resources.append(lid);

            e.setTagName("en-media");
            e.removeAttribute("lid");
            e.removeAttribute("height");
            e.removeAttribute("width");
        }

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



void EnmlFormatter::cleanupElementAttributes(QDomElement &e) {
    QDomNamedNodeMap attributeMap = e.attributes();
    for (int i=attributeMap.size()-1; i>=0; i--) {
        QString name = attributeMap.item(i).nodeName();
        if (!isAttributeValid(name))
            e.removeAttribute(name);
    }
}

bool EnmlFormatter::isAttributeValid(QString attribute) {
    attribute = attribute.toLower().trimmed();
    if (attribute.startsWith("on")) return false;
    if (attribute == "id") return false;
    if (attribute == "class") return false;
    if (attribute == "accesskey") return false;
    if (attribute == "data") return false;
    if (attribute == "dynsrc") return false;
    if (attribute == "tabindex") return false;

    // These are things that are NixNote specific
    if (attribute == "en-tag") return false;
    if (attribute == "src") return false;
    if (attribute == "en-new") return false;
    if (attribute == "guid") return false;
    if (attribute == "lid") return false;
    return true;
}


bool EnmlFormatter::isElementValid(QString element) {
    element = element.trimmed().toLower();
    if (element == "a") return true;
    if (element == "abbr") return true;
    if (element == "acronym") return true;
    if (element == "address") return true;
    if (element == "area") return true;
    if (element == "b") return true;
    if (element == "bdo") return true;
    if (element == "big") return true;
    if (element == "blockquote") return true;
    if (element == "br") return true;
    if (element == "caption") return true;
    if (element == "center") return true;
    if (element == "cite") return true;
    if (element == "code") return true;
    if (element == "col") return true;
    if (element == "colgroup") return true;
    if (element == "dd") return true;
    if (element == "del") return true;
    if (element == "dfn") return true;
    if (element == "div") return true;
    if (element == "dl") return true;
    if (element == "dt") return true;
    if (element == "em") return true;
    if (element == "en-media") return true;
    if (element == "en-crypt") return true;
    if (element == "en-todo") return true;
    if (element == "en-note") return true;
    if (element == "font") return true;
    if (element == "h1") return true;
    if (element == "h1") return true;
    if (element == "h1") return true;
    if (element == "h2") return true;
    if (element == "h3") return true;
    if (element == "h4") return true;
    if (element == "h5") return true;
    if (element == "h6") return true;
    if (element == "hr") return true;
    if (element == "i") return true;
    if (element == "img") return true;
    if (element == "ins") return true;
    if (element == "kbd") return true;
    if (element == "li") return true;
    if (element == "map") return true;
    if (element == "ol") return true;
    if (element == "p") return true;
    if (element == "pre") return true;
    if (element == "q") return true;
    if (element == "s") return true;
    if (element == "samp") return true;
    if (element == "small") return true;
    if (element == "span") return true;
    if (element == "strike") return true;
    if (element == "strong") return true;
    if (element == "sub") return true;
    if (element == "sup") return true;
    if (element == "table") return true;
    if (element == "tbody") return true;
    if (element == "td") return true;
    if (element == "tfoot") return true;
    if (element == "th") return true;
    if (element == "thread") return true;
    if (element == "title") return true;
    if (element == "tr") return true;
    if (element == "tt") return true;
    if (element == "u") return true;
    if (element == "ul") return true;
    if (element == "var") return true;
    if (element == "xmp") return true;

    return false;

}

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

#include "tageditor.h"
#include "global.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QHideEvent>
#include "tagviewer.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"

extern Global global;

//*******************************************************
//* Constructor
//*******************************************************
TagEditor::TagEditor(QWidget *parent) :
    QWidget(parent)
{
    QLOG_TRACE_IN() << typeid(*this).name();
    layout = new FlowLayout(this);
    layout->addWidget(&tagIcon);
    setLayout(layout);
    currentLid = 0;
    newEditorHasFocus = false;

    QPixmap pix = global.getPixmapResource(":tagIcon");
    tagIcon.setPixmap(pix);

    for (int i=0; i<MAX_TAGS; i++) {
        tags[i].setVisible(false);
        layout->addWidget(&tags[i]);
        connect(&tags[i],SIGNAL(closeClicked(QString)), this, SLOT(removeTag(QString)));
    }

    connect(&newTag, SIGNAL(focussed(bool)), this, SLOT(newTagFocusLost(bool)));
    connect(&newTag, SIGNAL(tabPressed()), this, SLOT(newTagTabPressed()));
    tagNames.clear();
    layout->addWidget(&newTag);
    //delete pix;
    account = 0;
    hide();
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//********************************************************
//* Set the current note lid
//********************************************************
void TagEditor::setCurrentLid(qint32 l) {
    QLOG_TRACE_IN() << typeid(*this).name();
    currentLid = l;
    QLOG_TRACE_OUT() << typeid(*this).name();
}

//*******************************************************
//* The new tag editor has lost focus.  Check for a new tag
//*******************************************************
void TagEditor::newTagFocusLost(bool focus) {
    QLOG_TRACE_IN() << typeid(*this).name();
    if (focus) {
        newEditorHasFocus = true;
        QLOG_TRACE_OUT() << typeid(*this).name();
        return;
    }
    newEditorHasFocus = false;
    checkNewTagEditor();
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Check the new tag editor for a new tag.
//*******************************************************
bool TagEditor::checkNewTagEditor() {
    QLOG_TRACE_IN() << typeid(*this).name();
    if (newTag.getText().trimmed() != "") {
        QString name = newTag.getText().trimmed();
        for (qint32 i=0; i<tagNames.size(); i++) {
            if (tagNames.at(i).trimmed().toLower() == name.trimmed().toLower()) {
                newTag.resetText();
                QLOG_TRACE_OUT() << typeid(*this).name();
                return false;
            }
        }
        addTag(name);
        newTag.resetText();
        QLOG_TRACE_OUT() << typeid(*this).name();
        return true;
    }
    QLOG_TRACE_OUT() << typeid(*this).name();
    return false;
}


//*******************************************************
//* A user has added a new tag
//*******************************************************
void TagEditor::addTag(QString text) {
    QLOG_TRACE_IN() << typeid(*this).name();
    qint32 tagLid;
    NoteTable noteTable(global.db);
    TagTable tagTable(global.db);
    tagLid = tagTable.findByName(text, account);

    if (tagLid <=0 && account !=0) {
        QLOG_TRACE_OUT() << typeid(*this).name();
        return;
    }

    // First blank out the old tags.
    emptyTags();

    // Add the new tag to the list & sort
    tagNames << text;
    newTag.addTag(text);
    loadTags();
    Tag newTag;
    if (tagLid <=0) {
        QUuid uuid;
        newTag.name = text;
        QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
        newTag.guid = newGuid;
        tagTable.add(0, newTag, true, account);
        tagLid = tagTable.getLid(newTag.guid);
        emit(newTagCreated(tagLid));
    }
    noteTable.addTag(currentLid, tagLid, true);
    emit(tagsUpdated());
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Load the tags for the current note.
//*******************************************************
void TagEditor::loadTags() {
    QLOG_TRACE_IN() << typeid(*this).name();
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);

    for (qint32 i=0; i<tagNames.size(); i++) {
        tags[i].setText(tagNames[i]);
        tags[i].setVisible(true);
        tags[i].resize();
    }
    layout->invalidate();
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Reload the tags after a sync
//*******************************************************
void TagEditor::reloadTags() {
    QLOG_TRACE_IN() << typeid(*this).name();
    NoteTable noteTable(global.db);
    Note n;
    noteTable.get(n, currentLid, false, false);
    QStringList names;
    QList<QString> tagNames;
    if (n.tagNames.isSet())
        tagNames = n.tagNames;
    for (int i=0; i<tagNames.size(); i++) {
        names << tagNames[i];
    }
    setTags(names);
    QLOG_TRACE_OUT() << typeid(*this).name();
}

//*******************************************************
//* Signal received that a tag name has been changed
//*******************************************************
void TagEditor::tagRenamed(qint32 lid, QString oldName, QString newName) {
    Q_UNUSED(lid);  // suppress unuesd
    QLOG_TRACE_IN() << typeid(*this).name();
    tagNames.removeOne(oldName);
    tagNames << newName;
    loadTags();
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* Set the tags for the current note
//*******************************************************
void TagEditor::setTags(QStringList s) {

    QLOG_TRACE_IN() << typeid(*this).name();
    // First blank out the old tags.
    emptyTags();
    tagNames.clear();

    // Add the new tag to the list & sort
    for (qint32 i =0; i<s.size(); i++)
        tagNames << s[i];
    loadTags();
    newTag.setTags(s);
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Get all current tags
//*******************************************************
void TagEditor::getTags(QStringList &names) {
    QLOG_TRACE_IN() << typeid(*this).name();
    names.clear();
    names = tagNames;
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Remove a tag for a note
//*******************************************************
void TagEditor::removeTag(QString text) {
    QLOG_TRACE_IN() << typeid(*this).name();
    bool found = false;
    qint32 j=-1;
    for (qint32 i=0; i<tagNames.size(); i++) {
        if (tags[i].text().toLower() == text.toLower()) {
            found = true;
            TagTable tagTable(global.db);
            QString name = tags[i].text();
            qint32 lid = tagTable.findByName(name, account);
            if (lid>0) {
                NoteTable noteTable(global.db);
                noteTable.removeTag(currentLid, lid, true);
            }
            j = i;
        }
        if (found && i<tagNames.size()-1) {
            tags[i].setText(tags[i+1].text());
            tags[i].setMinimumWidth(tags[i+1].minimumWidth());
        }

    }

    // If we found a match we need to chop off the end
    if (j>=0) {
        tags[tagNames.size()-1].clear();
        tags[tagNames.size()-1].setVisible(false);
        tags[tagNames.size()-1].setMinimumWidth(0);
        tagNames.removeAt(j);
    }

    emit(tagsUpdated());
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Empty out all tags
//*******************************************************
void TagEditor::clear() {
    QLOG_TRACE_IN() << typeid(*this).name();
    emptyTags();
    tagNames.clear();

    layout->invalidate();
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Hide this window
//*******************************************************
void TagEditor::hideEvent(QHideEvent* event) {
    QLOG_TRACE_IN() << typeid(*this).name();
    Q_UNUSED(event);  // suppress unused
    tagIcon.hide();
    newTag.hide();
    for (qint32 i=0; i<MAX_TAGS; i++) {
        tags[i].hide();
    }
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Show this window
//*******************************************************
void TagEditor::showEvent(QShowEvent* event) {
    QLOG_TRACE_IN() << typeid(*this).name();
    Q_UNUSED(event);  // suppress unused
    tagIcon.show();
    newTag.show();
    for (qint32 i=0; i<tagNames.size(); i++)
        tags[i].show();
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Empty out the tags window
//*******************************************************
void TagEditor::emptyTags() {
    QLOG_TRACE_IN() << typeid(*this).name();
    for (qint32 i=MAX_TAGS-1; i>=0; i--) {
        tags[i].clear();
        tags[i].setMinimumWidth(0);
        tags[i].setVisible(false);
    }
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* The user pressed "tab".  If it is a valid tab we
//* add it and reset focus back to this window
//*******************************************************
void TagEditor::newTagTabPressed() {
   QLOG_TRACE_IN() << typeid(*this).name();
   if (checkNewTagEditor())
       newTag.setFocus();
   QLOG_TRACE_OUT() << typeid(*this).name();
}


void TagEditor::setAccount(qint32 a) {
    QLOG_TRACE_IN() << typeid(*this).name();
    account = a;
    this->newTag.setAccount(account);
    QLOG_TRACE_OUT() << typeid(*this).name();
}



void TagEditor::reloadIcons() {
    QLOG_TRACE_IN() << typeid(*this).name();
    tagIcon.setPixmap(global.getPixmapResource(":tagIcon"));
    QLOG_TRACE_OUT() << typeid(*this).name();
}

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
    layout = new FlowLayout(this);
    layout->addWidget(&tagIcon);
    setLayout(layout);

    QPixmap *pix = new QPixmap(":tag.png");
    tagIcon.setPixmap(*pix);

    for (int i=0; i<MAX_TAGS; i++) {
        tags[i].setVisible(false);
        layout->addWidget(&tags[i]);
        connect(&tags[i],SIGNAL(closeClicked(QString)), this, SLOT(removeTag(QString)));
    }

    connect(&newTag, SIGNAL(focussed(bool)), this, SLOT(newTagFocusLost(bool)));
    connect(&newTag, SIGNAL(tabPressed()), this, SLOT(newTagTabPressed()));
    tagNames.clear();
    layout->addWidget(&newTag);
    delete pix;
    account = 0;
    hide();
}


//********************************************************
//* Set the current note lid
//********************************************************
void TagEditor::setCurrentLid(qint32 l) {
    currentLid = l;
}

//*******************************************************
//* The new tag editor has lost focus.  Check for a new tag
//*******************************************************
void TagEditor::newTagFocusLost(bool focus) {
    if (focus) {
        newEditorHasFocus = true;
        return;
    }
    newEditorHasFocus = false;
    checkNewTagEditor();
}



//*******************************************************
//* Check the new tag editor for a new tag.
//*******************************************************
bool TagEditor::checkNewTagEditor() {
    if (newTag.getText().trimmed() != "") {
        QString name = newTag.getText().trimmed();
        for (qint32 i=0; i<tagNames.size(); i++) {
            if (tagNames.at(i).trimmed().toLower() == name.trimmed().toLower()) {
                newTag.resetText();
                return false;
            }
        }
        addTag(name);
        newTag.resetText();
        return true;
    }
    return false;
}


//*******************************************************
//* A user has added a new tag
//*******************************************************
void TagEditor::addTag(QString text) {

    // First blank out the old tags.
    emptyTags();

    // Add the new tag to the list & sort
    tagNames << text;
    newTag.addTag(text);
    loadTags();
    NoteTable noteTable(global.db);
    TagTable tagTable(global.db);
    qint32 tagLid;
    Tag newTag;
    tagLid = tagTable.findByName(text, account);
    if (tagLid <=0) {
        QUuid uuid;
        newTag.name = text.toStdString();
        QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
        newTag.guid = newGuid.toStdString();
        newTag.__isset.name = true;
        newTag.__isset.guid = true;
        tagTable.add(0, newTag, true, account);
        tagLid = tagTable.getLid(newTag.guid);
        emit(newTagCreated(tagLid));
    }
    noteTable.addTag(currentLid, tagLid, true);
    emit(tagsUpdated());
}



//*******************************************************
//* Load the tags for the current note.
//*******************************************************
void TagEditor::loadTags() {
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);

    for (qint32 i=0; i<tagNames.size(); i++) {
        tags[i].setText(tagNames[i]);
        tags[i].setVisible(true);
        tags[i].resize();
    }
    layout->invalidate();
}



//*******************************************************
//* Reload the tags after a sync
//*******************************************************
void TagEditor::reloadTags() {
    NoteTable noteTable(global.db);
    Note n;
    noteTable.get(n, currentLid, false,false);
    QStringList names;
    for (unsigned int i=0; i<n.tagNames.size(); i++) {
        names << QString::fromStdString(n.tagNames[i]);
    }
    setTags(names);
}

//*******************************************************
//* Signal received that a tag name has been changed
//*******************************************************
void TagEditor::tagRenamed(qint32 lid, QString oldName, QString newName) {
    lid=lid;  // suppress unuesd
    tagNames.removeOne(oldName);
    tagNames << newName;
    loadTags();
}


//*******************************************************
//* Set the tags for the current note
//*******************************************************
void TagEditor::setTags(QStringList s) {

    // First blank out the old tags.
    emptyTags();
    tagNames.clear();

    // Add the new tag to the list & sort
    for (qint32 i =0; i<s.size(); i++)
        tagNames << s[i];
    loadTags();
    newTag.setTags(s);
}


//*******************************************************
//* Remove a tag for a note
//*******************************************************
void TagEditor::removeTag(QString text) {
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
}



//*******************************************************
//* Empty out all tags
//*******************************************************
void TagEditor::clear() {
    emptyTags();
    tagNames.clear();

    layout->invalidate();
}



//*******************************************************
//* Hide this window
//*******************************************************
void TagEditor::hideEvent(QHideEvent* event) {
    event=event;  // suppress unused
    tagIcon.hide();
    newTag.hide();
    for (qint32 i=0; i<MAX_TAGS; i++) {
        tags[i].hide();
    }
}



//*******************************************************
//* Show this window
//*******************************************************
void TagEditor::showEvent(QShowEvent* event) {
    event=event;  // suppress unused
    tagIcon.show();
    newTag.show();
    for (qint32 i=0; i<tagNames.size(); i++)
        tags[i].show();
}



//*******************************************************
//* Empty out the tags window
//*******************************************************
void TagEditor::emptyTags() {
    for (qint32 i=MAX_TAGS-1; i>=0; i--) {
        tags[i].clear();
        tags[i].setMinimumWidth(0);
        tags[i].setVisible(false);
    }
}



//*******************************************************
//* The user pressed "tab".  If it is a valid tab we
//* add it and reset focus back to this window
//*******************************************************
void TagEditor::newTagTabPressed() {
   if (checkNewTagEditor())
       newTag.setFocus();
}


void TagEditor::setAccount(qint32 a) {
    account = a;
}

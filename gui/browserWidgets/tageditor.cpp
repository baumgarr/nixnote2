#include "tageditor.h"
#include "global.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QHideEvent>
#include "tagviewer.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"


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
    tagNames.empty();
    layout->addWidget(&newTag);
    hide();
}


//********************************************************
//* Set the current note lid
//********************************************************
void TagEditor::setCurrentLid(int l) {
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
        for (int i=0; i<tagNames.size(); i++) {
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
//* A user has added o new tag
//*******************************************************
void TagEditor::addTag(QString text) {

    // First blank out the old tags.
    emptyTags();

    // Add the new tag to the list & sort
    tagNames << text;
    newTag.addTag(text);
    loadTags();
    NoteTable noteTable;
    TagTable tagTable;
    int tagLid;
    Tag newTag;
    tagLid = tagTable.findByName(text);
    if (tagLid <=0) {
        QUuid uuid;
        newTag.name = text.toStdString();
        QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
        newTag.guid = newGuid.toStdString();
        newTag.__isset.name = true;
        newTag.__isset.guid = true;
        tagTable.add(0, newTag, true);
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

    for (int i=0; i<tagNames.size(); i++) {
        tags[i].setText(tagNames[i]);
        tags[i].setVisible(true);
        tags[i].resize();
    }
    layout->invalidate();
}


//*******************************************************
//* Set the tags for the current note
//*******************************************************
void TagEditor::setTags(QStringList s) {

    // First blank out the old tags.
    emptyTags();

    // Add the new tag to the list & sort
    for (int i =0; i<s.size(); i++)
        tagNames << s[i];
    loadTags();

    newTag.setTags(s);
}


//*******************************************************
//* Remove a tag for a note
//*******************************************************
void TagEditor::removeTag(QString text) {
    bool found = false;
    int j=-1;
    for (int i=0; i<tagNames.size(); i++) {
        if (tags[i].text().toLower() == text.toLower()) {
            found = true;
            TagTable tagTable;
            NoteTable noteTable;
            int tagLid = tagTable.findByName(text);
            noteTable.removeTag(currentLid, tagLid, true);
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
    tagIcon.hide();
    newTag.hide();
    for (int i=0; i<MAX_TAGS; i++) {
        tags[i].hide();
    }
}



//*******************************************************
//* Show this window
//*******************************************************
void TagEditor::showEvent(QShowEvent* event) {
    tagIcon.show();
    newTag.show();
    for (int i=0; i<tagNames.size(); i++)
        tags[i].show();
}



//*******************************************************
//* Empty out the tags window
//*******************************************************
void TagEditor::emptyTags() {
    for (int i=MAX_TAGS-1; i>=0; i--) {
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



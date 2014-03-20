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

#include "tageditornewtag.h"
#include "global.h"
#include "sql/tagtable.h"

#include <QKeyEvent>
#include <QListView>
#include <QStringListModel>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>


extern Global global;


//****************************************************
//* Constructor
//****************************************************
TagEditorNewTag::TagEditorNewTag(QWidget *parent) :
    QLineEdit(parent)
{
    account = 0;
    this->setCursor(Qt::PointingHandCursor);
    // Setup the note title editor
    QPalette pal;
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    QFont f = font();
    f.setPointSize(8);
    setFont(f);

    inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} ";
    activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    this->setStyleSheet(inactiveColor);

    this->setPlaceholderText(tr("Click to add tag..."));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textModified(QString)));

//    connect(this, SIGNAL(focussed(bool)), this, SLOT(gainedFocus(bool)));
    completer = NULL;
    loadCompleter();
    hide();
}


//***********************************************************
//* Load the completer with the list of valid tag names
//***********************************************************
void TagEditorNewTag::loadCompleter() {
    if (completer != NULL) {
        delete completer;
    }
    QList<int> tagList;
    TagTable tagTable(global.db);
    QStringList tagNames;
    tagTable.getAll(tagList);
    for (qint32 i=0; i<tagList.size(); i++) {
        Tag t;
        QString guid;
        tagTable.getGuid(guid, tagList[i]);
        tagTable.get(t, guid);
        QString name = QString::fromStdString(t.name);
        qint32 tagAccount = tagTable.owningAccount(tagList[i]);
        if (!currentTags.contains(name) && tagAccount == account)
            tagNames << name;
    }
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);
    completer = new QCompleter(tagNames, this);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(completer);
}



//******************************************************
//* Set the color whe the editor has a good focus
//******************************************************
void TagEditorNewTag::setActiveColor() {
    setStyleSheet(activeColor);
}


//*******************************************************
//* Focus event.  The editor has gained focus
//*******************************************************
void TagEditorNewTag::focusInEvent(QFocusEvent *e)
{
  this->setCursor(Qt::ArrowCursor);
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  emit(focussed(true));
}


//*******************************************************
//* Focus event.  The editor has lost focus
//*******************************************************
void TagEditorNewTag::focusOutEvent(QFocusEvent *e)
{
  this->setCursor(Qt::PointingHandCursor);
  QLineEdit::focusOutEvent(e);
  setStyleSheet(inactiveColor);
  emit(focussed(false));
}


//*******************************************************
//* The current text has changed
//*******************************************************
void TagEditorNewTag::textModified(QString text) {
    if (this->hasFocus())
        return;
    this->blockSignals(true);
    setText(text);
    this->blockSignals(false);
}


//*******************************************************
//* Event listener for when the editor gains focus
//*******************************************************
//void TagEditorNewTag::gainedFocus(bool focus) {
//    if (focus && text() == defaultText)
//        textModified("");
//}



//*******************************************************
//* Get the current text.  If the text is the default
//* text, the n return an empty string.
//*******************************************************
QString TagEditorNewTag::getText() {
    return text().trimmed();
}


//*******************************************************
//* Set the list of valid tags for the completer
//*******************************************************
void TagEditorNewTag::setTags(QStringList s) {
    currentTags = s;
    loadCompleter();
}


//*******************************************************
//* Add a new tag to the completer list
//*******************************************************
void TagEditorNewTag::addTag(QString s) {
    currentTags.append(s);
    loadCompleter();
}



//*******************************************************
//* Override the event loop.  If the user presses "TAB"
//* and there is a valid tag entered, we signal the parent
//* that a new tag is needed.
//*******************************************************
bool  TagEditorNewTag::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = (QKeyEvent*)event;
        if (ke->key() == Qt::Key_Tab) {
            if (getText().trimmed() != "") {
                emit(tabPressed());
                return true;
            }
            return QLineEdit::event(event);
        }
    }
    return QLineEdit::event(event);
}



//*******************************************************
//* Reset the tag editor to the default text
//*******************************************************
void TagEditorNewTag::resetText() {
    blockSignals(true);
    setText("");
    blockSignals(false);
}


void TagEditorNewTag::notebookSelectionChanged(qint32 notebook) {
    account = notebook;
    loadCompleter();
}

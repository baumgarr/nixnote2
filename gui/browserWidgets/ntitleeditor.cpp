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


#include "ntitleeditor.h"
#include "sql/notetable.h"
#include "global.h"

#include <QPalette>

using namespace qevercloud;

extern Global global;

NTitleEditor::NTitleEditor(QWidget *parent) :
    QLineEdit(parent)
{
    // Setup the note title editor
    QPalette pal;
    //pal.setColor(QPalette::Text, QColor(102,153,255));
    pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} QLineEdit:hover {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    this->setStyleSheet(inactiveColor);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(titleChanged(QString)));

    this->setFont(global.getGuiFont(font()));
}

void NTitleEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


// We now have focus.  Change the appearance
void NTitleEditor::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  emit(focussed(true));
}


// Focus is lost.  Set the colors back and check for text changed
void NTitleEditor::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  setStyleSheet(inactiveColor);
  titleChanged(cleanupTitle(text()));
  emit(focussed(false));
  this->setCursorPosition(0);
}


// Set the title and values.  default is the default title if the user
// blanks everything else out.  The default text logic should eventually be
// moved to the browser editor since it has the actual text of the note
// we need for the note.
void NTitleEditor::setTitle(qint32 lid, QString title, QString defaultT) {
    priorTitle = title;
    currentLid = lid;
    initialTitle = title;
    defaultTitle = defaultT;
    blockSignals(true);
    setText(title);
    this->setCursorPosition(0);
    blockSignals(false);
}


// This function gets control whenever the text is edited.
void NTitleEditor::titleChanged(QString text) {
    if (currentLid <= 0)
        return;
    this->blockSignals(true);

    // Check that we have some type of title.  If not we go with
    // the default
    text = cleanupTitle(text);
    int cursorPosition = this->cursorPosition();
    this->setText(text);
    this->setCursorPosition(cursorPosition);
    this->blockSignals(false);
    emit(titleUpdated(text));

    // Now check that the text has actually changed.  We need to do this because
    // we can also get here when focus is lost.
    if (!hasFocus()) {
        if (text.trimmed() != initialTitle.trimmed() || priorTitle.trimmed() != text.trimmed()) {
            NoteTable noteTable(global.db);
            noteTable.updateTitle(currentLid, text, true);
            emit(titleChanged());
            setText(text);
            priorTitle = text;
            initialTitle = text;
        }
    }
}



QString NTitleEditor::cleanupTitle(QString text) {
    if (text.trimmed() == "" && !hasFocus()) {
        text = tr("Untitled note");
    }

    if (text.length() > EDAM_NOTE_TITLE_LEN_MAX)
        text = text.mid(0,EDAM_NOTE_TITLE_LEN_MAX-1);

    return text;
}


void NTitleEditor::setTitleFromContent(QString s) {
    if (initialTitle.toLower() != tr("untitled note"))
        return;
    if (s.trimmed() == "")
        s = tr("Untitled note");
    int newline = s.indexOf("\n");
    if (newline != -1)
        s = s.mid(0,newline);
    s = cleanupTitle(s);
    blockSignals(true);
    this->setText(s);
    blockSignals(false);
    emit(titleUpdated(s));
}


void NTitleEditor::checkNoteTitleChange() {
    if (this->text() != initialTitle) {
        NoteTable noteTable(global.db);
        noteTable.updateTitle(currentLid, text(), true);
        emit(titleChanged());
        priorTitle = text();
    }
}

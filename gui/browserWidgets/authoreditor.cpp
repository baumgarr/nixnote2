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

#include "authoreditor.h"
#include "global.h"
#include "sql/notetable.h"

extern Global global;

AuthorEditor::AuthorEditor(QWidget *parent) :
    QLineEdit(parent)
{
    // Setup the note title editor
    QPalette pal;
    //pal.setColor(QPalette::Text, QColor(102,153,255));
    //pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    setFont(global.getGuiFont(font()));
    this->setPlaceholderText(tr("Click to set author"));

    inactiveColor = global.getThemeCss("noteAuthorInactiveCss");
    activeColor = global.getThemeCss("noteAuthorActiveCss");
    if (inactiveColor == "")
        inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} ";
    if (activeColor == "")
        activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";

    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(inactiveColor);

    hide();
}

void AuthorEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


void AuthorEditor::setAuthor(qint32 lid, QString text) {
    currentLid = lid;
    blockSignals(true);
    setText(text);
    blockSignals(false);
    initialText = text;
    priorText = text;
}


void AuthorEditor::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  this->setCursor(Qt::ArrowCursor);
  emit(focussed(true));
}

void AuthorEditor::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  this->setCursor(Qt::PointingHandCursor);
  setStyleSheet(inactiveColor);
  textModified(text());
  emit(focussed(false));
}


void AuthorEditor::textModified(QString text) {
    if (this->hasFocus())
        return;

    this->blockSignals(true);

    NoteTable noteTable(global.db);
    Note n;
    noteTable.get(n, currentLid, false,false);
    QString oldAuthor;
    NoteAttributes attributes;
    if (n.attributes.isSet())
        attributes = n.attributes;
    if (attributes.author.isSet())
        oldAuthor = attributes.author;
    if (oldAuthor.trimmed() != text.trimmed())
        noteTable.updateAuthor(currentLid, text, true);

    setText(text);
    this->blockSignals(false);

    if (text.trimmed() != initialText.trimmed() || priorText.trimmed() != text.trimmed()) {
        emit textUpdated();
    }

    priorText = text;
}


QString AuthorEditor::getText() {
    return text();
}

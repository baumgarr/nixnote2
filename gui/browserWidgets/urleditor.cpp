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

#include "urleditor.h"
#include "global.h"
#include "sql/notetable.h"

#include <QDesktopServices>

extern Global global;

UrlEditor::UrlEditor(QWidget *parent) :
    QLineEdit(parent)
{
    // Setup the note title editor
    QPalette pal;
    //pal.setColor(QPalette::Text, QColor(102,153,255));
    //pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    this->setFont(global.getGuiFont(font()));

    inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} ";
    activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(inactiveColor);

    this->setPlaceholderText(tr("Click to set source URL..."));
//    defaultText = QString(tr("Click to set source URL..."));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textModified(QString)));
    //this->textModified(defaultText);

    //connect(this, SIGNAL(focussed(bool)), this, SLOT(gainedFocus(bool)));
    hide();
}

void UrlEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


void UrlEditor::setUrl(qint32 lid, QString text) {
    currentLid = lid;
    blockSignals(true);
    setText(text);
    blockSignals(false);
    initialText = text;
    priorText = text;
}


void UrlEditor::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  this->setCursor(Qt::ArrowCursor);
  emit(focussed(true));
}

void UrlEditor::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  this->setCursor(Qt::PointingHandCursor);
  setStyleSheet(inactiveColor);
  textModified(text());
  emit(focussed(false));
}


void UrlEditor::textModified(QString text) {
    if (hasFocus())
        return;

    this->blockSignals(true);

    NoteTable noteTable(global.db);
    noteTable.updateUrl(currentLid, text, true);

    if (text.trimmed() == "" && !hasFocus() && priorText.trimmed() == text.trimmed())
        return;
    else
        setText(text);
    this->blockSignals(false);

    if (text.trimmed() != initialText.trimmed() || priorText.trimmed() != text.trimmed())
        emit(textUpdated());

    priorText = text;
}


QString UrlEditor::getText() {
        return text();
}



// Listen for mouse press events.  This tells us if
// we need to open based on a middle click.
void UrlEditor::mouseReleaseEvent(QMouseEvent *e) {
    if ( e->button() == Qt::MidButton) {
        if (text().trimmed() != "") {
            QString url = text().trimmed();
            if (!url.toLower().startsWith("http://") &&
                !url.toLower().startsWith("https://") &&
                !url.toLower().startsWith("mailto://") &&
                !url.toLower().startsWith("mailto:") &&
                !url.toLower().startsWith("ftp://") &&
                !url.toLower().startsWith("file://"))
                   url = "http://"+url;
            QDesktopServices::openUrl(QUrl(url));
        }
    } else
        QLineEdit::mouseReleaseEvent(e);
}

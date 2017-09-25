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
#include <QStandardItem>

extern Global global;


//****************************************************
//* Constructor
//****************************************************
TagEditorNewTag::TagEditorNewTag(QWidget *parent) :
    QLineEdit(parent)
{
    QLOG_TRACE_IN() << typeid(*this).name();
    account = 0;
    this->setCursor(Qt::PointingHandCursor);
    // Setup the note title editor
    QPalette pal;
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    this->setFont(global.getGuiFont(font()));

    inactiveColor = global.getThemeCss("noteTagAddInactiveCss");
    if (inactiveColor=="")
        inactiveColor = global.getTagEditorInactiveStyle();

    activeColor = global.getThemeCss("noteTagAddActiveCss");
    if (activeColor == "")
        activeColor = global.getTagEditorActiveStyle();
    this->setStyleSheet(inactiveColor);

    this->setPlaceholderText(tr("Click to add tag..."));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textModified(QString)));

//    connect(this, SIGNAL(focussed(bool)), this, SLOT(gainedFocus(bool)));
    completer = new QCompleter(this);
    //completer->popup()->setItemDelegate(new TagEditorNewTagCompleterPopupDelegate());
    connect(completer, SIGNAL(activated(QString)), this, SLOT(mouseCompleterSelection(QString)));
    loadCompleter();
    connect(this, SIGNAL(returnPressed()), this, SLOT(enterPressed()));
    hide();
    QLOG_TRACE_OUT() << typeid(*this).name();
}


void TagEditorNewTag::mouseCompleterSelection(QString value) {
    QLOG_DEBUG() << "Mouse selection completer for tag: " << value;
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QCoreApplication::postEvent (this, event);
}


void TagEditorNewTag::enterPressed() {
    QLOG_DEBUG() << "Enter pressed in new tag";
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    QCoreApplication::postEvent (this, event);
}



//***********************************************************
//* Load the completer with the list of valid tag names
//***********************************************************
void TagEditorNewTag::loadCompleter() {
    QLOG_TRACE_IN() << typeid(*this).name();
    QList<int> tagList;
    TagTable tagTable(global.db);
    QStringList tagNames;
    QHash<qint32, QString> nameTable;
    QHash<qint32, QString> completerEntries;
    tagTable.getAllNames(&nameTable);

    tagTable.getAllInAccount(tagList, account);
    completerEntries.clear();
    for (qint32 i=0; i<tagList.size(); i++) {
        QString name = "";
        if (nameTable.contains(tagList[i]))
            name = nameTable[tagList[i]];
        if (name != "" && !completerEntries.contains(tagList[i]) && !currentTags.contains(name)) {
            tagNames << name;
            completerEntries.insert(tagList[i], name);
        }
    }
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#if QT_VERSION >= 0x050000
    completer->setFilterMode(Qt::MatchContains);
#endif
    setCompleter(completer);

    QStringListModel *model;
    model = (QStringListModel*)(completer->model());
    if(model==NULL)
        model = new QStringListModel();

    model->setStringList(tagNames);
    completer->setModel(model);
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//******************************************************
//* Set the color whe the editor has a good focus
//******************************************************
void TagEditorNewTag::setActiveColor() {
    QLOG_TRACE_IN() << typeid(*this).name();
    setStyleSheet(activeColor);
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* Focus event.  The editor has gained focus
//*******************************************************
void TagEditorNewTag::focusInEvent(QFocusEvent *e)
{
    QLOG_TRACE_IN() << typeid(*this).name();
    this->setCursor(Qt::ArrowCursor);
    QLineEdit::focusInEvent(e);
    setStyleSheet(activeColor);
    emit(focussed(true));
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* Focus event.  The editor has lost focus
//*******************************************************
void TagEditorNewTag::focusOutEvent(QFocusEvent *e)
{
    QLOG_TRACE_IN() << typeid(*this).name();
    this->setCursor(Qt::PointingHandCursor);
    QLineEdit::focusOutEvent(e);
    setStyleSheet(inactiveColor);
    emit(focussed(false));
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* The current text has changed
//*******************************************************
void TagEditorNewTag::textModified(QString text) {
    QLOG_TRACE_IN() << typeid(*this).name();
    if (this->hasFocus()) {
        QLOG_TRACE_OUT() << typeid(*this).name();
        return;
    }
    this->blockSignals(true);
    setText(text);
    this->blockSignals(false);
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* Get the current text.  If the text is the default
//* text, the n return an empty string.
//*******************************************************
QString TagEditorNewTag::getText() {
    QLOG_TRACE_IN() << typeid(*this).name();
    QString retval = text().trimmed();
    QLOG_TRACE_OUT() << typeid(*this).name();
    return retval;
}


//*******************************************************
//* Set the list of valid tags for the completer
//*******************************************************
void TagEditorNewTag::setTags(QStringList s) {
    QLOG_TRACE_IN() << typeid(*this).name();
    currentTags = s;
    loadCompleter();
    QLOG_TRACE_OUT() << typeid(*this).name();
}


//*******************************************************
//* Add a new tag to the completer list
//*******************************************************
void TagEditorNewTag::addTag(QString s) {
    QLOG_TRACE_IN() << typeid(*this).name();
    currentTags.append(s);
    loadCompleter();
    QLOG_TRACE_OUT() << typeid(*this).name();
}



//*******************************************************
//* Override the event loop.  If the user presses "TAB"
//* and there is a valid tag entered, we signal the parent
//* that a new tag is needed.
//*******************************************************
bool  TagEditorNewTag::event(QEvent *event)
{
    QLOG_TRACE_IN() << typeid(*this).name();
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = (QKeyEvent*)event;
        if (ke->key() == Qt::Key_Tab) {
            if (getText().trimmed() != "") {
                emit(tabPressed());
                QLOG_TRACE_OUT() << typeid(*this).name();
                return true;
            }
            QLOG_TRACE_OUT() << typeid(*this).name();
            return QLineEdit::event(event);
        }
    }
    QLOG_TRACE_OUT() << typeid(*this).name();
    return QLineEdit::event(event);
}



//*******************************************************
//* Reset the tag editor to the default text
//*******************************************************
void TagEditorNewTag::resetText() {
    QLOG_TRACE_IN() << typeid(*this).name();
    blockSignals(true);
    setText("");
    blockSignals(false);
    QLOG_TRACE_OUT() << typeid(*this).name();
}


void TagEditorNewTag::notebookSelectionChanged(qint32 notebook) {
    QLOG_TRACE_IN() << typeid(*this).name();
    account = notebook;
    loadCompleter();
    QLOG_TRACE_OUT() << typeid(*this).name();
}




void TagEditorNewTag::setAccount(qint32 account) {
    QLOG_TRACE_IN() << typeid(*this).name();
    this->account = account;
    loadCompleter();
    QLOG_TRACE_OUT() << typeid(*this).name();
}


void TagEditorNewTag::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Comma)
        this->enterPressed();
    else
        QLineEdit::keyPressEvent(event);

}

/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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


#include "externalbrowse.h"
#include <QGridLayout>
#include <QLayout>
#include "global.h"

extern Global global;

ExternalBrowse::ExternalBrowse(qint32 lid, QWidget *parent) :
    QMdiSubWindow(parent)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    this->setWindowTitle(tr("NixNote"));
    setWindowIcon(global.getIconResource(":windowIcon"));

    browser = new NBrowserWindow(this);
    setWidget(browser);

    // Setup shortcuts
    focusTitleShortcut = new QShortcut(this);
    focusTitleShortcut->setContext(Qt::WidgetShortcut);
    this->setupShortcut(focusTitleShortcut, "Focus_Title");
    connect(focusTitleShortcut, SIGNAL(activated()), &browser->noteTitle, SLOT(setFocus()));

    focusNoteShortcut = new QShortcut(this);
    focusNoteShortcut->setContext(Qt::WidgetShortcut);
    this->setupShortcut(focusNoteShortcut, "Focus_Note");
    connect(focusNoteShortcut, SIGNAL(activated()), browser->editor, SLOT(setFocus()));

    focusTagShortcut = new QShortcut(this);
    focusTagShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusTagShortcut, "Focus_Tag");
    connect(focusTagShortcut, SIGNAL(activated()), browser, SLOT(newTagFocusShortcut()));

    focusUrlShortcut = new QShortcut(this);
    focusUrlShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusUrlShortcut, "Focus_Url");
    connect(focusUrlShortcut, SIGNAL(activated()), browser, SLOT(urlFocusShortcut()));

    focusAuthorShortcut = new QShortcut(this);
    focusAuthorShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusAuthorShortcut, "Focus_Author");
    connect(focusAuthorShortcut, SIGNAL(activated()), browser, SLOT(authorFocusShortcut()));

    focusNotebookShortcut = new QShortcut(this);
    focusNotebookShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusNotebookShortcut, "Focus_Notebook");
    connect(focusNotebookShortcut, SIGNAL(activated()), browser, SLOT(notebookFocusShortcut()));

    focusFontShortcut = new QShortcut(this);
    focusFontShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusFontShortcut, "Focus_Font");
    connect(focusFontShortcut, SIGNAL(activated()), browser, SLOT(fontFocusShortcut()));

    focusFontSizeShortcut = new QShortcut(this);
    focusFontSizeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(focusFontSizeShortcut, "Focus_Font_Size");
    connect(focusFontSizeShortcut, SIGNAL(activated()), browser, SLOT(fontSizeFocusShortcut()));

    findShortcut = new QShortcut(this);
    findShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(findShortcut, "Edit_Search_Find");
    connect(findShortcut, SIGNAL(activated()), browser, SLOT(findShortcut()));

    findReplaceShortcut = new QShortcut(this);
    findReplaceShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(findReplaceShortcut, "Edit_Search_Find_Replace");
    connect(findReplaceShortcut, SIGNAL(activated()), browser, SLOT(findReplaceShortcut()));

    findNextShortcut = new QShortcut(this);
    findNextShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(findNextShortcut, "Edit_Search_Find_Next");
    connect(findNextShortcut, SIGNAL(activated()), browser, SLOT(findNextShortcut()));

    findPrevShortcut = new QShortcut(this);
    findPrevShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    this->setupShortcut(findPrevShortcut, "Edit_Search_Find_Prev");
    connect(findPrevShortcut, SIGNAL(activated()), browser, SLOT(findPrevShortcut()));

    // Load actual note
    browser->setContent(lid);


}


// We don't really close the window, we just hide it.  This
// solves problems later on if the user wants to re-open the same
// note in an external window.
void ExternalBrowse::closeEvent(QCloseEvent *closeEvent) {
    if (this->browser->editor->isDirty)
        this->browser->saveNoteContent();
    this->setVisible(false);
    closeEvent->ignore();
}



void ExternalBrowse::setTitle(QString text) {
    this->setWindowTitle(tr("NixNote - ") +text);
}




// Load any shortcut keys
void ExternalBrowse::setupShortcut(QShortcut *action, QString text) {
    if (!global.shortcutKeys->containsAction(&text))
        return;
    QKeySequence key(global.shortcutKeys->getShortcut(&text));
    action->setKey(key);
}

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

#include "ntabwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "html/noteformatter.h"
#include "gui/externalbrowse.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

NTabWidget::NTabWidget(SyncRunner *s, NNotebookView *n, NTagView *t)
{
    syncThread = s;
    notebookTreeView = n;
    tagTreeView = t;

    this->setFont(global.getGuiFont(font()));

    tabBar = new QTabBar(this);
    tabBar->setHidden(true);
    tabBar->setMovable(true);
    tabBar->setTabsClosable(true);
    tabBar->setShape(QTabBar::RoundedNorth);
 //   tabBar.setMinimumHeight(20);
 //   tabBar.setMaximumHeight(20);

    browserList = new QList<NBrowserWindow *>();
    externalList = new QList<ExternalBrowse *>();
    vboxlayout.addWidget(tabBar);
    vboxlayout.addWidget(&stack);
    setLayout(&vboxlayout);

    connect(tabBar, SIGNAL(currentChanged(int)),
                 &stack, SLOT(setCurrentIndex(int)));
    connect(tabBar, SIGNAL(tabCloseRequested(int)),
                 this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(tabMoved(int, int)),
                 this, SLOT(moveTab(int, int)));
    this->layout()->setMargin(0);
    htmlEntities = new HtmlEntitiesDialog();
    htmlEntities->setHidden(true);
    connect(htmlEntities, SIGNAL(entityClicked(QString)), this, SLOT(htmlEntitiesClicked(QString)));
}

NTabWidget::~NTabWidget() {
    htmlEntities->close();
    delete browserList;
}

void NTabWidget::addBrowser(NBrowserWindow *v, QString title) {
    tabBar->addTab(title);
    int index = stack.addWidget(v);
    stack.setCurrentIndex(index);
    browserList->append(v);
    tabBar->setCurrentIndex(index);
    tabBar->raise();

    setupConnections(v);

    if (browserList->size() <= 1)
        tabBar->setHidden(true);
    else
        tabBar->setHidden(false);

    connect(v, SIGNAL(showHtmlEntities()), this, SLOT(showHtmlEntities()));
    return;
}

void NTabWidget::closeTab(int index) {
    tabBar->removeTab(index);
    stack.removeWidget(browserList->at(index));
    delete browserList->at(index);
    browserList->removeAt(index);
    if (browserList->size() <= 1)
        tabBar->setHidden(true);
    else
        tabBar->setHidden(false);
}



void NTabWidget::moveTab(int from, int to) {
    stack.removeWidget(browserList->at(from));
    stack.insertWidget(to, browserList->at(from));
    browserList->move(from, to);

}


NBrowserWindow* NTabWidget::currentBrowser() {
    // If the current tab has focus, then we return it.
    if (this->browserList->at(tabBar->currentIndex())->hasFocus())
        return this->browserList->at(tabBar->currentIndex());

    // If no tab has focus, then an external window has focus
    for (int i=0; i<externalList->size(); i++) {
        if (externalList->at(i)->hasFocus())
            return externalList->at(i)->browser;
    }

    // If no external window has focus then we return any current
    // tab.  This can happen if a user clicks on an html entities dialog
    // from an external window.
    return this->browserList->at(tabBar->currentIndex());
}



// Open upCurrentTab a note.  If it is already open in a tab we raise that tab.
void NTabWidget::openNote(qint32 lid, OpenNoteMode mode) {

    // If the lid < 0, then we just clear it & get out
    if (lid < 0 && mode != ExternalWindow) {
        currentBrowser()->setContent(-1);
        return;
    }

    // If we've gotten this far, we have a good note
    Note n;
    NoteTable noteTable(global.db);
    noteTable.get(n, lid, false, false);

    NBrowserWindow *view;
    bool found = false;

    // Find out if we already have it open somewhere else
    for (int i=0;i<browserList->size() && !found &&
         mode != ExternalWindow; i++) {
        if (browserList->at(i)->lid == lid) {
            found = true;
            tabBar->setCurrentIndex(i);
            tabBar->raise();
            stack.setCurrentIndex(i);
            stack.raise();
            return;
        }
    }
    for (int i=0;i<externalList->size() && !found &&
         mode == ExternalWindow; i++) {
        if (externalList->at(i)->browser->lid == lid) {
            ExternalBrowse *external = externalList->at(i);
            external->setVisible(true);
            external->raise();
            external->setFocus();
            return;
        }
    }

    // if we want a new window AND it isn't already open, create a new tab
    if (mode == NewTab && !found) {
        view = new NBrowserWindow();
        addBrowser(view, n.title);
        setupConnections(view);
    } else {
        if (mode == CurrentTab) {
            view = currentBrowser();
            if (n.title.isSet())
                tabBar->setTabText(tabBar->currentIndex(), n.title);
            else
                tabBar->setTabText(tabBar->currentIndex(), tr("Untitled Note"));
        } else {
            ExternalBrowse *external = new ExternalBrowse(lid);
            externalList->append(external);
            setupExternalBrowserConnections(external->browser);
            external->setWindowTitle(tr("NixNote - ") +external->browser->noteTitle.text());
            external->show();
            connect(external->browser->editor->titleEditor, SIGNAL(titleUpdated(QString)), external, SLOT(setTitle(QString)));
            return;
        }
    }

    view->setContent(lid);
}


// Set the title of the current tab based on the note's title
void NTabWidget::setTitle(QString t) {
    this->setTitle(tabBar->currentIndex(), t);
}


// Set the title of a particular tab based on the note's title
void NTabWidget::setTitle(int index, QString t) {
    tabBar->setTabText(index, t);
}


// A signal that a note has been updated.
void NTabWidget::noteUpdateSignaled(qint32 lid) {
    //  COMMENTED OUT TO PREVENT SEGFAULT  emit(this->noteUpdated(lid));

    Note n;
    NoteTable noteTable(global.db);
    noteTable.get(n, lid, false, false);
    for (int i=0;i<browserList->size(); i++) {
        if (browserList->at(i)->lid == lid) {
            if (n.title.isSet())
                setTitle(i, n.title);
            return;
        }
    }
}


// A signal that a note has been updated.
void NTabWidget::updateNoteListSignaled(qint32 lid, int column, QVariant data) {
    emit(this->updateNoteList(lid, column, data));
}



// A note has been synchronized
void NTabWidget::noteSyncSignaled(qint32 lid) {
    // COMMENTED OUT TO PREVENT SEGFAULT emit(this->noteUpdated(lid));

    Note n;
    NoteTable noteTable(global.db);
    noteTable.get(n, lid, false, false);
    for (int i=0;i<browserList->size(); i++) {
        if (browserList->at(i)->lid == lid && !browserList->at(i)->editor->isDirty) {
            setTitle(i, n.title);
            browserList->at(i)->blockSignals(true);
            browserList->at(i)->clear();
            browserList->at(i)->setContent(lid);
            browserList->at(i)->blockSignals(false);
            return;
        }
    }
    for (int i=0;i<externalList->size(); i++) {
        if (externalList->at(i)->browser->lid == lid && !externalList->at(i)->browser->editor->isDirty) {
            externalList->at(i)->setWindowTitle(tr("NixNote - ")+ n.title);
            externalList->at(i)->browser->blockSignals(true);
            externalList->at(i)->browser->clear();
            externalList->at(i)->browser->setContent(lid);
            externalList->at(i)->browser->blockSignals(false);
            return;
        }
    }
}


void NTabWidget::tagCreationSignaled(qint32 lid) {
    emit(this->tagCreated(lid));
}


void NTabWidget::setupConnections(NBrowserWindow *newBrowser) {
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), newBrowser, SLOT(tagRenamed(qint32,QString,QString)));
    connect(tagTreeView, SIGNAL(tagDeleted(qint32, QString)), newBrowser, SLOT(tagDeleted(qint32, QString)));
    connect(tagTreeView, SIGNAL(tagAdded(qint32)), &newBrowser->tagEditor.newTag, SLOT(loadCompleter()));

    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), newBrowser, SLOT(notebookRenamed(qint32,QString,QString)));
    connect(notebookTreeView, SIGNAL(notebookDeleted(qint32, QString)), newBrowser, SLOT(notebookDeleted(qint32, QString)));
    connect(notebookTreeView, SIGNAL(notebookAdded(qint32)), newBrowser, SLOT(notebookAdded(qint32)));

    connect(notebookTreeView, SIGNAL(stackRenamed(QString,QString)), newBrowser, SLOT(stackRenamed(QString,QString)));
    connect(notebookTreeView, SIGNAL(stackDeleted(QString)), newBrowser, SLOT(stackDeleted(QString)));
    connect(notebookTreeView, SIGNAL(stackAdded(QString)), newBrowser, SLOT(stackAdded(QString)));

    connect(newBrowser, SIGNAL(noteUpdated(qint32)), this, SLOT(noteUpdateSignaled(qint32)));
    connect(newBrowser, SIGNAL(tagAdded(qint32)), this, SLOT(tagCreationSignaled(qint32)));
    connect(newBrowser, SIGNAL(updateNoteList(qint32,int,QVariant)), this, SLOT(updateNoteListSignaled(qint32,int,QVariant)));
    connect(syncThread, SIGNAL(noteUpdated(qint32)), this, SLOT(noteSyncSignaled(qint32)));
    connect(newBrowser, SIGNAL(noteContentEditedSignal(QString,qint32,QString)), this,  SLOT(noteContentEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteTitleEditedSignal(QString,qint32,QString)), this,  SLOT(noteTitleEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteAuthorEditedSignal(QString,qint32,QString)), this,  SLOT(noteAuthorEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteLocationEditedSignal(QString,qint32,double,double,double,QString)), this,  SLOT(noteLocationEdited(QString,qint32,double,double,double,QString)));
    connect(newBrowser, SIGNAL(noteUrlEditedSignal(QString,qint32,QString)), this,  SLOT(noteUrlEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteAlarmEditedSignal(QString,qint32,bool,QString)), this,  SLOT(noteAlarmEdited(QString,qint32,bool,QString)));
    connect(newBrowser, SIGNAL(noteTagsEditedSignal(QString,qint32,QStringList)), this,  SLOT(noteTagsEdited(QString,qint32,QStringList)));
    connect(newBrowser, SIGNAL(noteNotebookEditedSignal(QString,qint32,qint32,QString)), this,  SLOT(noteNotebookEdited(QString,qint32,qint32,QString)));
    connect(newBrowser, SIGNAL(noteDateEditedSignal(QString,qint32,int,QDateTime)), this,  SLOT(noteDateEdited(QString,qint32,int,QDateTime)));
    connect(newBrowser, SIGNAL(evernoteLinkClicked(qint32,bool)), this, SLOT(evernoteLinkClicked(qint32, bool)));
}



void NTabWidget::evernoteLinkClicked(qint32 openLid, bool newWindow) {
    emit(updateSelectionRequested());
    if (newWindow)
        openNote(openLid, NewTab);
    else
        openNote(openLid, CurrentTab);
}



void NTabWidget::undoButtonPressed() {
    currentBrowser()->undoButtonPressed();
}

void NTabWidget::redoButtonPressed() {
    currentBrowser()->redoButtonPressed();
}

void NTabWidget::cutButtonPressed() {
    currentBrowser()->cutButtonPressed();
}

void NTabWidget::copyButtonPressed() {
    currentBrowser()->copyButtonPressed();
}

void NTabWidget::pasteButtonPressed() {
    currentBrowser()->pasteButtonPressed();
}

void NTabWidget::pasteAsTextButtonPressed() {
    currentBrowser()->pasteWithoutFormatButtonPressed();
}


void NTabWidget::selectAllButtonPressed() {
    currentBrowser()->selectAllButtonPressed();
}


void NTabWidget::viewExtendedInformation() {
    switch (currentBrowser()->expandButton.currentState)  {
    case (EXPANDBUTTON_1) :
        currentBrowser()->expandButton.click();
        currentBrowser()->expandButton.click();
        break;
    case (EXPANDBUTTON_2) :
        currentBrowser()->expandButton.click();
        break;
    case (EXPANDBUTTON_3) :
        currentBrowser()->expandButton.click();
        break;
    }
}


void NTabWidget::toggleSource() {
    currentBrowser()->toggleSource();
}


void NTabWidget::updateResourceHash(qint32 noteLid, QByteArray oldHash, QByteArray newHash) {
    for (int i=0; i<browserList->size(); i++) {
        NBrowserWindow *browser = browserList->at(i);
        browser->updateResourceHash(noteLid, oldHash, newHash);
    }
}



// Refresh a note's content
void NTabWidget::refreshNoteContent(qint32 lid) {
    for (int i=0;i<browserList->size(); i++) {
        if (browserList->at(i)->lid == lid) {
            browserList->at(i)->setContent(lid);
            return;
        }
    }
}



// Called when an external editor is created

void NTabWidget::setupExternalBrowserConnections(NBrowserWindow *newBrowser) {
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), newBrowser, SLOT(tagRenamed(qint32,QString,QString)));
    connect(tagTreeView, SIGNAL(tagDeleted(qint32, QString)), newBrowser, SLOT(tagDeleted(qint32, QString)));
    connect(tagTreeView, SIGNAL(tagAdded(qint32)), &newBrowser->tagEditor.newTag, SLOT(loadCompleter()));

    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), newBrowser, SLOT(notebookRenamed(qint32,QString,QString)));
    connect(notebookTreeView, SIGNAL(notebookDeleted(qint32, QString)), newBrowser, SLOT(notebookDeleted(qint32, QString)));
    connect(notebookTreeView, SIGNAL(notebookAdded(qint32)), newBrowser, SLOT(notebookAdded(qint32)));

    connect(notebookTreeView, SIGNAL(stackRenamed(QString,QString)), newBrowser, SLOT(stackRenamed(QString,QString)));
    connect(notebookTreeView, SIGNAL(stackDeleted(QString)), newBrowser, SLOT(stackDeleted(QString)));
    connect(notebookTreeView, SIGNAL(stackAdded(QString)), newBrowser, SLOT(stackAdded(QString)));

    connect(newBrowser, SIGNAL(noteUpdated(qint32)), this, SLOT(noteUpdateSignaled(qint32)));
    connect(newBrowser, SIGNAL(tagAdded(qint32)), this, SLOT(tagCreationSignaled(qint32)));
    connect(newBrowser, SIGNAL(updateNoteList(qint32,int,QVariant)), this, SLOT(updateNoteListSignaled(qint32,int,QVariant)));
    connect(syncThread, SIGNAL(noteUpdated(qint32)), this, SLOT(noteSyncSignaled(qint32)));
    connect(newBrowser, SIGNAL(noteContentEditedSignal(QString,qint32,QString)), this,  SLOT(noteContentEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(evernoteLinkClicked(qint32,bool)), this, SLOT(evernoteLinkClicked(qint32, bool)));
    connect(newBrowser, SIGNAL(noteTitleEditedSignal(QString,qint32,QString)), this,  SLOT(noteTitleEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteAuthorEditedSignal(QString,qint32,QString)), this,  SLOT(noteAuthorEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteLocationEditedSignal(QString,qint32,double,double,double,QString)), this,  SLOT(noteLocationEdited(QString,qint32,double,double,double,QString)));
    connect(newBrowser, SIGNAL(noteUrlEditedSignal(QString,qint32,QString)), this,  SLOT(noteUrlEdited(QString,qint32,QString)));
    connect(newBrowser, SIGNAL(noteAlarmEditedSignal(QString,qint32,bool,QString)), this,  SLOT(noteAlarmEdited(QString,qint32,bool,QString)));
    connect(newBrowser, SIGNAL(noteTagsEditedSignal(QString,qint32,QStringList)), this,  SLOT(noteTagsEdited(QString,qint32,QStringList)));
    connect(newBrowser, SIGNAL(noteNotebookEditedSignal(QString,qint32,qint32,QString)), this,  SLOT(noteNotebookEdited(QString,qint32,qint32,QString)));
    connect(newBrowser, SIGNAL(noteDateEditedSignal(QString,qint32,int,QDateTime)), this,  SLOT(noteDateEdited(QString,qint32,int,QDateTime)));

    // Hide the html entities dialog since it doesn't work.
    newBrowser->hideHtmlEntities();
}


// A note was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteContentEdited(QString uuid, qint32 lid, QString content) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->editor->blockSignals(true);
            browserList->at(i)->editor->setHtml(content);
            browserList->at(i)->editor->blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->editor->blockSignals(true);
            externalList->at(i)->browser->editor->setHtml(content);
            externalList->at(i)->browser->editor->blockSignals(false);
        }
    }

    // Invalidate the cache (if needed)
    if (global.cache.contains(lid)) {
        NoteCache *cache = global.cache[lid];
        cache->noteContent = content.toUtf8();
    }
}



// A note's notebook was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteNotebookEdited(QString uuid, qint32 lid, qint32 notebookLid, QString notebookName) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->notebookMenu.blockSignals(true);
            browserList->at(i)->notebookMenu.updateCurrentNotebook(notebookLid, notebookName);
            browserList->at(i)->notebookMenu.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->notebookMenu.blockSignals(true);
            externalList->at(i)->browser->notebookMenu.updateCurrentNotebook(notebookLid, notebookName);
            externalList->at(i)->browser->notebookMenu.blockSignals(false);
        }
    }
}




// A note's notebook was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteDateEdited(QString uuid, qint32 lid, int dateType, QDateTime dt) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            DateTimeEditor *dte=NULL;
            switch(dateType) {
            case NOTE_CREATED_DATE:
                dte = &browserList->at(i)->dateEditor.createdDate;
                break;
            case NOTE_UPDATED_DATE:
                dte = &browserList->at(i)->dateEditor.updatedDate;
                break;
            case NOTE_ATTRIBUTE_SUBJECT_DATE:
                dte = &browserList->at(i)->dateEditor.subjectDate;
                break;
            default: return;
            }
            dte->blockSignals(true);
            dte->setDateTime(dt);
            dte->blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            DateTimeEditor *dte=NULL;
            switch(dateType) {
            case NOTE_CREATED_DATE:
                dte = &externalList->at(i)->browser->dateEditor.createdDate;
                break;
            case NOTE_UPDATED_DATE:
                dte = &externalList->at(i)->browser->dateEditor.updatedDate;
                break;
            case NOTE_ATTRIBUTE_SUBJECT_DATE:
                dte = &externalList->at(i)->browser->dateEditor.subjectDate;
                break;
            default: return;
            }
            dte->blockSignals(true);
            dte->setDateTime(dt);
            dte->blockSignals(false);
        }
    }
}


void NTabWidget::saveAllNotes() {
    for (int i=0; i<browserList->size(); i++) {
        browserList->at(i)->saveNoteContent();
    }

    for (int i=0; i<externalList->size(); i++) {
        externalList->at(i)->browser->saveNoteContent();
    }
}



// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteTitleEdited(QString uuid, qint32 lid, QString content) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->noteTitle.blockSignals(true);
            browserList->at(i)->noteTitle.setText(content);
            browserList->at(i)->noteTitle.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->noteTitle.blockSignals(true);
            externalList->at(i)->browser->noteTitle.setText(content);
            externalList->at(i)->browser->noteTitle.blockSignals(false);
        }
    }
    emit(updateNoteTitle(uuid, lid, content));
}


// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteAuthorEdited(QString uuid, qint32 lid, QString content) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->dateEditor.authorEditor.blockSignals(true);
            browserList->at(i)->dateEditor.authorEditor.setText(content);
            browserList->at(i)->dateEditor.authorEditor.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->dateEditor.authorEditor.blockSignals(true);
            externalList->at(i)->browser->dateEditor.authorEditor.setText(content);
            externalList->at(i)->browser->dateEditor.authorEditor.blockSignals(false);
        }
    }
}






// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteLocationEdited(QString uuid, qint32 lid, double longitude, double latitude, double altitude, QString name) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->dateEditor.locationEditor.blockSignals(true);
            browserList->at(i)->dateEditor.locationEditor.setGeography(lid, longitude, latitude, altitude, name);
            browserList->at(i)->dateEditor.locationEditor.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->dateEditor.locationEditor.blockSignals(true);
            externalList->at(i)->browser->dateEditor.locationEditor.setGeography(lid, longitude, latitude, altitude, name);
            externalList->at(i)->browser->dateEditor.locationEditor.blockSignals(false);
        }
    }
}



// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteUrlEdited(QString uuid, qint32 lid, QString content) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->urlEditor.blockSignals(true);
            browserList->at(i)->urlEditor.setText(content);
            browserList->at(i)->urlEditor.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->urlEditor.blockSignals(true);
            externalList->at(i)->browser->urlEditor.setText(content);
            externalList->at(i)->browser->urlEditor.blockSignals(false);
        }
    }
}






// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteTagsEdited(QString uuid, qint32 lid, QStringList names) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            browserList->at(i)->tagEditor.blockSignals(true);
            browserList->at(i)->tagEditor.setTags(names);
            browserList->at(i)->tagEditor.blockSignals(false);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            externalList->at(i)->browser->tagEditor.blockSignals(true);
            externalList->at(i)->browser->tagEditor.setTags(names);
            externalList->at(i)->browser->tagEditor.blockSignals(false);
        }
    }
}




// A note title was edited, so we need to make sure all the windows
// are in sync.
void NTabWidget::noteAlarmEdited(QString uuid, qint32 lid, bool strikeout, QString text) {
    for (int i=0; i<browserList->size(); i++) {
        if (lid == browserList->at(i)->lid &&
                browserList->at(i)->uuid != uuid) {
            QFont f = browserList->at(i)->alarmText.font();
            f.setStrikeOut(strikeout);
            browserList->at(i)->alarmText.setFont(f);
            browserList->at(i)->alarmText.setText(text);
        }
    }
    for (int i=0; i<externalList->size(); i++) {
        if (lid == externalList->at(i)->browser->lid &&
                externalList->at(i)->browser->uuid != uuid) {
            QFont f = externalList->at(i)->browser->alarmText.font();
            f.setStrikeOut(strikeout);
            externalList->at(i)->browser->alarmText.setFont(f);
            externalList->at(i)->browser->alarmText.setText(text);
        }
    }
}







void NTabWidget::showHtmlEntities() {
    htmlEntities->setVisible(true);
}


void NTabWidget::htmlEntitiesClicked(QString entity) {
    currentBrowser()->insertHtml(entity);
}


void NTabWidget::reloadIcons() {
    for (int i=0; i<this->browserList->size(); i++) {
        this->browserList->at(i)->buttonBar->reloadIcons();
        this->browserList->at(i)->expandButton.reloadIcon();
        this->browserList->at(i)->dateEditor.locationEditor.reloadIcons();
        this->browserList->at(i)->alarmButton.reloadIcons();
        this->browserList->at(i)->tagEditor.reloadIcons();
        this->browserList->at(i)->notebookMenu.reloadIcons();
    }
    for (int i=0; i<this->externalList->size(); i++) {
        this->externalList->at(i)->browser->buttonBar->reloadIcons();
        this->externalList->at(i)->browser->expandButton.reloadIcon();
        this->externalList->at(i)->browser->dateEditor.locationEditor.reloadIcons();
        this->externalList->at(i)->browser->alarmButton.reloadIcons();
        this->externalList->at(i)->browser->tagEditor.reloadIcons();
        this->externalList->at(i)->browser->notebookMenu.reloadIcons();
    }
}

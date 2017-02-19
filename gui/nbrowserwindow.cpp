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

#include "nbrowserwindow.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "gui/browserWidgets/urleditor.h"
#include "sql/tagtable.h"
#include "html/noteformatter.h"
#include "html/enmlformatter.h"
#include "sql/usertable.h"
#include "sql/resourcetable.h"
#include "sql/linkednotebooktable.h"
#include "email/smtpclient.h"
#include "email/mimehtml.h"
#include "email/mimemessage.h"
#include "email/mimeinlinefile.h"
#include "global.h"
#include "gui/browserWidgets/colormenu.h"
#include "gui/plugins/pluginfactory.h"
#include "dialog/insertlinkdialog.h"
#include "html/thumbnailer.h"
#include "dialog/tabledialog.h"
#include "dialog/insertlatexdialog.h"
#include "dialog/endecryptdialog.h"
#include "dialog/encryptdialog.h"
#include "dialog/emaildialog.h"
#include "sql/configstore.h"
#include "utilities/encrypt.h"
#include "utilities/mimereference.h"
#include "html/attachmenticonbuilder.h"
#include "dialog/remindersetdialog.h"
#include "dialog/spellcheckdialog.h"
#include "utilities/pixelconverter.h"
#include "gui/browserWidgets/table/tablepropertiesdialog.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFileIconProvider>
#include <QFontDatabase>
#include <QSplitter>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QBuffer>
#include <QDateTime>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QPaintEngine>
#include <iostream>
#include <istream>
#include <qcalendarwidget.h>
#include <qplaintextedit.h>

extern Global global;

NBrowserWindow::NBrowserWindow(QWidget *parent) :
    QWidget(parent)
{
    // Setup a unique identifier for this editor instance.
    QUuid uuid;
    this->uuid =  uuid.createUuid().toString().replace("{","").replace("}","");

//    this->setStyleSheet("margins:0px;");
    QHBoxLayout *line1Layout = new QHBoxLayout();
    QVBoxLayout *layout = new QVBoxLayout();   // Note content layout


    // Setup the alarm button & display
    alarmText.setStyleSheet("QPushButton {background-color: transparent; border-radius: 0px;}");
    connect(alarmButton.setAction, SIGNAL(triggered()), this, SLOT(alarmSet()));
    connect(alarmButton.clearAction, SIGNAL(triggered()), this, SLOT(alarmClear()));
    connect(alarmButton.doneAction, SIGNAL(triggered()), this, SLOT(alarmCompleted()));
    connect(&alarmButton.menu, SIGNAL(aboutToShow()), this, SLOT(alarmMenuActivated()));

    // Setup line #1 of the window.  The text & notebook
    connect(&alarmText, SIGNAL(clicked()), this, SLOT(alarmCompleted()));
    layout->addLayout(line1Layout);
    line1Layout->addWidget(&noteTitle,20);
    line1Layout->addWidget(&alarmText);
    line1Layout->addWidget(&alarmButton);
    line1Layout->addWidget(&notebookMenu,4);
    line1Layout->addWidget(&expandButton);


    // Add the second layout display
    layout->addLayout(&line2Layout);
    line2Layout.addWidget(&urlEditor,1);
    line2Layout.addWidget(&tagEditor, 3);

    // Add the third layout display
    layout->addLayout(&line3Layout);
    line3Layout.addWidget(&dateEditor);


    editor = new NWebView(this);
    editor->setTitleEditor(&noteTitle);
    setupToolBar();
    layout->addWidget(buttonBar);

    // setup the source editor
    sourceEdit = new QTextEdit(this);
    sourceEdit->setVisible(false);
    sourceEdit->setTabChangesFocus(true);


    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    global.getGuiFont(font);
    sourceEdit->setFont(global.getGuiFont(font));
    sourceEditorTimer = new QTimer();
    connect(sourceEditorTimer, SIGNAL(timeout()), this, SLOT(setSource()));

    // addthe actual note editor & source view
    QSplitter *editorSplitter = new QSplitter(Qt::Vertical, this);
    editorSplitter->addWidget(editor);
    editorSplitter->addWidget(sourceEdit);
    layout->addWidget(editorSplitter);
    setLayout(layout);
    layout->setMargin(0);

    findReplace = new FindReplace();
    layout->addWidget(findReplace);
    findReplace->setVisible(false);

    connect(findReplace->nextButton, SIGNAL(clicked()), this, SLOT(findNextInNote()));
    connect(findReplace->findLine, SIGNAL(returnPressed()), this, SLOT(findNextInNote()));
    connect(findReplace->prevButton, SIGNAL(clicked()), this, SLOT(findPrevInNote()));
    connect(findReplace->replaceButton, SIGNAL(clicked()), this, SLOT(findReplaceInNotePressed()));
    connect(findReplace->replaceAllButton, SIGNAL(clicked()), this, SLOT(findReplaceAllInNotePressed()));
    connect(findReplace->closeButton, SIGNAL(clicked()), this, SLOT(findReplaceWindowHidden()));


    // Setup shortcuts
    focusNoteShortcut = new QShortcut(this);
    setupShortcut(focusNoteShortcut, "Focus_Note");
    connect(focusNoteShortcut, SIGNAL(activated()), this, SLOT(focusNote()));
    focusTitleShortcut = new QShortcut(this);
    setupShortcut(focusTitleShortcut, "Focus_Title");
    connect(focusTitleShortcut, SIGNAL(activated()), this, SLOT(focusTitle()));
    insertDatetimeShortcut = new QShortcut(this);
    setupShortcut(insertDatetimeShortcut, "Insert_DateTime");
    connect(insertDatetimeShortcut, SIGNAL(activated()), this, SLOT(insertDatetime()));
    copyNoteUrlShortcut = new QShortcut(this);
    setupShortcut(copyNoteUrlShortcut, "Edit_Copy_Note_Url");
    connect(copyNoteUrlShortcut, SIGNAL(activated()), this, SLOT(copyNoteUrl()));


    // Setup the signals
    connect(&expandButton, SIGNAL(stateChanged(int)), this, SLOT(changeExpandState(int)));
    connect(&notebookMenu, SIGNAL(notebookChanged()), this, SLOT(sendNotebookUpdateSignal()));
    connect(&urlEditor, SIGNAL(textUpdated()), this, SLOT(sendUrlUpdateSignal()));
    connect(&noteTitle, SIGNAL(titleChanged()), this, SLOT(sendTitleUpdateSignal()));
    connect(&dateEditor.authorEditor, SIGNAL(textUpdated()), this, SLOT(sendAuthorUpdateSignal()));
    connect(&dateEditor.locationEditor, SIGNAL(clicked()), this, SLOT(sendLocationUpdateSignal()));
    connect(&dateEditor.createdDate, SIGNAL(editingFinished()), this, SLOT(sendDateCreatedUpdateSignal()));
    connect(&dateEditor.subjectDate, SIGNAL(editingFinished()), this, SLOT(sendDateSubjectUpdateSignal()));
    connect(&dateEditor, SIGNAL(valueChanged()), this, SLOT(sendDateUpdateSignal()));
    connect(&tagEditor, SIGNAL(tagsUpdated()), this, SLOT(sendTagUpdateSignal()));
    connect(&tagEditor, SIGNAL(newTagCreated(qint32)), this, SLOT(newTagAdded(qint32)));
    connect(editor, SIGNAL(noteChanged()), this, SLOT(noteContentUpdated()));
    connect(sourceEdit, SIGNAL(textChanged()), this, SLOT(noteSourceUpdated()));
    connect(editor, SIGNAL(htmlEditAlert()), this, SLOT(noteContentEdited()));
    connect(editor->page(), SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
    connect(editor->page(), SIGNAL(microFocusChanged()), this, SLOT(microFocusChanged()));

    editor->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(editor->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(exposeToJavascript()));
    connect(editor->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), editor, SLOT(exposeToJavascript()));

    editor->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    factory = new PluginFactory(this);
    editor->page()->setPluginFactory(factory);

    buttonBar->setupVisibleButtons();

    printPage = new QTextEdit();
    printPage->setVisible(false);
    //connect(printPage, SIGNAL(loadFinished(bool)), this, SLOT(printReady(bool)));

    printPreviewPage = new QTextEdit();
    printPreviewPage->setVisible(false);

    hammer = new Thumbnailer(global.db);
    lid = -1;
    thumbnailer = NULL;


    //Setup shortcuts for context menu
    removeFormattingShortcut = new QShortcut(this);
    this->setupShortcut(removeFormattingShortcut, "Edit_Remove_Formatting");
    connect(removeFormattingShortcut, SIGNAL(activated()), this, SLOT(removeFormatButtonPressed()));
    //removeFormattingShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    insertHtmlEntitiesShortcut = new QShortcut(this);
    this->setupShortcut(insertHtmlEntitiesShortcut, QString("Edit_Insert_Html_Entities"));
    connect(insertHtmlEntitiesShortcut, SIGNAL(activated()),this, SLOT(insertHtmlEntities()));
    //insertHtmlEntitiesShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    encryptTextShortcut = new QShortcut(this);
    this->setupShortcut(encryptTextShortcut, QString("Edit_Encrypt_Text"));
    connect(encryptTextShortcut, SIGNAL(activated()),this, SLOT(encryptButtonPressed()));
    //encryptTextShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    insertHyperlinkShortcut = new QShortcut(this);
    this->setupShortcut(insertHyperlinkShortcut, QString("Edit_Insert_Hyperlink"));
    connect(insertHyperlinkShortcut, SIGNAL(activated()),this, SLOT(insertLinkButtonPressed()));
    //insertHyperlinkShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    insertQuicklinkShortcut = new QShortcut(this);
    this->setupShortcut(insertQuicklinkShortcut, QString("Edit_Insert_QuickLink"));
    connect(insertQuicklinkShortcut, SIGNAL(activated()),this, SLOT(insertQuickLinkButtonPressed()));
    //insertQuicklinkShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    removeHyperlinkShortcut = new QShortcut(this);
    this->setupShortcut(removeHyperlinkShortcut, QString("Edit_Remove_Hyperlink"));
    connect(removeHyperlinkShortcut, SIGNAL(activated()),this, SLOT(removeLinkButtonPressed()));
    //removeHyperlinkShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    attachFileShortcut = new QShortcut(this);
    this->setupShortcut(attachFileShortcut, QString("Edit_Attach_File"));
    connect(attachFileShortcut, SIGNAL(activated()),this, SLOT(attachFile()));
    //attachFileShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    insertLatexShortcut = new QShortcut(this);
    this->setupShortcut(insertLatexShortcut, QString("Edit_Insert_Latex"));
    connect(insertLatexShortcut, SIGNAL(activated()),this, SLOT(insertLatexButtonPressed()));


    // Restore the expand/collapse state
    global.settings->beginGroup("SaveState");
    int expandButton = global.settings->value("ExpandButton", EXPANDBUTTON_1).toInt();
    global.settings->endGroup();
    this->expandButton.setState(expandButton);

    connect(&focusTimer, SIGNAL(timeout()), this, SLOT(focusCheck()));
    focusTimer.setInterval(100);
    focusTimer.start();

    hunspellInterface = NULL;
}



// Setup the toolbar window of the editor
void NBrowserWindow::setupToolBar() {
    buttonBar = new EditorButtonBar();

    // Toolbar action
    connect(buttonBar->undoButtonAction, SIGNAL(triggered()), this, SLOT(undoButtonPressed()));
    connect(buttonBar->undoButtonShortcut, SIGNAL(activated()), this, SLOT(undoButtonPressed()));

    connect(buttonBar->redoButtonAction, SIGNAL(triggered()), this, SLOT(redoButtonPressed()));
    connect(buttonBar->redoButtonShortcut, SIGNAL(activated()), this, SLOT(redoButtonPressed()));

    connect(buttonBar->cutButtonAction, SIGNAL(triggered()), this, SLOT(cutButtonPressed()));
    connect(buttonBar->cutButtonShortcut, SIGNAL(activated()), this, SLOT(cutButtonPressed()));

    connect(buttonBar->copyButtonAction, SIGNAL(triggered()), this, SLOT(copyButtonPressed()));
    connect(buttonBar->copyButtonShortcut, SIGNAL(activated()), this, SLOT(copyButtonPressed()));

    connect(buttonBar->pasteButtonAction, SIGNAL(triggered()), this, SLOT(pasteButtonPressed()));
    //connect(buttonBar->pasteButtonShortcut, SIGNAL(activated()), this, SLOT(pasteButtonPressed()));  // Handled via NWebView

    connect(buttonBar->removeFormatButtonAction, SIGNAL(triggered()), this, SLOT(removeFormatButtonPressed()));
    connect(buttonBar->removeFormatButtonShortcut, SIGNAL(activatedAmbiguously()), this, SLOT(removeFormatButtonPressed()));

    connect(buttonBar->boldButtonWidget, SIGNAL(clicked()), this, SLOT(boldButtonPressed()));
    connect(buttonBar->boldButtonShortcut, SIGNAL(activated()), this, SLOT(boldButtonPressed()));

    connect(buttonBar->italicButtonWidget, SIGNAL(clicked()), this, SLOT(italicsButtonPressed()));
    connect(buttonBar->italicButtonShortcut, SIGNAL(activated()), this, SLOT(italicsButtonPressed()));

    connect(buttonBar->underlineButtonWidget, SIGNAL(clicked()), this, SLOT(underlineButtonPressed()));
    connect(buttonBar->underlineButtonShortcut, SIGNAL(activated()), this, SLOT(underlineButtonPressed()));

    connect(buttonBar->leftJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignLeftButtonPressed()));
    connect(buttonBar->leftJustifyButtonShortcut, SIGNAL(activated()), this, SLOT(alignLeftButtonPressed()));

    connect(buttonBar->rightJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignRightButtonPressed()));
    connect(buttonBar->rightJustifyButtonShortcut, SIGNAL(activated()), this, SLOT(alignRightButtonPressed()));

    connect(buttonBar->centerJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignCenterButtonPressed()));
    connect(buttonBar->centerJustifyButtonShortcut, SIGNAL(activated()), this, SLOT(alignCenterButtonPressed()));

    connect(buttonBar->fullJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignFullButtonPressed()));
    connect(buttonBar->fullJustifyButtonShortcut, SIGNAL(activated()), this, SLOT(alignFullButtonPressed()));

    connect(buttonBar->strikethroughButtonAction, SIGNAL(triggered()), this, SLOT(strikethroughButtonPressed()));
    connect(buttonBar->strikethroughButtonShortcut, SIGNAL(activated()), this, SLOT(strikethroughButtonPressed()));

    connect(buttonBar->subscriptButtonAction, SIGNAL(triggered()), this, SLOT(subscriptButtonPressed()));
    connect(buttonBar->subscriptButtonShortcut, SIGNAL(activated()), this, SLOT(subscriptButtonPressed()));

    connect(buttonBar->superscriptButtonAction, SIGNAL(triggered()), this, SLOT(superscriptButtonPressed()));
    connect(buttonBar->superscriptButtonShortcut, SIGNAL(activated()), this, SLOT(superscriptButtonPressed()));

    connect(buttonBar->hlineButtonAction, SIGNAL(triggered()), this, SLOT(horizontalLineButtonPressed()));
    connect(buttonBar->hlineButtonShortcut, SIGNAL(activated()), this, SLOT(horizontalLineButtonPressed()));

    connect(buttonBar->shiftRightButtonAction, SIGNAL(triggered()), this, SLOT(shiftRightButtonPressed()));
    connect(buttonBar->shiftRightButtonShortcut, SIGNAL(activated()), this, SLOT(shiftRightButtonPressed()));

    connect(buttonBar->shiftLeftButtonAction, SIGNAL(triggered()), this, SLOT(shiftLeftButtonPressed()));
    connect(buttonBar->shiftLeftButtonShortcut, SIGNAL(activated()), this, SLOT(shiftLeftButtonPressed()));

    connect(buttonBar->bulletListButtonAction, SIGNAL(triggered()), this, SLOT(bulletListButtonPressed()));
    connect(buttonBar->bulletListButtonShortcut, SIGNAL(activated()), this, SLOT(bulletListButtonPressed()));

    connect(buttonBar->numberListButtonAction, SIGNAL(triggered()), this, SLOT(numberListButtonPressed()));
    connect(buttonBar->numberListButtonShortcut, SIGNAL(activated()), this, SLOT(numberListButtonPressed()));

    connect(buttonBar->todoButtonAction, SIGNAL(triggered()), this, SLOT(todoButtonPressed()));
    connect(buttonBar->todoButtonShortcut, SIGNAL(activated()), this, SLOT(todoButtonPressed()));

    connect(buttonBar->spellCheckButtonAction, SIGNAL(triggered()), this, SLOT(spellCheckPressed()));
    connect(buttonBar->spellCheckButtonShortcut, SIGNAL(activated()), this, SLOT(spellCheckPressed()));

    connect(buttonBar->fontSizes, SIGNAL(currentIndexChanged(int)), this, SLOT(fontSizeSelected(int)));

    connect(buttonBar->fontNames, SIGNAL(currentIndexChanged(int)), this, SLOT(fontNameSelected(int)));

    connect(buttonBar->fontColorButtonWidget, SIGNAL(clicked()), this, SLOT(fontColorClicked()));
    //connect(fontColorButtonShortcut, SIGNAL(activated()), this, SLOT(fontColorClicked()));

    connect(buttonBar->fontColorMenuWidget->getMenu(), SIGNAL(triggered(QAction*)), this, SLOT(fontColorClicked()));

    connect(buttonBar->highlightColorButtonWidget, SIGNAL(clicked()), this, SLOT(fontHighlightClicked()));
    //connect(fontHighlightColorShortcut, SIGNAL(activated()), this, SLOT(fontHighlightClicked()));
    connect(buttonBar->highlightColorAction, SIGNAL(triggered()), this, SLOT(fontHighlightClicked()));

    connect(buttonBar->highlightColorMenuWidget->getMenu(), SIGNAL(triggered(QAction*)), this, SLOT(fontHighlightClicked()));

    connect(buttonBar->insertTableButtonAction, SIGNAL(triggered()), this, SLOT(insertTableButtonPressed()));
    connect(buttonBar->insertTableButtonShortcut, SIGNAL(activated()), this, SLOT(insertTableButtonPressed()));

    connect(buttonBar->htmlEntitiesButtonAction, SIGNAL(triggered()), this, SLOT(insertHtmlEntities()));
    connect(buttonBar->htmlEntitiesButtonShortcut, SIGNAL(activated()), this, SLOT(insertHtmlEntities()));

    connect(buttonBar->insertDatetimeButtonAction, SIGNAL(triggered()), this, SLOT(insertDatetime()));
    connect(buttonBar->insertDatetimeButtonWidget,SIGNAL(clicked()), this, SLOT(insertDatetime()));
    connect(buttonBar->insertDatetimeButtonShortcut, SIGNAL(activated()), this, SLOT(insertDatetime()));
}




// Load any shortcut keys
void NBrowserWindow::setupShortcut(QShortcut *action, QString text) {
    if (!global.shortcutKeys->containsAction(&text))
        return;
    QKeySequence key(global.shortcutKeys->getShortcut(&text));
    action->setKey(key);
}


// Load the note content into the window
void NBrowserWindow::setContent(qint32 lid) {
    QLOG_DEBUG() << "Setting note contents to " << lid;

    // First, make sure we have a valid lid
    if (lid == -1) {
        blockSignals(true);
        setReadOnly(true);
        clear();
        blockSignals(false);
        return;
    }

    // If we are already updating this note, we don't do anything
    QLOG_DEBUG() << "this.lid:" << this->lid << " " << lid;
    if (lid == this->lid)
        return;

    bool hasFocus = false;
    if (this->editor->hasFocus())
        hasFocus = true;

    QLOG_DEBUG() << "editor is dirty";
    if (this->editor->isDirty)
        this->saveNoteContent();

    // let's load the new note
    this->lid = lid;
    this->editor->isDirty = false;

    NoteTable noteTable(global.db);
    Note n;

    QLOG_DEBUG() << "Getting note";
    bool rc = noteTable.get(n, this->lid, false, false);
    if (!rc)
        return;

    QByteArray content;
    bool inkNote = false;
    bool readOnly = false;

    // If we are searching, we never pull from the cache since the search string may
    // have changed since the last time.
    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (criteria->isSearchStringSet() && criteria->getSearchString().trimmed() != "")
        global.cache.remove(lid);

    QLOG_DEBUG() << "Checking if note is in cache";
    if (global.cache.contains(lid)) {
        QLOG_DEBUG() << "Fetching from cache";
        NoteCache *c = global.cache[lid];
        if (c == NULL || c->noteContent == (char*)NULL) {
            QLOG_DEBUG() << "Invalid note found in cache.  Removing it.";
            global.cache.remove(lid);
        } else {
            QLOG_DEBUG() << "Setting content from cache.";
            content = c->noteContent;
            readOnly = c->isReadOnly;
            inkNote = c->isInkNote;
        }
    }

    if (!global.cache.contains(lid)) {
        QLOG_DEBUG() << "Note not in cache";
        NoteFormatter formatter;
        if (criteria->isSearchStringSet())
            formatter.setHighlightText(criteria->getSearchString());
        formatter.setNote(n, global.pdfPreview);
        //formatter.setHighlight();
        QLOG_DEBUG() << "rebuilding note HTML";
        content = formatter.rebuildNoteHTML();
        if (!criteria->isSearchStringSet()) {
            QLOG_DEBUG() << "criteria search string set";
            NoteCache *newCache = new NoteCache();
            newCache->isReadOnly = formatter.readOnly;
            newCache->isInkNote = formatter.inkNote;
            newCache->noteContent = content;
            QLOG_DEBUG() << "adding to cache";
            global.cache.insert(lid, newCache);
        }
        readOnly = formatter.readOnly;
        inkNote = formatter.inkNote;
    }

    setReadOnly(readOnly);

    QLOG_DEBUG() << "Setting up note title";
    noteTitle.setTitle(lid, n.title, n.title);
    dateEditor.setNote(lid, n);
    QWebSettings::setMaximumPagesInCache(0);
    QWebSettings::setObjectCacheCapacities(0, 0, 0);
    QLOG_DEBUG() << "Setting editor contents";
    editor->setContent(content);
    // is this an ink note?
    if (inkNote)
        editor->page()->setContentEditable(false);

    // Setup the alarm
    NoteAttributes attributes;
    QLOG_DEBUG() << "Setting attributes";
    if (n.attributes.isSet())
        attributes = n.attributes;
    if (attributes.reminderTime.isSet()) {
        Timestamp t;
        if (attributes.reminderTime.isSet())
            t = attributes.reminderTime;
        QFont f = alarmText.font();
        if (attributes.reminderDoneTime.isSet()) {
            f.setStrikeOut(true);
        } else {
            f.setStrikeOut(false);
        }
        alarmText.setFont(f);
        alarmText.setVisible(true);
        QDateTime atime;
        atime.setMSecsSinceEpoch(t);
        if (atime.date() == QDate::currentDate())
            alarmText.setText(tr("Today"));
        else if (atime.date() == QDate::currentDate().addDays(+1))
            alarmText.setText(tr("Tomorrow"));
        else if (atime.date() == QDate::currentDate().addDays(-1))
            alarmText.setText(tr("Yesterday"));
        else
            alarmText.setText(atime.date().toString(global.dateFormat));


    } else {
        alarmText.setText("");
        alarmText.setVisible(false);
    }


    // Set the tag names
    QLOG_DEBUG() << "Setting tags";
    tagEditor.clear();
    QStringList names;
    QList<QString> tagNames;
    if (n.tagNames.isSet())
        tagNames = n.tagNames;
    for (int i=0; i<tagNames.size(); i++) {
        names << tagNames[i];
    }
    tagEditor.setTags(names);
    tagEditor.setCurrentLid(lid);
    NotebookTable notebookTable(global.db);
    qint32 notebookLid = notebookTable.getLid(n.notebookGuid);
    LinkedNotebookTable linkedTable(global.db);
    if (linkedTable.exists(notebookLid))
        tagEditor.setAccount(notebookLid);
    else
        tagEditor.setAccount(0);

    QLOG_DEBUG() << "Setting notebook";
    //this->lid = lid;
    notebookMenu.setCurrentNotebook(lid, n);
    QLOG_DEBUG() << "Setting URL";
    urlEditor.setUrl(lid, "");
    NoteAttributes na;
    QLOG_DEBUG() << "Setting note attributes";
    if (n.attributes.isSet()) {
        na = n.attributes;
        if (na.sourceURL.isSet()) {
            QLOG_DEBUG() << "Setting sourceUrl";
            urlEditor.setUrl(lid, na.sourceURL);
        }
    }

    QLOG_DEBUG() << "Calling set source";
    setSource();

    if (criteria->isSearchStringSet()) {
        QStringList list = criteria->getSearchString().split(" ");
        for (int i=0; i<list.size(); i++) {
            editor->page()->findText(list[i], QWebPage::HighlightAllOccurrences);
        }
    }

    QLOG_DEBUG() << "Checking thumbanail";
    if (hammer->idle && noteTable.isThumbnailNeeded(this->lid)) {
        hammer->render(this->lid);
    }
    this->setEditorStyle();

    if (hasFocus)
        this->editor->setFocus();
    QLOG_DEBUG() << "Exiting setContent";
}


void NBrowserWindow::setReadOnly(bool readOnly) {
    isReadOnly = readOnly;
    if (readOnly || global.disableEditing) {
        noteTitle.setFocusPolicy(Qt::NoFocus);
        tagEditor.setEnabled(false);
        buttonBar->setVisible(false);
        tagEditor.setFocusPolicy(Qt::NoFocus);
        //authorEditor.setFocusPolicy(Qt::NoFocus);
        //locationEditor.setFocusPolicy(Qt::NoFocus);
        urlEditor.setFocusPolicy(Qt::NoFocus);
        notebookMenu.setEnabled(false);
        dateEditor.setEnabled(false);
        editor->page()->setContentEditable(false);
        alarmButton.setEnabled(false);
        return;
    }
    noteTitle.setFocusPolicy(Qt::StrongFocus);
    tagEditor.setEnabled(true);
    tagEditor.setFocusPolicy(Qt::StrongFocus);
    urlEditor.setFocusPolicy(Qt::StrongFocus);
    notebookMenu.setEnabled(true);
    dateEditor.setEnabled(true);
    editor->page()->setContentEditable(true);
    alarmButton.setEnabled(true);

}




// Show / hide various note attributes depending upon what the user
// has clicked
void NBrowserWindow::changeExpandState(int value) {
    switch (value) {
    case EXPANDBUTTON_1:
        urlEditor.hide();
        tagEditor.hide();
        dateEditor.hide();
        break;
    case EXPANDBUTTON_2:
        urlEditor.show();
        tagEditor.show();
        break;
    case EXPANDBUTTON_3:
        urlEditor.show();
        tagEditor.show();
        dateEditor.show();
        break;
    }
    global.settings->beginGroup("SaveState");
    global.settings->setValue("ExpandButton", value);
    global.settings->endGroup();
}








// Send a signal that a tag has been added to a note
void NBrowserWindow::newTagAdded(qint32 lid) {
    emit(tagAdded(lid));
}



// Add a tag to a note
void NBrowserWindow::addTagName(qint32 lid) {
    TagTable table(global.db);
    Tag t;
    table.get(t, lid);
    tagEditor.addTag(t.name);
}




// Rename a tag in a note.
void NBrowserWindow::tagRenamed(qint32 lid, QString oldName, QString newName) {
    tagEditor.tagRenamed(lid, oldName, newName);
}



// Remove a tag in a note
void NBrowserWindow::tagDeleted(qint32 lid, QString name) {
    Q_UNUSED(lid);  /* suppress unused */
    tagEditor.removeTag(name);
}



// A notebook was renamed
void NBrowserWindow::notebookRenamed(qint32 lid, QString oldName, QString newName) {
    Q_UNUSED(lid);  /* suppress unused */
    Q_UNUSED(oldName);  /* suppress unused */
    Q_UNUSED(newName)  /* suppress unused */
    notebookMenu.reloadData();
}




// A notebook was deleted
void NBrowserWindow::notebookDeleted(qint32 lid, QString name) {
    Q_UNUSED(lid);  /* suppress unused */
    Q_UNUSED(name); /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was renamed
void NBrowserWindow::stackRenamed(QString oldName, QString newName) {
    Q_UNUSED(oldName);  /* suppress unused */
    Q_UNUSED(newName);  /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was deleted
void NBrowserWindow::stackDeleted(QString name) {
    Q_UNUSED(name);  /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was added
void NBrowserWindow::stackAdded(QString name) {
    Q_UNUSED(name);  /* suppress unused */
    notebookMenu.reloadData();
}



// A notebook was added
void NBrowserWindow::notebookAdded(qint32 lid) {
    Q_UNUSED(lid);  /* suppress unused */
    notebookMenu.reloadData();
}


// A note was synchronized with Evernote's servers
void NBrowserWindow::noteSyncUpdate(qint32 lid) {
    if (lid != this->lid || editor->isDirty)
        return;
    setContent(lid);
}




// A note's content was updated
void NBrowserWindow::noteContentUpdated() {
    if (editor->isDirty) {
        NoteTable noteTable(global.db);
        noteTable.setDirty(this->lid, true);
        editor->isDirty = false;
        qint64 dt = QDateTime::currentMSecsSinceEpoch();
        emit(noteUpdated(this->lid));
        emit(updateNoteList(this->lid, NOTE_TABLE_DATE_UPDATED_POSITION, dt));
    }
}


// Save the note's content
void NBrowserWindow::saveNoteContent() {
    microFocusChanged();

    if (this->editor->isDirty) {
        //QString contents = editor->editorPage->mainFrame()->toHtml();
        QString contents = editor->editorPage->mainFrame()->documentElement().toOuterXml();
        EnmlFormatter formatter;
        formatter.setHtml(contents);
        formatter.rebuildNoteEnml();
        if (formatter.formattingError) {
            QMessageBox::information(this, tr("Unable to Save"), QString(tr("Unable to save this note.  Either tidy isn't installed or the note is too complex to save.")));
            return;
        }


        // get a list of lids found in the note.
        // Purge anything that is no longer needed.
        QList<qint32> validLids = formatter.resources;
        QList<qint32> oldLids;
        ResourceTable resTable(global.db);
        resTable.getResourceList(oldLids, lid);

        QLOG_DEBUG() << "Valid Resource  LIDS:";
        for (int i=0; i<validLids.size(); i++) {
            QLOG_DEBUG() << " * " << i << " : " << validLids[i];
        }


        QLOG_DEBUG() << "Old Resource  LIDS:";
        for (int i=0; i<oldLids.size(); i++) {
            QLOG_DEBUG() << " * " << i << " : " << oldLids[i];
        }


        for (int i=0; i<oldLids.size(); i++) {
            if (!validLids.contains(oldLids[i])) {
                QLOG_DEBUG() << "Expunging old lid " << oldLids[i];
                resTable.expunge(oldLids[i]);
            }
        }

        QLOG_DEBUG() << "Updating note content";
        NoteTable table(global.db);
        table.updateNoteContent(lid, formatter.getEnml());
        editor->isDirty = false;
        if (thumbnailer == NULL)
            thumbnailer = new Thumbnailer(global.db);
        QLOG_DEBUG() << "Beginning thumbnail";
        thumbnailer->render(lid);
        QLOG_DEBUG() << "Thumbnail compleded";

        NoteCache* cache = global.cache[lid];
        if (cache != NULL) {
            QLOG_DEBUG() << "Updating cache";
            QByteArray b;
            b.append(contents);
            cache->noteContent = b;
            global.cache.remove(lid);
//            global.cache.insert(lid, cache);
        }
        QLOG_DEBUG() << "Leaving saveNoteContent()";
        // Make sure the thumnailer is done
        //while(!thumbnailer.idle);
    }
}



// The undo edit button was pressed
void NBrowserWindow::undoButtonPressed() {
    this->editor->triggerPageAction(QWebPage::Undo);
    this->editor->setFocus();
    microFocusChanged();
}



// The redo edit button was pressed
void NBrowserWindow::redoButtonPressed() {
    this->editor->triggerPageAction(QWebPage::Redo);
    this->editor->setFocus();
    microFocusChanged();
}


// The cut button was pressed
void NBrowserWindow::cutButtonPressed() {
    this->editor->triggerPageAction(QWebPage::Cut);
    this->editor->setFocus();
    microFocusChanged();
}


// The copy button was pressed
void NBrowserWindow::copyButtonPressed() {
//    editor->downloadImageAction()->setEnabled(true);
//    selectedFileName = f;
//    selectedFileLid = l.toInt();

    // If we have text selected
    if (this->editor->selectedText().trimmed() != "") {
        this->editor->triggerPageAction(QWebPage::Copy);
        this->editor->setFocus();
    } else {
        // If we have an image selected, we copy it to the clipboard.
        if (editor->downloadImageAction()->isEnabled()) {
            QString fileName = global.fileManager.getDbaDirPath()+selectedFileName;
            QApplication::clipboard()->setPixmap(QPixmap(fileName));
        }
    }

    microFocusChanged();

}


// Build URL from pasted text
QString NBrowserWindow::buildPasteUrl(QString url) {
    if (url.toLower().startsWith("http://") ||
        url.toLower().startsWith("https://") ||
        url.toLower().startsWith("mailto://") ||
        url.toLower().startsWith("mailto:") ||
        url.toLower().startsWith("ftp://")) {
        QString newUrl = QString("<a href=\"") +QApplication::clipboard()->text()
                +QString("\" title=\"") +url
                +QString("\" >") +url +QString("</a>");
        return newUrl;
    }
    return url;
}


// The paste button was pressed
void NBrowserWindow::pasteButtonPressed() {
    if (forceTextPaste) {
        pasteWithoutFormatButtonPressed();
        return;
    }

    const QMimeData *mime = QApplication::clipboard()->mimeData();

    if (mime->hasImage()) {
        editor->setFocus();
        insertImage(mime);
        editor->setFocus();
        return;
    }

    QLOG_DEBUG() << "Have URL?: " << mime->hasUrls();

    if (mime->hasUrls()) {
        QList<QUrl> urls = mime->urls();
        for (int i=0; i<urls.size(); i++) {
            QLOG_DEBUG() << urls[i].toString();
            if (urls[i].toString().startsWith("file://")) {
// Windows Check
#ifndef _WIN32
                QString fileName = urls[i].toString().mid(7);
#else
                QString fileName = urls[i].toString().mid(8);
#endif  // End windows check
                attachFileSelected(fileName);
                this->editor->triggerPageAction(QWebPage::InsertParagraphSeparator);
            }

            // If inserting a URL
            if (urls[i].toString().toLower().startsWith("https://") ||
                    urls[i].toString().toLower().startsWith("http://") ||
                    urls[i].toString().toLower().startsWith("ftp://") ||
                    urls[i].toString().toLower().startsWith("mailto:")) {
                QString url = this->buildPasteUrl(urls[i].toString());
                QString script = QString("document.execCommand('insertHtml', false, '%1');").arg(url);
                editor->page()->mainFrame()->evaluateJavaScript(script);
            }
        }

        this->editor->setFocus();
        microFocusChanged();
        return;
    }
    QLOG_DEBUG() << "Has HTML:" << mime->hasHtml() << " " << mime->html();
    QLOG_DEBUG() << "Has Color:" << mime->hasColor();
    QLOG_DEBUG() << "Has Url:" << mime->hasUrls();

    if (mime->hasText()) {
        QString urltext = mime->text();
        QLOG_DEBUG() << "Url:" << urltext;

        if (urltext.toLower().startsWith("https://") ||
            urltext.toLower().startsWith("http://") ||
            urltext.toLower().startsWith("ftp://") || \
            urltext.toLower().startsWith("mailto:")) {
            QString url = this->buildPasteUrl(urltext);
            QString script = QString("document.execCommand('insertHtml', false, '%1');").arg(url);
            editor->page()->mainFrame()->evaluateJavaScript(script);
            return;
        }


        if (urltext.toLower().mid(0,17) == "evernote:///view/") {
            urltext = urltext.mid(17);
            int pos = urltext.indexOf("/");
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            QString guid = urltext.mid(0,pos);
            urltext = urltext.mid(pos);
            pos = urltext.indexOf("/");
            QString locguid = urltext.mid(pos);

            Note n;
            bool goodrc = false;
            NoteTable ntable(global.db);
            goodrc = ntable.get(n, guid,false, false);
            if (!goodrc)
                goodrc = ntable.get(n,locguid,false, false);

            // If we have a good return, then we can paste the link, otherwise we fall out
            // to a normal paste.
            if (goodrc) {
                QString url = QString("<a href=\"%1\" title=\"%2\">%3</a>").arg(QApplication::clipboard()->text(), n.title, n.title);
                QLOG_DEBUG() << "HTML to insert:" << url;
                QString script = QString("document.execCommand('insertHtml', false, '%1');").arg(url);
                editor->page()->mainFrame()->evaluateJavaScript(script);
                return;
            } else {
                QLOG_ERROR() << "Error retrieving note";
            }
        }
    }


    this->editor->triggerPageAction(QWebPage::Paste);
    this->editor->setFocus();
    microFocusChanged();
}




// The paste button was pressed
void NBrowserWindow::selectAllButtonPressed() {
    this->editor->triggerPageAction(QWebPage::SelectAll);
    this->editor->setFocus();
    microFocusChanged();
}



// The paste without mime format was pressed
void NBrowserWindow::pasteWithoutFormatButtonPressed() {
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (!mime->hasText())
        return;
    QString text = mime->text();
    QApplication::clipboard()->clear();
    QApplication::clipboard()->setText(text, QClipboard::Clipboard);
    this->editor->triggerPageAction(QWebPage::Paste);

    // This is done because pasting into an encryption block
    // can cause multiple cells (which can't happen).  It
    // just goes through the table, extracts the data, &
    // puts it back as one table cell.
    if (insideEncryption) {
        QString js = QString( "function fixEncryption() { ")
                +QString("   var selObj = window.getSelection();")
                +QString("   var selRange = selObj.getRangeAt(0);")
                +QString("   var workingNode = window.getSelection().anchorNode;")
                +QString("   while(workingNode != null && workingNode.nodeName.toLowerCase() != 'table') { ")
                +QString("           workingNode = workingNode.parentNode;")
                +QString("   } ")
                +QString("   workingNode.innerHTML = window.browserWindow.fixEncryptionPaste(workingNode.innerHTML);")
                +QString("} fixEncryption();");
        editor->page()->mainFrame()->evaluateJavaScript(js);
    }

    this->editor->setFocus();
    microFocusChanged();
}

// This basically removes all the table tags and returns just the contents.
// This is called by JavaScript to fix encryption pastes.
QString NBrowserWindow::fixEncryptionPaste(QString data) {
    data = data.replace("<tbody>", "");
    data = data.replace("</tbody>", "");
    data = data.replace("<tr>", "");
    data = data.replace("</tr>", "");
    data = data.replace("<td>", "");
    data = data.replace("</td>", "<br>");
    data = data.replace("<br><br>", "<br>");
    return QString("<tbody><tr><td>")+data+QString("</td></tr></tbody>");
}



// The bold button was pressed / toggled
void NBrowserWindow::boldButtonPressed() {
    QAction *action = editor->page()->action(QWebPage::ToggleBold);
    action->activate(QAction::Trigger);
    this->editor->setFocus();
    microFocusChanged();
}



// The toggled button was pressed/toggled
void NBrowserWindow::italicsButtonPressed() {
    QAction *action = editor->page()->action(QWebPage::ToggleItalic);
    action->activate(QAction::Trigger);
    this->editor->setFocus();
    microFocusChanged();
}


// The underline button was toggled
void NBrowserWindow::underlineButtonPressed() {
    this->editor->triggerPageAction(QWebPage::ToggleUnderline);
    this->editor->setFocus();
    microFocusChanged();
}



// The underline button was toggled
void NBrowserWindow::removeFormatButtonPressed() {
    this->editor->triggerPageAction(QWebPage::RemoveFormat);
    this->editor->setFocus();
    microFocusChanged();
}



// The strikethrough button was pressed
void NBrowserWindow::strikethroughButtonPressed() {
    this->editor->triggerPageAction(QWebPage::ToggleStrikethrough);
    this->editor->setFocus();
    microFocusChanged();
}



// The horizontal line button was pressed
void NBrowserWindow::horizontalLineButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('insertHorizontalRule', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The center align button was pressed
void NBrowserWindow::alignCenterButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('JustifyCenter', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The full align button was pressed
void NBrowserWindow::alignFullButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('JustifyFull', false, '');");
    editor->setFocus();
    microFocusChanged();
}


// The left align button was pressed
void NBrowserWindow::alignLeftButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('JustifyLeft', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The align right button was pressed
void NBrowserWindow::alignRightButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('JustifyRight', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The shift right button was pressed
void NBrowserWindow::shiftRightButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('indent', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The shift left button was pressed
void NBrowserWindow::shiftLeftButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('outdent', false, '');");
    editor->setFocus();
    microFocusChanged();
}




// The number list button was pressed
void NBrowserWindow::numberListButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('InsertOrderedList', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The bullet list button was pressed
void NBrowserWindow::bulletListButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('InsertUnorderedList', false, '');");
    editor->setFocus();
    microFocusChanged();
}


void NBrowserWindow::contentChanged() {
    this->editor->isDirty = true;
    saveNoteContent();
    this->sendDateUpdateSignal();
}

// The todo button was pressed
void NBrowserWindow::todoButtonPressed() {
    QString script_start="document.execCommand('insertHtml', false, '";
    QString script_end = "');";
    QString todo =
            "<input TYPE=\"CHECKBOX\" " +
            QString("onMouseOver=\"style.cursor=\\'hand\\'\" ") +
            QString("onClick=\"if(!checked) removeAttribute(\\'checked\\'); else setAttribute(\\'checked\\', \\'checked\\'); editorWindow.editAlert();\" />");

    QString selectedText = editor->selectedText().trimmed();
    QRegExp regex("\\r?\\n");
    QStringList items = selectedText.split(regex);
    if (items.size() == 0)
        items.append(" ");
    QString newLineChar = "<div><br><div>";
    for (int i=0; i<items.size(); i++) {
        if (i == items.size()-1)
            newLineChar = "";
           editor->page()->mainFrame()->evaluateJavaScript(
                script_start +todo +items[i] +newLineChar + script_end);
    }
    editor->setFocus();
    microFocusChanged();
}



// The font size button was pressed
void NBrowserWindow::fontSizeSelected(int index) {
    int size = buttonBar->fontSizes->itemData(index).toInt();

    if (size <= 0)
        return;

    QString text = editor->selectedHtml();
    if (text.trimmed() == "")
        return;

    // Go througth the selected HTML and strip out all of the existing font-sizes.
    // This allows for the font size to be changed multiple times.  Without this the inner most font
    // size would always win.
    for (int i=text.indexOf("<"); i>=0; i=text.indexOf("<",i+1)) {
        QString text1="";
        QString text2="";
        text1 = text.mid(0,i);
        QString interior = text.mid(i);
        if (!interior.startsWith("</")) {
            int endPos = text.indexOf(">",i);
            if (endPos>0) {
                interior = text.mid(i,endPos-i);
                text2 = text.mid(endPos);
            }
            // Now that we have a substring, look for the font-size
            if (interior.contains("font-size:")) {
                interior = interior.mid(0,interior.indexOf("font-size:"))+
                        //QString::number(size)+
                        interior.mid(interior.indexOf("pt;")+3);
                text = text1+interior+text2;
            }
        }
    }

    // Start building a new font span.
    int idx = buttonBar->fontNames->currentIndex();
    QString font = buttonBar->fontNames->itemText(idx);

    QString newText = "<span style=\"font-size: " +QString::number(size) +"pt; font-family:"+font+";\">"+text+"</span>";
    QString script = QString("document.execCommand('insertHtml', false, '"+newText+"');");
    editor->page()->mainFrame()->evaluateJavaScript(script);

    editor->setFocus();
    microFocusChanged();
}



void NBrowserWindow::insertHtml(QString html) {
    QString script = QString("document.execCommand('insertHtml', false, '%1');").arg(html);
    editor->page()->mainFrame()->evaluateJavaScript(script);
    microFocusChanged();
}


// The font name list was selected
void NBrowserWindow::fontNameSelected(int index) {
    QString font = buttonBar->fontNames->itemData(index).toString();
    buttonBar->fontSizes->blockSignals(true);
    buttonBar->loadFontSizeComboBox(font);
    buttonBar->fontSizes->blockSignals(false);
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('fontName', false, '"+font+"');");
    editor->setFocus();
    microFocusChanged();
}



// The font highlight color was pressed
void NBrowserWindow::fontHighlightClicked() {
    QColor *color = buttonBar->highlightColorMenuWidget->getColor();
    if (color->isValid()) {
        this->editor->page()->mainFrame()->evaluateJavaScript(
                "document.execCommand('backColor', false, '"+color->name()+"');");
        editor->setFocus();
        microFocusChanged();
    }
}



// The font color was pressed
void NBrowserWindow::fontColorClicked() {
    QColor *color = buttonBar->fontColorMenuWidget->getColor();
    if (color->isValid()) {
        this->editor->page()->mainFrame()->evaluateJavaScript(
                "document.execCommand('foreColor', false, '"+color->name()+"');");
        editor->setFocus();
        microFocusChanged();
    }
}


void NBrowserWindow::insertLinkButtonPressed() {
    QString text = editor->selectedText().trimmed();
    if (text == "" && currentHyperlink == "")
        return;

    InsertLinkDialog dialog(insertHyperlink);

    // If we have a link already highlighted, set it to the dialog.
    if (text.startsWith("http://", Qt::CaseInsensitive) ||
            text.startsWith("https://", Qt::CaseInsensitive) ||
            text.startsWith("ftp://", Qt::CaseInsensitive) ||
            text.startsWith("mailto:", Qt::CaseInsensitive)) {
        dialog.setUrl(text);
    }

    if (currentHyperlink != NULL && currentHyperlink != "") {
        dialog.setUrl(currentHyperlink);
    }
    dialog.exec();
    if (!dialog.okButtonPressed()) {
        return;
    }

    // Take care of inserting new links
    if (insertHyperlink) {
        QString selectedText = editor->selectedText().replace("'","\\'");
        if (dialog.getUrl().trimmed() == "")
            return;
        QString durl = dialog.getUrl().trimmed().replace("'","\\'");
        QString url = QString("<a href=\"%1\" title=\"%2\">%3</a>").arg(durl,durl,selectedText);
        QString script = QString("document.execCommand('insertHtml', false, '%1')").arg(url);
        editor->page()->mainFrame()->evaluateJavaScript(script);
        return;
    }

    QString x = dialog.getUrl();
    // Edit existing links
    QString js =  "function getCursorPos() {"
            "var cursorPos;"
            "if (window.getSelection) {"
            "   var selObj = window.getSelection();"
            "   var selRange = selObj.getRangeAt(0);"
            "   var workingNode = window.getSelection().anchorNode.parentNode;"
            "   while(workingNode != null) { "
            "      if (workingNode.nodeName.toLowerCase()=='a') workingNode.setAttribute('href','";
    js = js + dialog.getUrl() +QString("');")
            +QString("      workingNode = workingNode.parentNode;")
            +QString("   }")
            +QString("}")
            +QString("} getCursorPos();");
    editor->page()->mainFrame()->evaluateJavaScript(js);

    if (dialog.getUrl().trimmed() != "" ) {
        contentChanged();
        return;
    }

    // Remove URL
    js = QString( "function getCursorPos() {")
            + QString("var cursorPos;")
            + QString("if (window.getSelection) {")
            + QString("   var selObj = window.getSelection();")
            + QString("   var selRange = selObj.getRangeAt(0);")
            + QString("   var workingNode = window.getSelection().anchorNode.parentNode;")
            + QString("   while(workingNode != null) { ")
            + QString("      if (workingNode.nodeName.toLowerCase()=='a') { ")
            + QString("         workingNode.removeAttribute('href');")
            + QString("         workingNode.removeAttribute('title');")
            + QString("         var text = document.createTextNode(workingNode.innerText);")
            + QString("         workingNode.parentNode.insertBefore(text, workingNode);")
            + QString("         workingNode.parentNode.removeChild(workingNode);")
            + QString("      }")
            + QString("      workingNode = workingNode.parentNode;")
            + QString("   }")
            + QString("}")
            + QString("} getCursorPos();");
        editor->page()->mainFrame()->evaluateJavaScript(js);

        contentChanged();
}





void NBrowserWindow::removeLinkButtonPressed() {
    // Remove URL
    QString js = QString( "function getCursorPos() {")
            + QString("var cursorPos;")
            + QString("if (window.getSelection) {")
            + QString("   var selObj = window.getSelection();")
            + QString("   var selRange = selObj.getRangeAt(0);")
            + QString("   var workingNode = window.getSelection().anchorNode.parentNode;")
            + QString("   while(workingNode != null) { ")
            + QString("      if (workingNode.nodeName.toLowerCase()=='a') { ")
            + QString("         workingNode.removeAttribute('href');")
            + QString("         workingNode.removeAttribute('title');")
            + QString("         var text = document.createTextNode(workingNode.innerText);")
            + QString("         workingNode.parentNode.insertBefore(text, workingNode);")
            + QString("         workingNode.parentNode.removeChild(workingNode);")
            + QString("      }")
            + QString("      workingNode = workingNode.parentNode;")
            + QString("   }")
            + QString("}")
            + QString("} getCursorPos();");
        editor->page()->mainFrame()->evaluateJavaScript(js);
        contentChanged();
}



void NBrowserWindow::insertQuickLinkButtonPressed() {
    QString text = editor->selectedText();
    if (text.trimmed() == "")
        return;

    NoteTable ntable(global.db);
    QList<qint32> lids;
    if (!ntable.findNotesByTitle(lids, text))
        if (!ntable.findNotesByTitle(lids, text.trimmed()+"%"))
            if (!ntable.findNotesByNotebook(lids, "%"+text.trimmed()+"%"))
                return;
    Note n;

    // If we have a good return, then we can paste the link, otherwise we fall out
    // to a normal paste.
    if (ntable.get(n, lids[0],false, false)) {
        UserTable utable(global.db);
        User user;
        utable.getUser(user);

        QString href = "evernote:///view/" + QString::number(user.id) + QString("/") +
               user.shardId +QString("/") +
                n.guid +QString("/") +
                n.guid + QString("/");

        QString url = QString("<a href=\"") +href
                +QString("\" title=\"") +text
                +QString("\">") +text +QString("</a>");
        QString script = QString("document.execCommand('insertHtml', false, '")+url+QString("');");
        editor->page()->mainFrame()->evaluateJavaScript(script);
        return;
    }
}


void NBrowserWindow::insertLatexButtonPressed() {
    this->editLatex("");
}




void NBrowserWindow::insertTableButtonPressed() {
    TableDialog dialog(this);
    dialog.exec();
    if (!dialog.isOkPressed()) {
        return;
    }

    int cols = dialog.getCols();
    int rows = dialog.getRows();
    int width = dialog.getWidth();
    bool percent = dialog.isPercent();
    QString tableStyle = "style=\"-evernote-table:true;border-collapse:collapse;width:%1;table-layout:fixed;margin-left:0px;\"";
    QString cellStyle = "style=\"border-style:solid;border-width:1px;border-color:rgb(211,211,211);padding:10px;margin:0px;width:33.33%;\"";

    QString newHTML = QString("<table border=\"1px\" width=\"") +QString::number(width);
    QString widthString = QString::number(width);
    if (percent)
        widthString = widthString+"%";
    newHTML = "<table "+tableStyle.arg(widthString)+"<tbody>";

    for (int i=0; i<rows; i++) {
        newHTML = newHTML +"<tr>";
        for (int j=0; j<cols; j++) {
            newHTML = newHTML +"<td "+cellStyle+">&nbsp;</td>";
        }
        newHTML = newHTML +"</tr>";
    }
    newHTML = newHTML+"</tbody></table>";

    QString script = "document.execCommand('insertHtml', false, '"+newHTML+"');";
    editor->page()->mainFrame()->evaluateJavaScript(script);
    contentChanged();
}


void NBrowserWindow::insertTableRowButtonPressed() {
    QString js ="function insertTableRow() {"
        "   var selObj = window.getSelection();"
        "   var selRange = selObj.getRangeAt(0);"
        "   var workingNode = window.getSelection().anchorNode.parentNode;"
        "   var cellCount = 0;"
        "   while(workingNode != null) { "
        "      if (workingNode.nodeName.toLowerCase()=='tr') {"
        "           row = document.createElement('TR');"
        "           var nodes = workingNode.getElementsByTagName('td');"
        "           var style = '';"
        "           for (j=0; j<nodes.length; j=j+1) {"
        "             if (style == '' && nodes[0].hasAttribute('style')) style = nodes[0].attributes['style'].value;"
        "             window.browserWindow.printNodeName(style);"
        "             cell = document.createElement('TD');"
        "             if (style != '') cell.setAttribute('style',style);"
        "             cell.innerHTML='&nbsp;';"
        "             row.appendChild(cell);"
        "           }"
        "           workingNode.parentNode.insertBefore(row,workingNode.nextSibling);"
        "           return;"
        "      }"
        "      workingNode = workingNode.parentNode;"
        "   }"
        "} insertTableRow();";
    editor->page()->mainFrame()->evaluateJavaScript(js);
    contentChanged();
}


void NBrowserWindow::insertTableColumnButtonPressed() {
    if (!editor->insertTableColumnAction->isEnabled())
        return;
    QString js = "function insertTableColumn() {"
            "   var selObj = window.getSelection();"
            "   var selRange = selObj.getRangeAt(0);"
            "   var workingNode = window.getSelection().anchorNode.parentNode;"
            "   var current = 0;"
            "   var style = '';"
            "   while (workingNode.nodeName.toLowerCase() != 'table' && workingNode != null) {"
            "       if (workingNode.nodeName.toLowerCase() == 'td') {"
            "          var td = workingNode;"
            "          if (style == '' && td.hasAttribute('style')) style = td.attributes['style'].value;"
            "          while (td.previousSibling != null) { "
            "             current = current+1; td = td.previousSibling;"
            "          }"
            "       }"
            "       workingNode = workingNode.parentNode; "
            "   }"
            "   if (workingNode == null) return;"
            "   for (var i=0; i<workingNode.rows.length; i++) { "
            "      var cell = workingNode.rows[i].insertCell(current+1); "
            "      cell.setAttribute('style',style);"
//            "          window.browserWindow.printNodeName(cell.style);"
            "      cell.innerHTML = '&nbsp'; "
            "   }"
            "} insertTableColumn();";
        editor->page()->mainFrame()->evaluateJavaScript(js);
        contentChanged();
}




void NBrowserWindow::tablePropertiesButtonPressed() {
    if (!editor->tablePropertiesAction->isEnabled())
        return;
    tableCellStyle = "";
    tableStyle = "";

    // First go through the table & find the existing cell & table attributes
    QString js = "function tableProperties() {"
            "   var selObj = window.getSelection();"
            "   var selRange = selObj.getRangeAt(0);"
            "   var workingNode = window.getSelection().anchorNode.parentNode;"
            "   var current = 0;"
            "   var style = '';"
            "   while (workingNode.nodeName.toLowerCase() != 'table' && workingNode != null) {"
            "       if (workingNode.nodeName.toLowerCase() == 'td') {"
            "          var td = workingNode;"
            "          if (style == '' && td.hasAttribute('style')) style = td.attributes['style'].value;"
            "          while (td.previousSibling != null) { "
            "             current = current+1; td = td.previousSibling;"
            "          }"
            "       }"
            "       workingNode = workingNode.parentNode; "
            "   }"
            "   if (workingNode == null) return;"
            "   window.browserWindow.setTableCellStyle(style);"
            "   window.browserWindow.printNodeName(style);"
            "   if (workingNode.hasAttribute('style')) {"
            "       var td = workingNode;"
            "       style = td.attributes['style'].value;"
            "       window.browserWindow.setTableStyle(style);"
            "   }"
            "} tableProperties();";
        editor->page()->mainFrame()->evaluateJavaScript(js);
        QLOG_DEBUG() << this->tableStyle;
        QLOG_DEBUG() << this->tableCellStyle;

        TablePropertiesDialog dialog(tableStyle, tableCellStyle);
        dialog.exec();

        if (!dialog.okButtonPressed)
            return;

        QString newTableStyle = dialog.getTableCss();
        QString newCellStyle = dialog.getCellCss();

        // Go through the table & change the styles attributes.
        js = "function setTableProperties() {"
                "   var selObj = window.getSelection();"
                "   var selRange = selObj.getRangeAt(0);"
                "   var workingNode = window.getSelection().anchorNode.parentNode;"
                "   var style = '';"
                "   while (workingNode.nodeName.toLowerCase() != 'table' && workingNode != null) {"
                "       if (workingNode.nodeName.toLowerCase() == 'td') {"
                "          var td = workingNode;"
                "          while (td.previousSibling != null) { "
                "             td = td.previousSibling;"
                "          }"
                "       }"
                "       workingNode = workingNode.parentNode; "
                "   }"
                "   if (workingNode == null) return;"
                "   workingNode.attributes['style'].value = '%1';"
                "   window.browserWindow.setTableCellStyle(style);"
                "   var rowCount = workingNode.rows.length;"
                "   for (var i=0; i<rowCount; i++) {"
                "      var colCount = workingNode.rows[i].cells.length;"
                "      for (var j=0; j<colCount; j++) {"
                "         workingNode.rows[i].cells[j].attributes['style'].value = '%2';"
                "      }"
                "   }"
                "} setTableProperties();";
        js = js.arg(newTableStyle).arg(newCellStyle);
        editor->page()->mainFrame()->evaluateJavaScript(js);
        this->editor->isDirty = true;
        microFocusChanged();
}



void NBrowserWindow::deleteTableRowButtonPressed() {
    if (!editor->deleteTableRowAction->isEnabled())
        return;

    QString js = "function deleteTableRow() {"
        "   var selObj = window.getSelection();"
        "   var selRange = selObj.getRangeAt(0);"
        "   var workingNode = window.getSelection().anchorNode.parentNode;"
        "   var cellCount = 0;"
        "   while(workingNode != null) { "
        "      if (workingNode.nodeName.toLowerCase()=='tr') {"
        "           workingNode.parentNode.removeChild(workingNode);"
        "           return;"
        "      }"
        "      workingNode = workingNode.parentNode;"
        "   }"
        "} deleteTableRow();";
    editor->page()->mainFrame()->evaluateJavaScript(js);
    contentChanged();
}



void NBrowserWindow::deleteTableColumnButtonPressed() {
    if (!editor->deleteTableColumnAction->isEnabled())
        return;

    QString js = "function deleteTableColumn() {"
            "   var selObj = window.getSelection();"
            "   var selRange = selObj.getRangeAt(0);"
            "   var workingNode = window.getSelection().anchorNode.parentNode;"
            "   var current = 0;"
            "   while (workingNode.nodeName.toLowerCase() != 'table' && workingNode != null) {"
            "       if (workingNode.nodeName.toLowerCase() == 'td') {"
            "          var td = workingNode;"
            "          while (td.previousSibling != null) { "
            "             current = current+1; td = td.previousSibling;"
            "          }"
            "       }"
            "       workingNode = workingNode.parentNode; "
            "   }"
            "   if (workingNode == null) return;"
            "   for (var i=0; i<workingNode.rows.length; i++) { "
            "      workingNode.rows[i].deleteCell(current); "
            "   }"
            "} deleteTableColumn();";
        editor->page()->mainFrame()->evaluateJavaScript(js);
        contentChanged();
}



void NBrowserWindow::rotateImageLeftButtonPressed() {
    rotateImage(-90.0);
}




void NBrowserWindow::rotateImageRightButtonPressed() {
    rotateImage(90.0);
}


void NBrowserWindow::rotateImage(qreal degrees) {

    // rotate the image
    QWebSettings::setMaximumPagesInCache(0);
    QWebSettings::setObjectCacheCapacities(0, 0, 0);
    QImage image(global.fileManager.getDbaDirPath() +selectedFileName);
    QMatrix matrix;
    matrix.rotate( degrees );
    image = image.transformed(matrix);
    image.save(global.fileManager.getDbaDirPath() +selectedFileName);
    editor->setHtml(editor->page()->mainFrame()->toHtml());

    // Now, we need to update the note's MD5
    QFile f(global.fileManager.getDbaDirPath() +selectedFileName);
    f.open(QIODevice::ReadOnly);
    QByteArray filedata = f.readAll();
    QCryptographicHash hash(QCryptographicHash::Md5);
    QByteArray b = hash.hash(filedata, QCryptographicHash::Md5);
    updateImageHash(b);

    // Reload the web page
    editor->triggerPageAction(QWebPage::ReloadAndBypassCache);
    contentChanged();
}


void NBrowserWindow::updateImageHash(QByteArray newhash) {
    QString content = editor->page()->mainFrame()->toHtml();
    int pos = content.indexOf("<img ");
    for (; pos != -1; pos=content.indexOf("<img ", pos+1) ) {
        int endPos = content.indexOf(">", pos);
        QString section = content.mid(pos, endPos-pos);
        if (section.contains("lid=\"" +QString::number(selectedFileLid) + "\"")) {
            ResourceTable rtable(global.db);
            QString oldhash = section.mid(section.indexOf("hash=\"")+6);
            oldhash = oldhash.mid(0,oldhash.indexOf("\""));
            section.replace(oldhash, newhash.toHex());
            QString newcontent = content.mid(0,pos) +section +content.mid(endPos);
            QByteArray c;
            c.append(newcontent);
            editor->page()->mainFrame()->setContent(c);
            rtable.updateResourceHash(selectedFileLid, newhash);
            return;
        }
    }
}

void NBrowserWindow::imageContextMenu(QString l, QString f) {
    editor->downloadAttachmentAction()->setEnabled(true);
    editor->rotateImageRightAction->setEnabled(true);
    editor->rotateImageLeftAction->setEnabled(true);
    editor->openAction->setEnabled(true);
    editor->downloadImageAction()->setEnabled(true);
    selectedFileName = f;
    selectedFileLid = l.toInt();
}


void NBrowserWindow::attachFile() {
    QFileDialog fileDialog;
    if (attachFilePath != "")
        fileDialog.setDirectory(attachFilePath);
    else
        fileDialog.setDirectory(QDir::homePath());
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    connect(&fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(attachFileSelected(QString)));
    fileDialog.exec();
}



//****************************************************************
//* MicroFocus changed
//****************************************************************
 void NBrowserWindow::microFocusChanged() {
     buttonBar->boldButtonWidget->setDown(false);
     buttonBar->italicButtonWidget->setDown(false);
     buttonBar->underlineButtonWidget->setDown(false);
     editor->openAction->setEnabled(false);
     editor->downloadAttachmentAction()->setEnabled(false);
     editor->rotateImageLeftAction->setEnabled(false);
     editor->rotateImageRightAction->setEnabled(false);
     editor->insertTableAction->setEnabled(true);
     editor->insertTableColumnAction->setEnabled(false);
     editor->insertTableRowAction->setEnabled(false);
     editor->tablePropertiesAction->setEnabled(false);
     editor->deleteTableRowAction->setEnabled(false);
     editor->deleteTableColumnAction->setEnabled(false);
     editor->insertLinkAction->setText(tr("Insert Link"));
     editor->removeLinkAction->setEnabled(false);
     editor->insertQuickLinkAction->setEnabled(true);
     editor->rotateImageRightAction->setEnabled(false);
     editor->rotateImageLeftAction->setEnabled(false);

//     QLOG_DEBUG() << editor->page()->inputMethodQuery(Qt::ImCursorPosition).toInt();
//     QLOG_DEBUG() << editor->page()->inputMethodQuery(Qt::ImSurroundingText).toString();

     insertHyperlink = true;
     currentHyperlink ="";
     insideList = false;
     insideTable = false;
     insideEncryption = false;
     forceTextPaste = false;

     if (editor->selectedText().trimmed().length() > 0 && global.javaFound)
         editor->encryptAction->setEnabled(true);
     else
         editor->encryptAction->setEnabled(false);


         //     +QString("            window.browserWindow.printNodeName(workingNode.firstChild.nodeValue);")
    QString js = QString("function getCursorPos() {")
        +QString("var cursorPos;")
        +QString("var insideUrl=false;")
        +QString("if (window.getSelection) {")
        +QString("   var selObj = window.getSelection();")
        +QString("   var selRange = selObj.getRangeAt(0);")
        +QString("   var workingNode = window.getSelection().anchorNode.parentNode;")
        //+QString("    window.browserWindow.printNodeName(workingNode.nodeName);")
        +QString("   while(workingNode != null) { ")
        //+QString("      window.browserWindow.printNodeName(workingNode.nodeName);")
        +QString("      if (workingNode.nodeName=='TABLE') {")
        +QString("          if (workingNode.getAttribute('class').toLowerCase() == 'en-crypt-temp') window.browserWindow.insideEncryptionArea();")
        +QString("      }")
        +QString("      if (workingNode.nodeName=='B') window.browserWindow.boldActive();")
        +QString("      if (workingNode.nodeName=='I') window.browserWindow.italicsActive();")
        +QString("      if (workingNode.nodeName=='U') window.browserWindow.underlineActive();")
        +QString("      if (workingNode.nodeName=='UL') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='OL') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='LI') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='TBODY') window.browserWindow.setInsideTable();")
        +QString("      if (workingNode.nodeName=='A') {")
        +QString("           insideUrl = true;")
        +QString("           for(var x = 0; x < workingNode.attributes.length; x++ ) {")
        +QString("              if (workingNode.attributes[x].nodeName.toLowerCase() == 'href')")
        +QString("                  window.browserWindow.setInsideLink(workingNode.attributes[x].nodeValue);")
        +QString("           }")
        +QString("      }")
        +QString("      if (workingNode.nodeName=='SPAN') {")
        +QString("         if (workingNode.getAttribute('style') == 'text-decoration: underline;') window.browserWindow.underlineActive();")
        +QString("      }")
        +QString("      workingNode = workingNode.parentNode;")
        +QString("   }")
        +QString("}")
        +QString("}  getCursorPos();");
    editor->page()->mainFrame()->evaluateJavaScript(js);


    QString js2 = QString("function getFontSize() {") +
                  QString("    var node = document.getSelection().anchorNode;") +
                  QString("    var anchor = (node.nodeType == 3 ? node.parentNode : node);") +
                  QString("      var size = window.getComputedStyle(anchor,null)[\"fontSize\"];") +
                  QString("      var font = window.getComputedStyle(anchor,null)[\"fontFamily\"];") +
                  QString("      window.browserWindow.changeDisplayFontSize(size);") +
                  QString("      window.browserWindow.changeDisplayFontName(font);") +
                  QString("} getFontSize();");
    editor->page()->mainFrame()->evaluateJavaScript(js2);
 }

 void NBrowserWindow::printNodeName(QString v) {
     QLOG_DEBUG() << v;
 }


 // Tab button pressed
 void NBrowserWindow::tabPressed() {
     if (insideEncryption)
         return;
     if (!insideList && !insideTable) {
         QString script_start =  "document.execCommand('insertHtml', false, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;');";
         editor->page()->mainFrame()->evaluateJavaScript(script_start);
         return;
     }
     if (insideList) {
         shiftRightButtonPressed();
     }
     if (insideTable) {
         QString js =  "function getCursorPosition() { "
                 "   var selObj = window.getSelection();"
                 "   var selRange = selObj.getRangeAt(0);"
                 "   var workingNode = window.getSelection().anchorNode;"
                 "   var rowCount = 0;"
                 "   var colCount = 0;"
                 "   while(workingNode != null && workingNode.nodeName.toLowerCase() != 'table') { "
                 "      if (workingNode.nodeName.toLowerCase()=='tr') {"
                 "         rowCount = rowCount+1;"
                 "      }"
                 "      if (workingNode.nodeName.toLowerCase() == 'td') {"
                 "         colCount = colCount+1;"
                 "      }"
                 "      if (workingNode.previousSibling != null)"
                 "          workingNode = workingNode.previousSibling;"
                 "      else "
                 "           workingNode = workingNode.parentNode;"
                 "   }"
                 "   var nodes = workingNode.getElementsByTagName('tr');"
                 "   var tableRows = nodes.length;"
                 "   nodes = nodes[0].getElementsByTagName('td');"
                 "   var tableColumns = nodes.length;"
                 "   window.browserWindow.setTableCursorPositionTab(rowCount, colCount, tableRows, tableColumns);"
                 "} getCursorPosition();";
         editor->page()->mainFrame()->evaluateJavaScript(js);
     }

 }


 // Backtab pressed.
 void NBrowserWindow::backtabPressed() {
     if (insideEncryption)
         return;
     if (insideList)
         shiftLeftButtonPressed();
     if (insideTable) {
         QString js = "function getCursorPosition() { "
                 "   var selObj = window.getSelection();"
                 "   var selRange = selObj.getRangeAt(0);"
                 "   var workingNode = window.getSelection().anchorNode;"
                 "   var rowCount = 0;"
                 "   var colCount = 0;"
                 "   while(workingNode != null && workingNode.nodeName.toLowerCase() != 'table') { "
                 "      if (workingNode.nodeName.toLowerCase()=='tr') {"
                 "         rowCount = rowCount+1;"
                 "      }"
                 "      if (workingNode.nodeName.toLowerCase() == 'td') {"
                 "         colCount = colCount+1;"
                 "      }"
                 "      if (workingNode.previousSibling != null)"
                 "          workingNode = workingNode.previousSibling;"
                 "      else "
                 "           workingNode = workingNode.parentNode;"
                 "   }"
                 "   var nodes = workingNode.getElementsByTagName('tr');"
                 "   var tableRows = nodes.length;"
                 "   nodes = nodes[0].getElementsByTagName('td');"
                 "   var tableColumns = nodes.length;"
                 "   window.browserWindow.setTableCursorPositionBackTab(rowCount, colCount, tableRows, tableColumns);"
                 "} getCursorPosition();";
         editor->page()->mainFrame()->evaluateJavaScript(js);
     }
 }



 // If a user presses backtab from within a table
void NBrowserWindow::setTableCursorPositionBackTab(int currentRow, int currentCol, int tableRows, int tableColumns) {
    // suppress unused warninsg
    Q_UNUSED(tableRows);
    Q_UNUSED(tableColumns);

    // Determine what key to emulate.
     if (currentRow  == 1 && currentCol == 1) {
         return;
     }
     QKeyEvent *up = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
     QCoreApplication::postEvent(editor->editorPage, up);
}



// If a user presses backtab from within a table
void NBrowserWindow::setTableCursorPositionTab(int currentRow, int currentCol, int tableRows, int tableColumns) {
    if (currentRow  == tableRows && currentCol == tableColumns) {
        this->insertTableRowButtonPressed();
    }
    QKeyEvent *down = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QCoreApplication::postEvent(editor->editorPage, down);
}



// Set the background color of a note
 void NBrowserWindow::setBackgroundColor(QString value) {
     QString js = QString("function changeBackground(color) {")
         +QString("document.body.style.background = color;")
         +QString("}")
         +QString("changeBackground('" +value+"');");
     editor->page()->mainFrame()->evaluateJavaScript(js);
     NoteTable noteTable(global.db);
     noteTable.setDirty(this->lid, true);
     this->editor->isDirty = true;
     editor->setFocus();
     microFocusChanged();
 }


 // The user clicked a link in the note
 void NBrowserWindow::linkClicked(const QUrl url) {
     if (url.toString().startsWith("latex:///", Qt::CaseInsensitive)) {
         editLatex(url.toString().mid(9));
         return;
     }
     if (url.toString().startsWith("evernote:/view/", Qt::CaseInsensitive) ||
             url.toString().startsWith("evernote:///view/", Qt::CaseInsensitive)) {

         QStringList tokens;
         if (url.toString().startsWith("evernote:/view/", Qt::CaseInsensitive))
            tokens = url.toString().replace("evernote:/view/", "").split("/", QString::SkipEmptyParts);
         else
            tokens = url.toString().replace("evernote:///view/", "").split("/", QString::SkipEmptyParts);
         QString oguid =tokens[2];
         QString eguid = tokens[3];
         NoteTable ntable(global.db);
         qint32 newlid = ntable.getLid(eguid);
         if (newlid <= 0)
             newlid = ntable.getLid(oguid);
         if (newlid <= 0)
             return;

         bool newExternalWindow = false;
         bool newTab = false;
         if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
            if (global.getMiddleClickAction() == MOUSE_MIDDLE_CLICK_NEW_WINDOW)
                newExternalWindow = true;
            else
                newTab = true;
         } else {
             // Setup a new filter
             FilterCriteria *criteria = new FilterCriteria();
             global.filterCriteria[global.filterPosition]->duplicate(*criteria);
             criteria->unsetSelectedNotes();
             criteria->unsetLid();
             criteria->setLid(newlid);
             global.appendFilter(criteria);
             global.filterPosition++;
         }
         emit(evernoteLinkClicked(newlid, newTab, newExternalWindow));

         return;
     }
     if (url.toString().startsWith("nnres:", Qt::CaseInsensitive)) {
         if (url.toString().endsWith("/vnd.evernote.ink")) {
             QMessageBox::information(this, tr("Unable Open"), QString(tr("This is an ink note.\nInk notes are not supported since Evernote has not\n published any specifications on them\nand I'm too lazy to figure them out by myself.")));
             return;
         }
         QString filepath = global.fileManager.getDbaDirPath();
// Windows check
#ifdef _WIN32
         filepath = filepath.replace("\\", "/");
#endif // End windows check
         QString fullName = url.toString().mid(6).replace(filepath,"");
         filepath = filepath.replace("\\", "/");
         QLOG_DEBUG() << global.fileManager.getDbaDirPath();
         int index = fullName.lastIndexOf(".");
         QString guid = "";
         if (index != -1) {
             guid = fullName.mid(0,index);
         } else
             guid = fullName;
         QDirIterator dirIt(global.fileManager.getDbaDirPath());
         QString fileUrl = "";
         while (dirIt.hasNext()) {
             if (QFileInfo(dirIt.filePath()).isFile() && QFileInfo(dirIt.filePath()).baseName() == guid) {
                 fileUrl = dirIt.fileName();
             }
             dirIt.next();
         }
         if (fileUrl == "")
             return;
         fileUrl = global.fileManager.getDbaDirPath()+fileUrl;

// Windows check
#ifdef _WIN32
         fileUrl = fileUrl.replace("\\", "/");
#endif // End windows check
         global.resourceWatcher->addPath(fileUrl);
         QDesktopServices::openUrl(fileUrl);
         return;
     }
     QDesktopServices::openUrl(url);
 }



 // show/hide view source window
void NBrowserWindow::showSource(bool value) {
     setSource();
     sourceEdit->setVisible(value);
     sourceEditorTimer->setInterval(1000);
     if (!value)
         sourceEditorTimer->stop();
     else
         sourceEditorTimer->start();
 }



// Toggle the show source button
void NBrowserWindow::toggleSource() {
    if (sourceEdit->isVisible())
        showSource(false);
    else
        showSource(true);
}



// Clear out the window's contents
void NBrowserWindow::clear() {

    sourceEdit->blockSignals(true);
    editor->blockSignals(true);
    sourceEdit->setPlainText("");
    editor->setContent("<html><body></body></html>");
    sourceEdit->setReadOnly(true);
    editor->page()->setContentEditable(false);
    lid = -1;
    editor->blockSignals(false);
    sourceEdit->blockSignals(false);

    noteTitle.blockSignals(true);
    noteTitle.setTitle(-1, "", "");
    noteTitle.blockSignals(false);

    tagEditor.blockSignals(true);
    tagEditor.clear();
    tagEditor.blockSignals(false);

//    authorEditor.blockSignals(true);
//    authorEditor.setText("");
//    authorEditor.blockSignals(false);

    urlEditor.blockSignals(true);
    urlEditor.setUrl(-1, "");
    urlEditor.blockSignals(false);

//    dateEditor.setEnabled(false);
//    editor->page()->setContentEditable(false);

    dateEditor.clear();
}



// Set the source for the "show source" button
void NBrowserWindow::setSource() {
    if (sourceEdit->hasFocus())
        return;

    QString text = editor->editorPage->mainFrame()->toHtml();
    sourceEdit->blockSignals(true);
    int body = text.indexOf("<body", Qt::CaseInsensitive);
    if (body != -1) {
        body = text.indexOf(">",body);
        if (body != -1) {
            sourceEditHeader =text.mid(0, body+1);
            text = text.mid(body+1);
        }
    }
    text = text.replace("</body></html>", "");
    sourceEdit->setPlainText(text);
 //   sourceEdit->setReadOnly(true);
    sourceEdit->setReadOnly(!editor->page()->isContentEditable());
    sourceEdit->blockSignals(false);
}



// Expose the programs to the javascript process
void NBrowserWindow::exposeToJavascript() {
    editor->page()->mainFrame()->addToJavaScriptWindowObject("browserWindow", this);
}



// If we are within bold text, set the bold button active
void NBrowserWindow::boldActive() {
    buttonBar->boldButtonWidget->setDown(true);
}



// If we are within italics text, make the text button active
void NBrowserWindow::italicsActive() {
   buttonBar->italicButtonWidget->setDown(true);
}



// If we are within encrypted text, make sure we force a paste text
void NBrowserWindow::insideEncryptionArea() {
    insideEncryption = true;
    forceTextPaste = true;
}



// If we are within underlined text, make the button active
void NBrowserWindow::underlineActive() {
    buttonBar->underlineButtonWidget->setDown(true);
}



// Set true if we are within some type of list
void NBrowserWindow::setInsideList() {
    insideList = true;
}



// If we are within a table, set the menu options active
void NBrowserWindow::setInsideTable() {
    editor->insertTableAction->setEnabled(false);
    editor->insertTableRowAction->setEnabled(true);
    editor->tablePropertiesAction->setEnabled(true);
    editor->insertTableColumnAction->setEnabled(true);
    editor->deleteTableRowAction->setEnabled(true);
    editor->deleteTableColumnAction->setEnabled(true);
    editor->encryptAction->setEnabled(false);
    insideTable = true;
}


// Set if we are within a link
void NBrowserWindow::setInsideLink(QString link) {
    currentHyperlink = link;
    editor->insertLinkAction->setText(tr("Edit Link"));
    editor->removeLinkAction->setEnabled(true);
    currentHyperlink = link;
    insertHyperlink = false;
}




// Edit a LaTeX formula
void NBrowserWindow::editLatex(QString guid) {
    QString text = editor->selectedText();
    QString oldFormula = "";
    if (text.trimmed() == "\n" || text.trimmed() == "") {
        InsertLatexDialog dialog;
        if (guid.trimmed() != "") {
            Resource r;
            ResourceTable resTable(global.db);
            resTable.get(r, guid.toInt(), false);
            if (r.attributes.isSet()) {
                ResourceAttributes attributes;
                attributes = r.attributes;
                if (attributes.sourceURL.isSet()) {
                    QString formula = attributes.sourceURL;
                    formula = formula.replace("http://latex.codecogs.com/gif.latex?", "");
                    oldFormula = formula;
                    dialog.setFormula(formula);
                }
            }
        }
        dialog.exec();
        if (!dialog.okPressed()) {
            return;
        }
        text = dialog.getFormula().trimmed();
    }

    ConfigStore cs(global.db);
    qint32 newlid = cs.incrementLidCounter();
    Resource r;
    NoteTable ntable(global.db);
    ResourceTable rtable(global.db);
    QString outfile = global.fileManager.getDbaDirPath() + QString::number(newlid) +QString(".gif");

    // Run it through "mimetex" to create the gif
    QProcess latexProcess;
    QStringList args;
    args.append("-e");
    args.append(outfile);
    args.append(text);
    QLOG_DEBUG() << "Formula:" << "mimetex -e "+outfile +" '" +text +"'";
    //latexProcess.start(formula, QIODevice::ReadWrite|QIODevice::Unbuffered);
    latexProcess.start("mimetex", args, QIODevice::ReadWrite|QIODevice::Unbuffered);

    latexProcess.waitForStarted();
    latexProcess.waitForFinished();
    QLOG_DEBUG() << " LaTeX Return Code: " << latexProcess.state();
    QLOG_DEBUG() << "mimetex Errors:" << latexProcess.readAllStandardError();
    QLOG_DEBUG() << "mimetex Output:" << latexProcess.readAllStandardOutput();

    // Now, check if the file exists.  If it does, we continue to create the resource
    QFile f(outfile);
    if (!f.exists()) {
        QMessageBox msgBox;
        msgBox.setText(tr("Unable to create LaTeX image"));
        msgBox.setInformativeText(tr("Unable to create LaTeX image.  Are you sure mimetex is installed?"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    f.close();
    f.open(QIODevice::ReadOnly);
    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(data, QCryptographicHash::Md5);

    Data d;
    if (r.data.isSet())
        d = r.data;
    d.body = f.read(data.size());
    r.data = d;
    f.close();
    d.bodyHash = hash;
    d.size = data.size();
    r.data = d;

    r.guid = QString::number(newlid);
    r.noteGuid = ntable.getGuid(lid);

    r.mime = "image/gif";
    r.active = true;
    r.updateSequenceNum = 0;
    r.width = 0;
    r.height = 0;
    r.duration = 0;

    ResourceAttributes a;
    a.attachment = false;
    a.sourceURL = "http://latex.codecogs.com/gif.latex?" +text;
    r.attributes = a;

    rtable.add(newlid, r, true, lid);

    // do the actual insert into the note

    QString buffer;
    buffer.append("<a onmouseover=\"cursor:&apos;hand&apos;\" title=\"");
    buffer.append(text.remove(QRegExp("[^a-zA-Z +-*/^{}()]")));
    buffer.append("\" href=\"latex:///");
    buffer.append(QString::number(newlid));
    buffer.append("\">");
    buffer.append("<img src=\"file://");
#ifdef _WIN32
    buffer.append("/");
#endif
    buffer.append(outfile);
    buffer.append("\" type=\"image/gif\" hash=\"");
    buffer.append(hash.toHex());
    buffer.append("\" onContextMenu=\"window.browser.imageContextMenu(&apos;");
    buffer.append(QString::number(newlid));
    buffer.append("&apos;, &apos;");
    buffer.append(outfile);
    buffer.append("&apos;);\" ");
    buffer.append(" en-tag=\"en-latex\" lid=\"");
    buffer.append(QString::number(newlid));
    buffer.append("\"></a>");

    // If this is a new formula, we insert it, otherwise we replace the old one.
    if (oldFormula == "") {
        QString script_start = "document.execCommand('insertHTML', false, '";
        QString script_end = "');";

        editor->page()->mainFrame()->evaluateJavaScript(
                script_start + buffer + script_end);
    } else {
        QString oldHtml = editor->page()->mainFrame()->toHtml();
        int startPos = oldHtml.indexOf("<a");
        while (startPos != -1) {
	    int endPos = oldHtml.indexOf("</a>", startPos);
            if (endPos != -1) {
                QString slice = oldHtml.mid(startPos, endPos-startPos+4);
                if (slice.contains("lid=\""+guid+"\"") && slice.contains("en-latex")) {
                    oldHtml.replace(slice, buffer);
                }
                startPos = oldHtml.indexOf("<a", endPos);
                editor->page()->mainFrame()->setHtml(oldHtml);
            }
        }
	editor->reload();
	contentChanged();
    }
}


// Set the focus to the note title
void NBrowserWindow::focusTitle() {
    this->noteTitle.setFocus();
}


// Set the focus to the note
void NBrowserWindow::focusNote() {
    this->editor->setFocus();
}


// Insert the date/time into a note
void NBrowserWindow::insertDatetime() {
    QDateTime dt = QDateTime::currentDateTime();
    QLocale locale;
    QString dts = dt.toString(locale.dateTimeFormat(QLocale::ShortFormat));

    editor->page()->mainFrame()->evaluateJavaScript(
        "document.execCommand('insertHtml', false, '"+dts+"');");
    editor->setFocus();
}



// Insert an image into the editor
void NBrowserWindow::insertImage(const QMimeData *mime) {

    // Get the image from the clipboard and save it into a QByteArray
    // that can be saved
    QImage img = qvariant_cast<QImage>(mime->imageData());
//    QClipboard *clipboard = global.clipboard;
//    QImage img = clipboard->pixmap().toImage();
    QByteArray imageBa;
    QBuffer b(&imageBa);
    b.open(QIODevice::WriteOnly);
    img.save(&b, "PNG");

    QString script_start = "document.execCommand('insertHTML', false, '";
    QString script_end = "');";

    Resource newRes;
    qint32 rlid = createResource(newRes, 0, imageBa, "image/png", false, "");
    if (rlid <= 0)
        return;

    // The resource is done, now we need to add it to the
    // note body
    QString g =  QString::number(rlid)+QString(".png");
    QString path = global.fileManager.getDbaDirPath() + g;

    // do the actual insert into the note
    QString buffer;
    Data d;
    if (newRes.data.isSet())
        d =newRes.data;
    QByteArray hash;
    if (d.bodyHash.isSet())
         hash = d.bodyHash;
    buffer.append("<img src=\"file://");
#ifdef _WIN32
    buffer.append("/");
#endif
    buffer.append(path);
    buffer.append("\" type=\"image/png\" hash=\"");
    buffer.append(hash.toHex());
    buffer.append("\" onContextMenu=\"window.browser.imageContextMenu(&apos;");
    buffer.append(QString::number(rlid));
    buffer.append("&apos;, &apos;");
    buffer.append(g);
    buffer.append("&apos;);\" ");
    buffer.append(" en-tag=\"en-media\" style=\"cursor: default;\" lid=\"");
    buffer.append(QString::number(rlid));
    buffer.append("\">");

    // Insert the actual note
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + buffer + script_end);

    return;
}


// Create  a new resource and add it to the database
qint32 NBrowserWindow::createResource(Resource &r, int sequence, QByteArray data,  QString mime, bool attachment, QString filename) {
    ConfigStore cs(global.db);
    qint32 rlid = cs.incrementLidCounter();

    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);

    QString guid =  QString::number(rlid);
    NoteTable noteTable(global.db);
    r.guid = guid;
    r.noteGuid = noteTable.getGuid(lid);
    QString noteguid = r.noteGuid;
    if (noteguid == "")
        return 0;
    r.mime = mime;
    r.active = true;
    r.updateSequenceNum = sequence;
    r.width = 0;
    r.height = 0;
    r.duration = 0;
    ResourceAttributes a;
    if (r.attributes.isSet())
        a = r.attributes;
    a.attachment = attachment;
    if (filename != "") {
        a.fileName = filename;
    }

    Data d;
    d.body = data;
    d.bodyHash = hash;
    d.size = data.size();

    r.data = d;
    r.attributes = a;
    ResourceTable resourceTable(global.db);
    resourceTable.add(rlid, r, true, lid);

    return rlid;
}




// Prepare the email for sending.  This function scans through
// the email for images & attachments.  The resulting
// MimeMessage has all of the email contents.
void NBrowserWindow::prepareEmailMessage(MimeMessage *message, QString note) {
    MimeHtml *text = new MimeHtml();

    // Prepare the massage the same as if we were printing it.
    QString contents = this->stripContentsForPrint();
    QString textContents = editor->page()->currentFrame()->toPlainText();
    QStringList images;
    QStringList attachments;

    // Now, go thgough & reformat all the img tags.
    int cidCount=0;
    int pos = contents.indexOf("src=\"file:");
    while (pos>=0) {
        QString localFile = contents.mid(pos+13);
        int endPos = localFile.indexOf("\"");
        localFile = localFile.mid(0,endPos);
        images.append(localFile);
        endPos = pos+endPos;
        QString part1 = contents.mid(0,pos);
        QString part2 = contents.mid(endPos+14);
        cidCount++;
        contents = part1 + "src='cid:file" +QString::number(cidCount) +"'" + part2;

        pos = contents.indexOf("src=\"file:", pos+5);
    }

    // next, look for all the attachments
    pos = contents.indexOf("href=\"nnres:");
    while (pos != -1) {
        QString localFile = contents.mid(pos+12);
        int endPos = localFile.indexOf("\"");
        localFile = localFile.mid(0,endPos);
        attachments.append(localFile);
        cidCount++;
        pos = contents.indexOf("href=\"nnres:", pos+5);
    }

    // If the user adds a note, then prepend it to the beginning.
    if (note.trimmed() != "") {
        int pos = contents.indexOf("<body");
        int endPos = contents.indexOf(">", pos);
        contents.insert(endPos+1,  Qt::escape(note)+"<p><p><hr><p>");
    }
    text->setHtml(contents);
    message->addPart(text);


    // Add all the images
    for (int i=0; i<images.size(); i++) {
        MimeReference mimeRef;
        QString localFile = images[i];
        QString mime = mimeRef.getMimeFromFileName(localFile);
        MimeInlineFile *file = new MimeInlineFile(new QFile(localFile));
        QString lidFile = localFile.mid(localFile.lastIndexOf(QDir::separator())+1);
        qint32 lid = lidFile.mid(0,lidFile.lastIndexOf(".")).toInt();
        ResourceTable rtable(global.db);
        Resource r;
        ResourceAttributes ra;
        if (rtable.get(r, lid, false) && r.attributes.isSet()) {
            ra = r.attributes;
            if (ra.fileName.isSet())
                file->setContentName(ra.fileName);
        }
        file->setContentId("file"+QString::number(i+1));
        file->setContentType(mime);
        message->addPart(file);
    }

    // Add all the attachments
    for (int i=0; i<attachments.size(); i++) {
        MimeReference mimeRef;
        QString localFile = attachments[i];
        QString mime = mimeRef.getMimeFromFileName(localFile);
        MimeInlineFile *file = new MimeInlineFile(new QFile(localFile));
        QString lidFile = localFile.mid(localFile.lastIndexOf(QDir::separator())+1);
        qint32 lid = lidFile.mid(0,lidFile.lastIndexOf(".")).toInt();
        ResourceTable rtable(global.db);
        Resource r;
        ResourceAttributes ra;
        if (rtable.get(r, lid, false) && r.attributes.isSet()) {
            ra = r.attributes;
            if (ra.fileName.isSet())
                file->setContentName(ra.fileName);
        }
        file->setContentType(mime);
        message->addPart(file);
    }
    return;

}





// Email current note.
void NBrowserWindow::emailNote() {
    global.settings->beginGroup("Email");
    QString server = global.settings->value("smtpServer", "").toString();
    int port = global.settings->value("smtpPort", 25).toInt();
    QString smtpConnectionType = global.settings->value("smtpConnectionType", "TcpConnection").toString();
    QString userid = global.settings->value("userid", "").toString();
    QString password = global.settings->value("password", "").toString();
    QString senderEmail = global.settings->value("senderEmail", "").toString();
    QString senderName = global.settings->value("senderName", "").toString();
    global.settings->endGroup();

    if (senderEmail.trimmed() == "" || server.trimmed() == "") {
        QMessageBox::critical(this, tr("Setup Error"),
             tr("SMTP Server has not been setup.\n\nPlease specify server settings\nin the Preferences menu."), QMessageBox::Ok);
        return;
    }

    EmailDialog emailDialog;
    emailDialog.subject->setText(noteTitle.text());
    emailDialog.exec();
    if (emailDialog.cancelPressed)
        return;
    emit(setMessage(tr("Sending Email. Please be patient.")));

    QStringList toAddresses = emailDialog.getToAddresses();
    QStringList ccAddresses = emailDialog.getCcAddresses();
    QStringList bccAddresses = emailDialog.getBccAddresses();

    if (senderName.trimmed() == "")
        senderName = senderEmail;

    SmtpClient::ConnectionType type = SmtpClient::TcpConnection;
    if (smtpConnectionType == "SslConnection")
        type = SmtpClient::SslConnection;
    if (smtpConnectionType == "TlsConnection")
        type = SmtpClient::TlsConnection;

    SmtpClient smtp(server, port, type);
    smtp.setResponseTimeout(-1);

    // We need to set the username (your email address) and password
    // for smtp authentication.
    smtp.setUser(userid);
    smtp.setPassword(password);

    // Now we create a MimeMessage object. This is the email.
    MimeMessage message;

    EmailAddress sender(senderEmail, senderName);
    message.setSender(&sender);

    for (int i=0; i<toAddresses.size(); i++) {
        EmailAddress *to = new EmailAddress(toAddresses[i], toAddresses[i]);
        message.addRecipient(to);
    }

    for (int i=0; i<ccAddresses.size(); i++) {
        EmailAddress *cc = new EmailAddress(ccAddresses[i], ccAddresses[i]);
        message.addRecipient(cc);
    }


    if (emailDialog.ccSelf->isChecked()) {
        EmailAddress *cc = new EmailAddress(senderEmail, senderName);
        message.addRecipient(cc);
    }

    for (int i=0; i<bccAddresses.size(); i++) {
        EmailAddress *bcc = new EmailAddress(bccAddresses[i], bccAddresses[i]);
        message.addRecipient(bcc);
    }

    // Set the subject
    message.setSubject(emailDialog.subject->text().trimmed());

    // Build the note content
    QString text =  emailDialog.note->toPlainText();
    prepareEmailMessage(&message, text);

    // Send the actual message.
    if (!smtp.connectToHost()) {
        QLOG_ERROR()<< "Failed to connect to host!";
        QMessageBox::critical(this, tr("Connection Error"), tr("Unable to connect to host."), QMessageBox::Ok);
        return;
    }

    if (!smtp.login()) {
        QLOG_ERROR() << "Failed to login!";
        QMessageBox::critical(this, tr("Login Error"), tr("Unable to login."), QMessageBox::Ok);
        return;
    }

    if (!smtp.sendMail(message)) {
        QMessageBox::critical(this, tr("Send Error"), tr("Unable to send email."), QMessageBox::Ok);
        QLOG_ERROR() << "Failed to send mail!";
        return;
    }

    smtp.quit();
    emit(setMessage("Message Sent"));
//    QMessageBox::information(this, tr("Message Sent"), tr("Message sent."), QMessageBox::Ok);
}





// Strip the contents from the current webview in preparation for printing.
QString NBrowserWindow::stripContentsForPrint() {
    // Start removing object tags
    QString contents = this->editor->selectedHtml().trimmed();
    if (contents == "")
       contents = editor->editorPage->mainFrame()->toHtml();
    int pos = contents.indexOf("<object");
    while (pos != -1) {
        int endPos = contents.indexOf(">", pos);
        QString lidString = contents.mid(contents.indexOf("lid=", pos)+5);
        lidString = lidString.mid(0,lidString.indexOf("\" "));
#ifndef _WIN32
        contents = contents.mid(0,pos) + "<img src=\"file://" +
                global.fileManager.getTmpDirPath() + lidString +
                QString("-print.png\" width=\"10%\" height=\"10%\"></img>")+contents.mid(endPos+1);
#else
        contents = contents.mid(0,pos) + "<img src=\"file:///" +
                global.fileManager.getTmpDirPath() + lidString +
                QString("-print.png\" width=\"10%\" height=\"10%\"></img>")+contents.mid(endPos+1);
#endif
        pos = contents.indexOf("<object", endPos);
    }
    return contents.replace("src=\"file:////", "src=\"/");
}


// Do a print preview of this note.  This works
// in much the same way as printNote().  It removes all the
// <object> tags & replaces them with <img>.
void NBrowserWindow::printPreviewNote() {
    QString contents = stripContentsForPrint();

    // Load the print page.  When it is ready the printReady() slot will
    // do the actual print
    printPreviewPage->setHtml(contents.toUtf8());
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags(Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printPreviewReady(QPrinter*)));
    preview.exec();
}


// Slot for when the printPreview is ready.
void NBrowserWindow::printPreviewReady(QPrinter *printer) {
   printPreviewPage->print(printer);
}



// Print the contents of a note.  Basically it loops through the
// note and replaces the <object> tags with <img> tags.  The plugin
// object should be creating temporary images for the print.
void NBrowserWindow::printNote() {
    QString contents = stripContentsForPrint();

    // Load the print page.  When it is ready the printReady() slot will
    // do the actual print
    printPage->setDocumentTitle(editor->title());
    printPage->setHtml(contents.toUtf8());

    QPrinter *printer;

    global.settings->beginGroup("Printer");
    QPrinter::Orientation orientation = static_cast<QPrinter::Orientation>(global.settings->value("orientation").toUInt());
    QString name = global.settings->value("printerName", "").toString();
    QPrinter::OutputFormat format = static_cast<QPrinter::OutputFormat>(global.settings->value("outputFormat", 0).toUInt());
    QPrinter::PaperSize pageSize  = static_cast<QPrinter::PageSize>(global.settings->value("pageSize", 2).toUInt());
    QPrinter::ColorMode colorMode  = static_cast<QPrinter::ColorMode>(global.settings->value("colorMode", 1).toUInt());
    QString fileName = global.settings->value("outputFileName", "").toString();
    global.settings->endGroup();

    bool error = false;
    printer = new QPrinter();
    printer->setPageSize(pageSize);
    printer->setOutputFormat(format);
    printer->setOrientation(orientation);
    printer->setColorMode(colorMode);


    if (fastPrint) {
        if (format == QPrinter::PdfFormat) {
            if (fileName == "")
                error = true;
            else
                printer->setOutputFileName(fileName);
        } else {
            if (name == "")
                error = true;
            else
                printer->setPrinterName(name);
        }
        if (error) {
            fastPrint = false;

            // Re-initialize printer object so we don't have any bogus
            // values from settings.
            delete printer;
            printer = new QPrinter();
        }
    }

    if (!fastPrint) {
        if (format == QPrinter::PdfFormat && fileName.trimmed() != "")
            printer->setOutputFileName(fileName);
        if (name.trimmed() != "")
            printer->setPrinterName(name);

        QPrintDialog dialog(printer);
        if (dialog.exec() ==  QDialog::Accepted) {
            printer = dialog.printer();
            global.settings->beginGroup("Printer");
            global.settings->setValue("orientation", printer->orientation());
            global.settings->setValue("printerName", printer->printerName());
            global.settings->setValue("outputFormat", printer->outputFormat());
            global.settings->setValue("outputFileName", printer->outputFileName());
            global.settings->setValue("pageSize", printer->pageSize());
            global.settings->setValue("colorMode", printer->colorMode());
            global.settings->endGroup();
            printPage->print(printer);
        }
    } else {
            printPage->print(printer);
//        QTextDocument td;
//        td.setHtml(printPage->toHtml());
//        td.setPageSize(printer->pageRect().size());
//            QRect innerRect = printer->pageRect();
//            innerRect.setTop(innerRect.top() + 20);
//            innerRect.setBottom(innerRect.bottom() - 30);
//            QRect contentRect = QRect(QPoint(0,0), td.size().toSize());
//            QRect currentRect = QRect(QPoint(0,0), innerRect.size());
//            QPainter painter(printer);
//            int count = 0;
//            painter.save();
//            painter.translate(0, 30);
//            while (currentRect.intersects(contentRect) && count < td.pageCount()) {
//                td.drawContents(&painter, currentRect);
//                count++;
//                currentRect.translate(0, currentRect.height());
//                painter.restore();
//                painter.drawText(10, 10, editor->title());
//                painter.drawText(10, printer->pageRect().bottom() - 10, QString("Page %1 of %2").arg(count).arg(td.pageCount()));
//                painter.save();
//                painter.translate(0, -currentRect.height() * count + 30);
//                if (currentRect.intersects(contentRect) && count < td.pageCount())
//                    printer->newPage();
//            }
//            painter.restore();
//            painter.end();
    }

    this->fastPrint = false;
}



void NBrowserWindow::noteSourceUpdated() {
    QByteArray ba;
    QString source = sourceEdit->toPlainText();
   //source = Qt::escape(source);
    ba.append(sourceEditHeader);
    ba.append(source);
    ba.append("</body></html>");
    editor->setContent(ba);
    this->editor->isDirty = true;
    emit noteContentEditedSignal(uuid, lid, editor->editorPage->mainFrame()->documentElement().toOuterXml());
}

// Update a resource's hash if it was edited somewhere else
void NBrowserWindow::updateResourceHash(qint32 noteLid, QByteArray oldHash, QByteArray newHash) {
    if (noteLid != lid)
        return;

    QString content = editor->editorPage->mainFrame()->documentElement().toOuterXml();

    // Start going through & looking for the old hash
    int pos = content.indexOf("<body");
    int endPos;
    int hashPos = -1;
    QString hashString = "hash=\"" +oldHash.toHex() +"\"";
    while (pos != -1) {
        endPos = content.indexOf(">", pos);  // Find the matching end of the tag
        hashPos = content.indexOf(hashString, pos);
        if (hashPos < endPos && hashPos != -1) {  // If we found the hash, begin the update
            QString startString = content.mid(0, hashPos);
            QString endString = content.mid(hashPos+hashString.length());
            QString newContent = startString + "hash=\"" +newHash.toHex() +"\"" +endString;
            QByteArray byteArray;
            byteArray.append(newContent);
            editor->setContent(byteArray);
            noteUpdated(lid);
            return;
        } else {
            pos = content.indexOf("<", pos+1);
        }
    }


}



void NBrowserWindow::attachFileSelected(QString filename) {
    // Read in the file
    QFile file(filename);

    // Save prior path for future use
    QFileInfo fileInfo(file);
    attachFilePath = fileInfo.path();

    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    file.close();

    QString script_start = "document.execCommand('insertHTML', false, '";
    QString script_end = "');";

    MimeReference mimeRef;
    QString extension = filename;
    int endPos = filename.lastIndexOf(".");
    if (endPos != -1)
        extension = extension.mid(endPos);
    QString mime =  mimeRef.getMimeFromExtension(extension);
    Resource newRes;
    bool attachment = true;
    if (mime == "application/pdf" || mime.startsWith("image/"))
        attachment = false;
    qint32 rlid = createResource(newRes, 0, ba, mime, attachment, QFileInfo(filename).fileName());
    QByteArray hash;
    if (newRes.data.isSet()) {
        Data d = newRes.data;
        if (d.bodyHash.isSet())
            hash = d.bodyHash;
    }
    if (rlid <= 0)
        return;

    // If we have an image, then insert it.
    if (mime.startsWith("image", Qt::CaseInsensitive)) {

        // The resource is done, now we need to add it to the
        // note body
        QString g =  QString::number(rlid)+extension;
        QString path = global.fileManager.getDbaDirPath() + g;

        // do the actual insert into the note
        QString buffer;
        QByteArray hash = "";
        if (newRes.data.isSet()) {
            Data d= newRes.data;
            if (d.bodyHash.isSet())
            hash = d.bodyHash;
        }
        buffer.append("<img src=\"file://");
#ifdef _WIN32
        buffer.append("/");
#endif
        buffer.append(path);
        buffer.append("\" type=\"");
        buffer.append(mime);
        buffer.append("\" hash=\"");
        buffer.append(hash.toHex());
        buffer.append("\" onContextMenu=\"window.browser.imageContextMenu(&apos;");
        buffer.append(QString::number(rlid));
        buffer.append("&apos;, &apos;");
        buffer.append(g);
        buffer.append("&apos;);\" ");
        buffer.append(" en-tag=\"en-media\" style=\"cursor: default;\" lid=\"");
        buffer.append(QString::number(rlid));
        buffer.append("\">");

        // Insert the actual image
        editor->page()->mainFrame()->evaluateJavaScript(
                script_start + buffer + script_end);
        return;
    }

    if (mime == "application/pdf" && global.pdfPreview) {
        // The resource is done, now we need to add it to the
        // note body
        QString g =  QString::number(rlid)+extension;

        // do the actual insert into the note
        QString buffer;
        QByteArray hash;
        if (newRes.data.isSet()) {
            Data data = newRes.data;
            if (data.bodyHash.isSet())
                hash = data.bodyHash;
        }
        buffer.append("<object width=\"100%\" height=\"100%\" lid=\"" +QString::number(rlid) +"\" hash=\"");
        buffer.append(hash.toHex());
        buffer.append("\" type=\"application/pdf\" />");

        // Insert the actual image
        editor->page()->mainFrame()->evaluateJavaScript(
                script_start + buffer + script_end);
        return;

    }

    // If we have something other than an image or PDF
    // First get the icon for this type of file
    AttachmentIconBuilder builder;
    QString g =  global.fileManager.getDbaDirPath()+ QString::number(rlid)+extension;
    QString tmpFile = builder.buildIcon(rlid, filename);

    // do the actual insert into the note
    QString buffer;
    buffer.append("<a en-tag=\"en-media\" ");
    buffer.append("lid=\""+QString::number(rlid) +QString("\" "));
    buffer.append("type=\"" +mime +"\" ");
    buffer.append("hash=\"" +hash.toHex()+"\" ");
    buffer.append("href=\"nnres:" +g+"\" ");
    buffer.append("oncontextmenu=\"window.browserWindow.resourceContextMenu(&apos");
    buffer.append(g +QString("&apos);\" "));
    buffer.append(">");

    buffer.append("<img en-tag=\"temporary\" title=\""+QFileInfo(filename).fileName() +"\" ");
    buffer.append("src=\"file://");
#ifdef _WIN32
    buffer.append("/");
#endif
    buffer.append(tmpFile);
    buffer.append("\" />");
    buffer.append("</a>");
    buffer.replace("\'", "&quot;");

    // Insert the actual attachment
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + buffer + script_end);
}



// Alarm has been completed
void NBrowserWindow::alarmCompleted() {
    QFont f = alarmText.font();
    f.setStrikeOut(!f.strikeOut());
    alarmText.setFont(f);

    NoteTable noteTable(global.db);
    noteTable.setDirty(this->lid, true);
    noteTable.setReminderCompleted(this->lid, f.strikeOut());
    global.reminderManager->remove(this->lid);
    emit(noteUpdated(this->lid));
    emit noteAlarmEditedSignal(uuid, lid, f.strikeOut(), alarmText.text());
}



void NBrowserWindow::alarmSet() {
    ReminderSetDialog dialog;
    Note n;
    NoteTable ntable(global.db);
    ntable.get(n, lid, false, false);
    NoteAttributes attributes;
    if (n.attributes.isSet())
        attributes = n.attributes;
    if (attributes.reminderTime.isSet()) {
        QDateTime dt;
        dt.setMSecsSinceEpoch(attributes.reminderTime);
        dialog.time->setTime(dt.time());
        dialog.calendar->setSelectedDate(dt.date());
    } else {
        QTime t = QTime::currentTime();
        dialog.time->setTime(t.addSecs(60*60));
    }
    dialog.exec();
    if (!dialog.okPressed)
        return;

    QDateTime dt;
    dt.setTime(dialog.time->time());
    QTime t = dialog.time->time();
    t.setHMS(t.hour(), t.minute(), 0,0);
    dt.setTime(t);
    dt.setDate(dialog.calendar->selectedDate());

    ntable.updateDate(this->lid, dt.toMSecsSinceEpoch(), NOTE_ATTRIBUTE_REMINDER_TIME, true);
    //alarmText.setText(dt.date().toString(Qt::SystemLocaleShortDate));
    if (dt.date() == QDate::currentDate())
        alarmText.setText(tr("Today"));
    else if (dt.date() == QDate::currentDate().addDays(+1))
        alarmText.setText(tr("Tomorrow"));
    else if (dt.date() == QDate::currentDate().addDays(-1))
        alarmText.setText(tr("Yesterday"));
    else
        alarmText.setText(dt.date().toString(global.dateFormat));

    alarmText.setVisible(true);
    QFont f = alarmText.font();
    f.setStrikeOut(false);
    alarmText.setFont(f);

    // Update the reminders
    global.reminderManager->updateReminder(this->lid, dt);
    this->noteUpdated(this->lid);
    this->editor->isDirty = true;
    emit noteAlarmEditedSignal(uuid, lid, false, alarmText.text());
}

void NBrowserWindow::alarmClear() {
    alarmText.setText("");
    alarmText.setVisible(false);

    NoteTable noteTable(global.db);
    noteTable.setDirty(this->lid, true);
    noteTable.removeReminder(this->lid);
    emit(noteUpdated(this->lid));
    emit noteAlarmEditedSignal(uuid, lid, false, "");
}

void NBrowserWindow::alarmMenuActivated() {
    QFont f = alarmText.font();
    f.setStrikeOut(false);
    emit noteAlarmEditedSignal(uuid, lid, false, alarmText.text());
    alarmText.setFont(f);

    NoteTable noteTable(global.db);
    noteTable.setDirty(this->lid, true);
    noteTable.setReminderCompleted(this->lid, false);
    emit(noteUpdated(this->lid));
}



void NBrowserWindow::decryptText(QString id, QString text, QString hint, QString cipher, int len) {
    if (cipher != "RC2") {
        QMessageBox::critical(this, tr("Decryption Error"),
                                       tr("Unknown encryption method.\n"
                                          "Unable to decrypt."));
        return;
    }

    EnCrypt crypt;
    QString plainText = "";
    QUuid uuid;
    QString slot = uuid.createUuid().toString().replace("{","").replace("}","");

    // First, try to decrypt with any keys we already have
    for (int i=0; i<global.passwordRemember.size(); i++) {
        QString password = global.passwordRemember.at(i).first;
        int rc = crypt.decrypt(plainText, text, password, cipher, len);
        if (rc == 0) {
            QPair<QString, QString> newEntry;
            newEntry.first = id;
            newEntry.second = global.passwordRemember.at(i).second;
            global.passwordRemember.append(newEntry);
            removeEncryption(id, plainText, false, slot);
            return;
        }
    }


    EnDecryptDialog dialog;
    if (hint.trimmed() != "")
        dialog.hint->setText(hint);
    while (plainText == "" || !dialog.okPressed) {
        dialog.exec();
        if (!dialog.okPressed) {
            return;
        }
        int rc = crypt.decrypt(plainText, text, dialog.password->text().trimmed());
        if (rc == EnCrypt::Invalid_Key) {
//            QMessageBox.warning(this, tr("Incorrect Password"), tr("The password entered is not correct"));
        }
    }
    QPair<QString,QString> passwordPair;
    passwordPair.first = dialog.password->text().trimmed();
    passwordPair.second = dialog.hint->text().trimmed();
    global.passwordSafe.insert(slot, passwordPair);
    bool permanentlyDecrypt = dialog.permanentlyDecrypt->isChecked();
    removeEncryption(id, plainText, permanentlyDecrypt, slot);
    bool rememberPassword = dialog.rememberPassword->isChecked();
    if (rememberPassword) {
        QPair<QString, QString> pair;
        pair.first = dialog.password->text().trimmed();
        pair.second = dialog.hint->text().trimmed();
        global.passwordRemember.append(pair);
    }
}



void NBrowserWindow::removeEncryption(QString id, QString plainText, bool permanent, QString slot) {
    if (!permanent) {
        plainText = " <table class=\"en-crypt-temp\" slot=\""
                +slot
                +"\""
                +"border=1 width=100%><tbody><tr><td>"
                +plainText+"</td></tr></tbody></table>";
    }

    QString html = editor->page()->mainFrame()->toHtml();
    QString text = html;
    int imagePos = html.indexOf("<img");
    int endPos;
    for ( ;imagePos != -1; ) {
        // Find the end tag
        endPos = text.indexOf(">", imagePos);
        QString tag = text.mid(imagePos-1,endPos);
        if (tag.indexOf("id=\""+id+"\"") > -1) {
                text = text.mid(0,imagePos) +plainText+text.mid(endPos+1);
                editor->page()->mainFrame()->setHtml(text);
                editor->reload();
                if (permanent)
                    contentChanged();
        }
        imagePos = text.indexOf("<img", imagePos+1);
    }
}


void NBrowserWindow::encryptButtonPressed() {
        EnCrypt encrypt;

    QString text = editor->selectedText();
    if (text.trimmed() == "")
        return;
    text = text.replace("\n", "<br/>");

    EnCryptDialog dialog;
    dialog.exec();
    if (!dialog.okPressed()) {
        return;
    }

    EnCrypt crypt;
    QString encrypted;
    int rc = crypt.encrypt(encrypted, text, dialog.getPassword().trimmed());

    if (rc != 0) {
        QMessageBox::information(this, tr("Error"),
                                tr("Error Encrypting String.  Please verify you have Java installed."));
        return;
    }
    QString buffer;
    buffer.append("<img en-tag=\"en-crypt\" cipher=\"RC2\" hint=\""
            + dialog.getHint().replace("'","\\'") + "\" length=\"64\" ");
    buffer.append("contentEditable=\"false\" alt=\"");
    buffer.append(encrypted);
#ifndef _WIN32
    buffer.append("\" src=\"file://").append(global.fileManager.getImageDirPath("encrypt.png") +"\"");
#else
    buffer.append("\" src=\"file:///").append(global.fileManager.getImageDirPath("encrypt.png") +"\"");
#endif
    global.cryptCounter++;
    buffer.append(" id=\"crypt"+QString::number(global.cryptCounter) +"\"");
    buffer.append(" onMouseOver=\"style.cursor=\\'hand\\'\"");
    buffer.append(" onClick=\"window.browserWindow.decryptText(\\'crypt"+QString::number(global.cryptCounter)
                  +"\\', \\'"+encrypted+"\\', \\'"+dialog.getHint().replace("'", "\\&amp;apos;")+"\\', \\'RC2\\', 64);\"");
    buffer.append("style=\"display:block\" />");


    QString script_start = "document.execCommand('insertHtml', false, '";
    QString script_end = "');";
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + buffer + script_end);
}


void NBrowserWindow::sendAuthorUpdateSignal() {
    emit noteAuthorEditedSignal(uuid, lid, dateEditor.authorEditor.getText());
}



void NBrowserWindow::sendLocationUpdateSignal() {
    double longitude, latitude, altitude;
    QString name;
    dateEditor.locationEditor.getGeography(longitude, latitude, altitude, name);
    emit noteLocationEditedSignal(uuid, lid, longitude, latitude, altitude, name);
}


void NBrowserWindow::sendDateCreatedUpdateSignal() {
    emit noteDateEditedSignal(uuid, lid, NOTE_CREATED_DATE, dateEditor.createdDate.dateTime());
}


void NBrowserWindow::sendDateSubjectUpdateSignal() {
    emit noteDateEditedSignal(uuid, lid, NOTE_ATTRIBUTE_SUBJECT_DATE, dateEditor.subjectDate.dateTime());
}




// Send a signal that the note has been updated
void NBrowserWindow::sendTitleUpdateSignal() {
    NoteTable ntable(global.db);
    QString text = this->noteTitle.text().replace("\n"," ").trimmed();
    ntable.updateTitle(this->lid, text, true);
    emit noteTitleEditedSignal(uuid, lid, text);
    emit(this->noteUpdated(lid));
    emit(this->updateNoteList(lid, NOTE_TABLE_TITLE_POSITION, text));
    sendDateUpdateSignal();
}


// Send a signal that the note has been updated
void NBrowserWindow::sendNotebookUpdateSignal() {
    NoteTable ntable(global.db);

//    QString notebook = notebookMenu.d
//    ntable.updateNotebook(this->lid, this->noteTitle.text().trimmed(), true);
//    this->editor->isDirty = true;
    ntable.setDirty(this->lid, true,false);
    emit(this->noteUpdated(lid));
    qint32 lid = notebookMenu.notebookLid;
    QString name = notebookMenu.notebookName;
    emit(this->updateNoteList(lid, NOTE_TABLE_NOTEBOOK_POSITION, name));
    emit(this->updateNoteList(lid, NOTE_TABLE_NOTEBOOK_LID_POSITION, lid));
    emit noteNotebookEditedSignal(uuid, this->lid, lid, name);


    //sendDateUpdateSignal();
}


// Send a signal that the note has been updated
void NBrowserWindow::sendDateUpdateSignal(qint64 dt) {
    NoteTable ntable(global.db);
    ntable.setDirty(this->lid, true);
    if (dt == 0) {
        dt = QDateTime::currentMSecsSinceEpoch();
        this->dateEditor.setUpdateDate(dt);
    }
    emit(this->noteUpdated(lid));
    emit(this->updateNoteList(lid, NOTE_TABLE_DATE_UPDATED_POSITION, dt));
}



// Send a signal that the note has been updated
void NBrowserWindow::sendTagUpdateSignal() {
    NoteTable ntable(global.db);
    ntable.setDirty(this->lid, true,false);
    emit(this->noteUpdated(lid));
    //sendDateUpdateSignal();
    QStringList names;
    tagEditor.getTags(names);
    emit noteTagsEditedSignal(uuid, lid, names);

}


// Send a signal that the note has been updated
void NBrowserWindow::sendUrlUpdateSignal() {
    NoteTable ntable(global.db);
    ntable.setDirty(this->lid, true);
    emit(this->noteUpdated(lid));
    sendDateUpdateSignal();
    emit(this->updateNoteList(lid, NOTE_TABLE_SOURCE_URL_POSITION, urlEditor.getText()));
    emit noteUrlEditedSignal(uuid, lid, urlEditor.getText());

}




void NBrowserWindow::spellCheckPressed() {
    // Check if we have a plugin for Hunspell loaded. This could have been done at startup, but if this is
    // an external window we could need to load it again.
    if (!hunspellInterface) {
        this->loadPlugins();
    }

    // If we STILL don't have a plugin then it can't be loaded. Quit out
    if (!hunspellPluginAvailable) {
        QMessageBox::critical(this, tr("Plugin Error"), tr("Hunspell plugin not found or could not be loaded."), QMessageBox::Ok);
        return;
    }

    QWebPage *page = editor->page();
    page->action(QWebPage::MoveToStartOfDocument);
    page->mainFrame()->setFocus();

    Qt::KeyboardModifier ctrl(Qt::ControlModifier);

    QKeyEvent key(QEvent::KeyPress, Qt::Key_Home, ctrl);
    editor->keyPressEvent(&key);
    page->mainFrame()->setFocus();

    QStringList words = page->mainFrame()->toPlainText().split(" ");
    QStringList ignoreWords;
    QStringList rwords;
    //SpellChecker checker;
    bool finished = false;

    for (int i=0; i<words.size() && !finished; i++) {
        QString currentWord = words[i];
        page->findText(currentWord);
        rwords.clear();
        if (!hunspellInterface->spellCheck(currentWord, rwords) && !ignoreWords.contains(currentWord)) {
            SpellCheckDialog dialog(currentWord, rwords, this);
            dialog.move(0,0);
            dialog.exec();
            if (dialog.cancelPressed)
                finished = true;
            if (dialog.ignoreAllPressed)
                ignoreWords.append(currentWord);
            if (dialog.replacePressed)  {
                QApplication::clipboard()->setText(dialog.replacement);
                pasteButtonPressed();
            }
            if (dialog.addToDictionaryPressed) {
                hunspellInterface->addWord(global.fileManager.getSpellDirPathUser() +"user.lst", currentWord);
            }
        }
    }

    // Go to the end of the document & finish up
    QKeyEvent key2(QEvent::KeyPress, Qt::Key_End, ctrl);
    editor->keyPressEvent(&key2);

    QMessageBox::information(this, tr("Spell Check Complete"), tr("Spell Check Complete."), QMessageBox::Ok);
}



void NBrowserWindow::insertHtmlEntities() {
    emit showHtmlEntities();
}



void NBrowserWindow::hideHtmlEntities() {
    buttonBar->htmlEntitiesButtonVisible->setVisible(false);
    buttonBar->htmlEntitiesButtonAction->setVisible(false);
    editor->insertHtmlEntitiesAction->setVisible(false);
}




void NBrowserWindow::handleUrls(const QMimeData *mime) {
    QList<QUrl> urlList = mime->urls();
    bool ctrlModifier = QApplication::keyboardModifiers() & Qt::ControlModifier;
    for (int i=0; i<urlList.size(); i++) {
        QString file  = urlList[i].toString();
        if (file.toLower().startsWith("file://") && !ctrlModifier) {
#ifndef _WIN32
            attachFileSelected(file.mid(7));
#else
            attachFileSelected(file.mid(8));
#endif
            if (i<urlList.size()-1)
                insertHtml("<div><br/></div>");
        } else if (file.toLower().startsWith("file://") && ctrlModifier) {
            QString url = QString("<a href=\"%1\" title=\"%2\">%3</a>").arg(file).arg(file).arg(file);
            QLOG_DEBUG() << url;
            insertHtml(url);
            if (i<urlList.size()-1)
                insertHtml("<div><br/></div>");
        } else {
            editor->setFocus();
            QApplication::clipboard()->clear();
            QApplication::clipboard()->setText(file, QClipboard::Clipboard);
            this->editor->triggerPageAction(QWebPage::Paste);
        }
    }
}



// This is used to notify the tab window that the contents of a
// note have changed.  It avoids some of the overhead that happens
// when a note is first edited, but it is signaled on every change.
// The tab window uses it to update any duplicate windows (i.e. a note
// was edited in an external editor and is still being viewed internally
// so we need to keep the contents in sync.
void NBrowserWindow::noteContentEdited() {
    emit noteContentEditedSignal(uuid, lid, editor->editorPage->mainFrame()->documentElement().toOuterXml());
}




void NBrowserWindow::changeDisplayFontSize(QString size) {
    bool convert =true;
    if (size.endsWith("px", Qt::CaseInsensitive))
        convert = true;
    size.chop(2);  // Remove px from the end
    int converted = size.toInt();
    if (convert) {
        PixelConverter c;
        converted = c.getPoints(converted);
        size = QString::number(converted);
    }
    int idx = buttonBar->fontSizes->findData(size, Qt::UserRole);
    if (idx > 0) {
        buttonBar->fontSizes->blockSignals(true);
        buttonBar->fontSizes->setCurrentIndex(idx);
        buttonBar->fontSizes->blockSignals(false);
    }
}



// This function is called when the cursor position within the document changes.  It should
// change the combo box to the current font name.
void NBrowserWindow::changeDisplayFontName(QString name) {
    //QLOG_DEBUG() << "Font Name:" << name;
    if (name.startsWith("'")) {
            name = name.mid(1);
            int idx = name.indexOf("'");
            if (idx != -1)
                name = name.mid(0,idx);
    }
    name = name.toLower();
    buttonBar->fontNames->blockSignals(true);
    int idx = buttonBar->fontNames->findData(name, Qt::UserRole);
    if (idx != -1)
        buttonBar->fontNames->setCurrentIndex(idx);
    buttonBar->fontNames->blockSignals(false);
}



void NBrowserWindow::focusCheck() {
    bool buttonBarVisible = false;
    if (editor->hasFocus())
        buttonBarVisible = true;
    if (editor->contextMenu->hasFocus())
        buttonBarVisible = true;
    if (buttonBar->hasFocus())
        buttonBarVisible = true;
    if (buttonBar->fontNames->isExpanded())
        buttonBarVisible = true;
    if (buttonBar->fontNames->lineEdit()->hasFocus())
        buttonBarVisible = true;
    if (buttonBar->fontSizes->lineEdit()->hasFocus())
        buttonBarVisible = true;
    if (buttonBar->fontSizes->isExpanded())
        buttonBarVisible = true;
    if (!global.autoHideEditorToolbar)
        buttonBarVisible = true;
    if (global.isFullscreen)
        buttonBarVisible = false;

    if (!editor->page()->isContentEditable())
        buttonBarVisible = false;
    buttonBar->setVisible(buttonBarVisible);
}




void NBrowserWindow::notebookFocusShortcut() {
    this->notebookMenu.setFocus();
    this->notebookMenu.click();
}



void NBrowserWindow::fontFocusShortcut() {
    if (this->buttonBar->fontNames->isVisible()) {
        this->buttonBar->fontNames->setFocus();
        this->buttonBar->fontNames->showPopup();
    }
}



void NBrowserWindow::fontSizeFocusShortcut() {
    if (this->buttonBar->fontSizes->isVisible()) {
        this->buttonBar->fontSizes->setFocus();
        this->buttonBar->fontSizes->showPopup();
    }
}



void NBrowserWindow::authorFocusShortcut() {
    if (!this->dateEditor.authorEditor.isVisible()) {
        this->changeExpandState(EXPANDBUTTON_3);
        this->expandButton.setState(EXPANDBUTTON_3);
    }
    dateEditor.authorEditor.setFocus();
}

void NBrowserWindow::urlFocusShortcut() {
    if (!this->urlEditor.isVisible()) {
        this->changeExpandState(EXPANDBUTTON_2);
        this->expandButton.setState(EXPANDBUTTON_2);
    }
    this->urlEditor.setFocus();
}




void NBrowserWindow::copyNoteUrl() {
    Note n;
    NoteTable ntable(global.db);
    ntable.get(n,this->lid,false,false);
    UserTable utable(global.db);
    User user;
    utable.getUser(user);

    QString href = "evernote:///view/" + QString::number(user.id) + QString("/") +
           user.shardId +QString("/") +
            n.guid +QString("/") +
            n.guid + QString("/");
    QApplication::clipboard()->setText(href, QClipboard::Clipboard);
}



void NBrowserWindow::newTagFocusShortcut() {
    if (!this->tagEditor.newTag.isVisible()) {
        this->changeExpandState(EXPANDBUTTON_2);
        this->expandButton.setState(EXPANDBUTTON_2);
    }
    tagEditor.newTag.setFocus();
}


// User pressed the superscript editor button
void NBrowserWindow::superscriptButtonPressed() {
    editor->page()->mainFrame()->evaluateJavaScript("document.execCommand('superscript')");
}



// User pressed the subscript editor button
void NBrowserWindow::subscriptButtonPressed() {
    editor->page()->mainFrame()->evaluateJavaScript("document.execCommand('subscript');");
}

// Set the editor background & font color
void NBrowserWindow::setEditorStyle() {
    QString qss = global.getEditorCss();
#ifndef _WIN32
    editor->settings()->setUserStyleSheetUrl(QUrl("file://"+qss));
#else
    editor->settings()->setUserStyleSheetUrl(QUrl("file:///"+qss));
#endif
    return;
}


void NBrowserWindow::loadPlugins() {
    hunspellPluginAvailable = false;

    // Start loading plugins
    QDir pluginsDir(global.fileManager.getProgramDirPath(""));
    pluginsDir.cd("plugins");
    QStringList filter;
    filter.append("libhunspellplugin.so");
    foreach (QString fileName, pluginsDir.entryList(filter)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (fileName == "libhunspellplugin.so") {
            if (plugin) {
                hunspellInterface = qobject_cast<HunspellInterface *>(plugin);
                if (hunspellInterface) {
                    hunspellPluginAvailable = true;
                    hunspellInterface->initialize(global.fileManager.getProgramDirPath(""), global.fileManager.getSpellDirPathUser());
                }
            } else {
                QLOG_ERROR() << pluginLoader.errorString();
            }
        }
    }
}


// Find shortcut activated
void NBrowserWindow::findShortcut() {
    if (!findReplace->isVisible()) {
        findReplace->showFind();
    } else {
        if (findReplace->findLine->hasFocus())
            findReplace->hide();
        else {
            findReplace->showFind();
            findReplace->findLine->setFocus();
            findReplace->findLine->selectAll();
        }
    }

}


//*******************************************
//* Search for the next occurrence of text
//* in a note.
//*******************************************
void NBrowserWindow::findNextShortcut() {
    findReplace->showFind();
    QString find = findReplace->findLine->text();
    if (find != "")
        editor->page()->findText(find,
            findReplace->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
}



//*******************************************
//* Search for the previous occurrence of
//* text in a note.
//*******************************************
void NBrowserWindow::findPrevShortcut() {
    findReplace->showFind();
    QString find = findReplace->findLine->text();
    if (find != "")
        editor->page()->findText(find,
            findReplace->getCaseSensitive() | QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
}



// Find shortcut activated
void NBrowserWindow::findReplaceShortcut() {
    this->findReplace->showFindReplace();
}



//***************************************
//* Find/replace button pressed, so we
//* need to highlight all the occurrences
//* in a note.
//***************************************
void NBrowserWindow::findReplaceInNotePressed() {
    QString find = findReplace->findLine->text();
    QString replace = findReplace->replaceLine->text();
    if (find == "")
        return;
    bool found = false;
    found = editor->page()->findText(find,
        findReplace->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
    if (!found)
        return;

    QApplication::clipboard()->setText(replace);
    editor->pasteAction->trigger();
}




//*************************************************
//* Replace All button pressed.
//*************************************************
void NBrowserWindow::findReplaceAllInNotePressed() {
    QString find = findReplace->findLine->text();
    QString replace = findReplace->replaceLine->text();
    if (find == "")
        return;
    bool found = false;
    while (true) {
        found = editor->page()->findText(find,
            findReplace->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
        if (!found)
            return;
        QApplication::clipboard()->setText(replace);
        editor->pasteAction->trigger();
    }
}




//*******************************************
//* Search for the next occurrence of text
//* in a note.
//*******************************************
void NBrowserWindow::findNextInNote() {
    findReplace->showFind();
    QString find = findReplace->findLine->text();
    if (find != "")
        editor->page()->findText(find,
            findReplace->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
}



//*******************************************
//* Search for the previous occurrence of
//* text in a note.
//*******************************************
void NBrowserWindow::findPrevInNote() {
    findReplace->showFind();
    QString find = findReplace->findLine->text();
    if (find != "")
        editor->page()->findText(find,
            findReplace->getCaseSensitive() | QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);

}




//*******************************************
//* This just does a null find to reset the
//* text in a note so nothing is highlighted.
//* This is triggered when the find dialog
//* box is hidden.
//*******************************************
void NBrowserWindow::findReplaceWindowHidden() {
   editor->page()->findText("");
}




//************************************************
//* Set the current edited cell style in a table
//* This is called from a javascript function to
//* get the current cell style the cursor is in.
//*************************************************
void NBrowserWindow::setTableCellStyle(QString value) {
    this->tableCellStyle = value;
}




//************************************************
//* Set the current table style
//* This is called from a javascript function to
//* get the currenttablel style the cursor is in.
//*************************************************
void NBrowserWindow::setTableStyle(QString value) {
    this->tableStyle = value;
}

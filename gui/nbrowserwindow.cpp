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
#include "global.h"
#include "gui/browserWidgets/colormenu.h"
#include "gui/plugins/pluginfactory.h"
#include "dialog/insertlinkdialog.h"
#include "html/thumbnailer.h"
#include "dialog/tabledialog.h"
#include "dialog/insertlatexdialog.h"
#include "dialog/endecryptdialog.h"
#include "dialog/encryptdialog.h"
#include "sql/configstore.h"
#include "utilities/encrypt.h"
#include "utilities/mimereference.h"
#include "html/attachmenticonbuilder.h"
#include "dialog/remindersetdialog.h"
#include "utilities/spellchecker.h"
#include "dialog/spellcheckdialog.h"


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
#include <iostream>
#include <istream>

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
    line1Layout->addWidget(&noteTitle);
    line1Layout->addWidget(&alarmText);
    line1Layout->addWidget(&alarmButton);
    line1Layout->addWidget(&notebookMenu);
    line1Layout->addWidget(&expandButton);


    // Add the third layout display (which actually appears on line 2)
    layout->addLayout(&line3Layout);
    line3Layout.addWidget(&dateEditor);

    // Add the second layout display (which actually appears on line 3)
    layout->addLayout(&line2Layout);
    line2Layout.addWidget(&urlEditor,1);
    line2Layout.addWidget(&tagEditor, 3);

    editor = new NWebView(this);
    editor->setTitleEditor(&noteTitle);
    setupToolBar();
    layout->addWidget(buttonBar);

    // setup the source editor
    sourceEdit = new QTextEdit(this);
    sourceEdit->setVisible(false);
    sourceEdit->setTabChangesFocus(true);
    //sourceEdit->setLineWrapMode(QTextEdit::LineWrapMode);
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    sourceEdit->setFont(font);
    XmlHighlighter *highlighter = new XmlHighlighter(sourceEdit->document());
    highlighter = highlighter;  // Prevents the unused warning
    sourceEditorTimer = new QTimer();
    connect(sourceEditorTimer, SIGNAL(timeout()), this, SLOT(setSource()));

    // addthe actual note editor & source view
    QSplitter *editorSplitter = new QSplitter(Qt::Vertical, this);
    editorSplitter->addWidget(editor);
    editorSplitter->addWidget(sourceEdit);
    layout->addWidget(editorSplitter);
    setLayout(layout);
    layout->setMargin(0);

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


    // Setup the signals
    connect(&expandButton, SIGNAL(stateChanged(int)), this, SLOT(changeExpandState(int)));
    connect(&notebookMenu, SIGNAL(notebookChanged()), this, SLOT(sendNotebookUpdateSignal()));
    connect(&urlEditor, SIGNAL(textUpdated()), this, SLOT(sendUrlUpdateSignal()));
    connect(&noteTitle, SIGNAL(titleChanged()), this, SLOT(sendTitleUpdateSignal()));
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

    printPage = new QWebView();
    printPage->setVisible(false);
    connect(printPage, SIGNAL(loadFinished(bool)), this, SLOT(printReady(bool)));
    hammer = new Thumbnailer(global.db);
    lid = -1;
    thumbnailer = NULL;

    // Restore the expand/collapse state
    global.settings->beginGroup("SaveState");
    int expandButton = global.settings->value("ExpandButton", EXPANDBUTTON_1).toInt();
    global.settings->endGroup();
    this->expandButton.setState(expandButton);
//    changeExpandState(expandButton);
}



// Setup the toolbar window of the editor
void NBrowserWindow::setupToolBar() {
    buttonBar = new EditorButtonBar();

    // Toolbar action
    connect(buttonBar->undoButtonAction, SIGNAL(triggered()), this, SLOT(undoButtonPressed()));
    connect(buttonBar->redoButtonAction, SIGNAL(triggered()), this, SLOT(redoButtonPressed()));
    connect(buttonBar->cutButtonAction, SIGNAL(triggered()), this, SLOT(cutButtonPressed()));
    connect(buttonBar->copyButtonAction, SIGNAL(triggered()), this, SLOT(copyButtonPressed()));
    connect(buttonBar->pasteButtonAction, SIGNAL(triggered()), this, SLOT(pasteButtonPressed()));
    connect(buttonBar->removeFormatButtonAction, SIGNAL(triggered()), this, SLOT(removeFormatButtonPressed()));
    connect(buttonBar->boldButtonWidget, SIGNAL(clicked()), this, SLOT(boldButtonPressed()));
    connect(buttonBar->italicButtonWidget, SIGNAL(clicked()), this, SLOT(italicsButtonPressed()));
    connect(buttonBar->underlineButtonWidget, SIGNAL(clicked()), this, SLOT(underlineButtonPressed()));
    connect(buttonBar->leftJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignLeftButtonPressed()));
    connect(buttonBar->rightJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignRightButtonPressed()));
    connect(buttonBar->centerJustifyButtonAction, SIGNAL(triggered()), this, SLOT(alignCenterButtonPressed()));
    connect(buttonBar->strikethroughButtonAction, SIGNAL(triggered()), this, SLOT(strikethroughButtonPressed()));
    connect(buttonBar->hlineButtonAction, SIGNAL(triggered()), this, SLOT(horizontalLineButtonPressed()));
    connect(buttonBar->shiftRightButtonAction, SIGNAL(triggered()), this, SLOT(shiftRightButtonPressed()));
    connect(buttonBar->shiftLeftButtonAction, SIGNAL(triggered()), this, SLOT(shiftLeftButtonPressed()));
    connect(buttonBar->bulletListButtonAction, SIGNAL(triggered()), this, SLOT(bulletListButtonPressed()));
    connect(buttonBar->numberListButtonAction, SIGNAL(triggered()), this, SLOT(numberListButtonPressed()));
    connect(buttonBar->todoButtonAction, SIGNAL(triggered()), this, SLOT(todoButtonPressed()));
    connect(buttonBar->spellCheckButtonAction, SIGNAL(triggered()), this, SLOT(spellCheckPressed()));
    connect(buttonBar->fontSizes, SIGNAL(currentIndexChanged(int)), this, SLOT(fontSizeSelected(int)));
    connect(buttonBar->fontNames, SIGNAL(currentIndexChanged(int)), this, SLOT(fontNameSelected(int)));
    connect(buttonBar->fontColorButtonWidget, SIGNAL(clicked()), this, SLOT(fontColorClicked()));
    connect(buttonBar->fontColorMenuWidget->getMenu(), SIGNAL(triggered(QAction*)), this, SLOT(fontColorClicked()));
    connect(buttonBar->highlightColorButtonWidget, SIGNAL(clicked()), this, SLOT(fontHighlightClicked()));
    connect(buttonBar->highlightColorMenuWidget->getMenu(), SIGNAL(triggered(QAction*)), this, SLOT(fontHighlightClicked()));
    connect(buttonBar->insertTableButtonAction, SIGNAL(triggered()), this, SLOT(insertTableButtonPressed()));
    connect(buttonBar->htmlEntitiesButtonAction, SIGNAL(triggered()), this, SLOT(insertHtmlEntities()));
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

    //hammer->timer.stop();
    // First, make sure we have a valid lid
    if (lid == -1) {
        blockSignals(true);
        setReadOnly(true);
        clear();
        blockSignals(false);
        return;
    }

    // If we are already updating this note, we don't do anything
    if (lid == this->lid)
        return;

    if (this->editor->isDirty)
        this->saveNoteContent();

    // let's load the new note
    this->lid = lid;
    this->editor->isDirty = false;

    NoteTable noteTable(global.db);
    Note n;

    bool rc = noteTable.get(n, this->lid, false, false);
    if (!rc)
        return;

    QByteArray content;
    bool inkNote;
    bool readOnly;

    // If we are searching, we never pull from the cache since the search string may
    // have changed since the last time.
    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (criteria->isSearchStringSet() && criteria->getSearchString().trimmed() != "")
        global.cache.remove(lid);

    if (!global.cache.contains(lid)) {
        NoteFormatter formatter;
        if (criteria->isSearchStringSet())
            formatter.setHighlightText(criteria->getSearchString());
        formatter.setNote(n, global.pdfPreview);
        formatter.setHighlight();
        content = formatter.rebuildNoteHTML();
        if (!criteria->isSearchStringSet()) {
            NoteCache *newCache = new NoteCache();
            newCache->isReadOnly = formatter.readOnly;
            newCache->isInkNote = formatter.inkNote;
            newCache->noteContent = content;
            global.cache.insert(lid, newCache);
        }
        readOnly = formatter.readOnly;
        inkNote = formatter.inkNote;
    } else {
        NoteCache *c = global.cache[lid];
        content = c->noteContent;
        readOnly = c->isReadOnly;
        inkNote = c->isInkNote;
    }

    setReadOnly(readOnly);

    noteTitle.setTitle(lid, n.title, n.title);
    dateEditor.setNote(lid, n);
    //QLOG_DEBUG() << content;
    //editor->setContent(content,  "application/xhtml+xml");
    QWebSettings::setMaximumPagesInCache(0);
    QWebSettings::setObjectCacheCapacities(0, 0, 0);
    editor->setContent(content);
    // is this an ink note?
    if (inkNote)
        editor->page()->setContentEditable(false);

    // Setup the alarm
    NoteAttributes attributes;
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
        //alarmText.setText(atime.toString(Qt::SystemLocaleShortDate));
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

    this->lid = lid;
    notebookMenu.setCurrentNotebook(lid, n);
    NoteAttributes na;
    if (n.attributes.isSet())
        na = n.attributes;
    if (na.sourceURL.isSet())
        urlEditor.setUrl(lid, na.sourceURL);
    else
        urlEditor.setUrl(lid, "");
    setSource();

    if (criteria->isSearchStringSet()) {
        QStringList list = criteria->getSearchString().split(" ");
        for (int i=0; i<list.size(); i++) {
            editor->page()->findText(list[i], QWebPage::HighlightAllOccurrences);
        }
    }

    if (hammer->idle && noteTable.isThumbnailNeeded(this->lid)) {
        hammer->render(this->lid);
    } /*else
        hammer->timer.start(1000);*/

}


void NBrowserWindow::setReadOnly(bool readOnly) {
    if (readOnly) {
        noteTitle.setFocusPolicy(Qt::NoFocus);
        tagEditor.setEnabled(false);
        tagEditor.setFocusPolicy(Qt::NoFocus);
        authorEditor.setFocusPolicy(Qt::NoFocus);
        locationEditor.setFocusPolicy(Qt::NoFocus);
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
    authorEditor.setFocusPolicy(Qt::StrongFocus);
    locationEditor.setFocusPolicy(Qt::StrongFocus);
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
//    if (sourceEdit->isVisible()) {
//        sourceEditorTimer->stop();
//        sourceEditorTimer->setInterval(500);
//        sourceEditorTimer->setSingleShot(false);
//        sourceEditorTimer->start();
//    }
}


// Save the note's content
void NBrowserWindow::saveNoteContent() {
    // Do a little bit of focus changing to make sure things are saved properly
    this->editor->setFocus();
    microFocusChanged();
    this->editor->titleEditor->setFocus();


    if (this->editor->isDirty) {
        NoteTable table(global.db);
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

        for (int i=0; i<oldLids.size(); i++) {
            if (!validLids.contains(oldLids[i])) {
                resTable.expunge(oldLids[i]);
            }
        }



        table.updateNoteContent(lid, formatter.getEnml());
        editor->isDirty = false;
        if (thumbnailer == NULL)
            thumbnailer = new Thumbnailer(global.db);
        thumbnailer->render(lid);

        NoteCache* cache = global.cache[lid];
        if (cache != NULL) {
            QByteArray b;
            b.append(contents);
            cache->noteContent = b;
            global.cache.remove(lid);
            global.cache.insert(lid, cache);
        }

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
    this->editor->triggerPageAction(QWebPage::Copy);
    this->editor->setFocus();
    microFocusChanged();
}


// The paste button was pressed
void NBrowserWindow::pasteButtonPressed() {
    if (forceTextPaste) {
        pasteWithoutFormatButtonPressed();
        return;
    }

    QClipboard *clipboard = global.clipboard;
    const QMimeData *mime = clipboard->mimeData();

    if (mime->hasImage()) {
        editor->setFocus();
        insertImage(mime);
        editor->setFocus();
        return;
    }

    if (mime->hasUrls()) {
        QList<QUrl> urls = mime->urls();
        for (int i=0; i<urls.size(); i++) {
            if (urls[i].toString().startsWith("file://")) {
                QString fileName = urls[i].toString().mid(7);
                attachFileSelected(fileName);
//                addAttachment(fileName);
                this->editor->triggerPageAction(QWebPage::InsertParagraphSeparator);
            }
        }
        this->editor->setFocus();
        microFocusChanged();
        return;
    }
    QLOG_DEBUG() << "HTML:" << mime->hasHtml() << " " << mime->html();
    QLOG_DEBUG() << "Color:" << mime->hasColor();
    QLOG_DEBUG() << "Url:" << mime->hasUrls();

    if (mime->hasText()) {
        QString urltext = mime->text();
        if (urltext.toLower().mid(0,17) == "evernote:///view/") {
            urltext = urltext.mid(17);
            int pos = urltext.indexOf("/");
            QString userid = urltext.mid(0,pos-1);
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            QString shard = urltext.mid(0,pos);
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            QString uid = urltext.mid(0,pos);
            urltext = urltext.mid(pos+1);
            pos = urltext.indexOf("/");
            QString guid = urltext.mid(0,pos);
            urltext = urltext.mid(pos);
            pos = urltext.indexOf("/");
            QString locguid = urltext.mid(pos);
            QString linkedNotebookGuid = urltext.mid(pos);

            Note n;
            bool goodrc = false;
            NoteTable ntable(global.db);
            goodrc = ntable.get(n, guid,false, false);
            if (!goodrc)
                goodrc = ntable.get(n,locguid,false, false);

            // If we have a good return, then we can paste the link, otherwise we fall out
            // to a normal paste.
            if (goodrc) {
                QString url = QString("<a href=\"") +global.clipboard->text()
                        +QString("\" title=") +n.title
                        +QString(" >") +n.title +QString("</a>");
                QString script = QString("document.execCommand('insertHtml', false, '")+url+QString("');");
                editor->page()->mainFrame()->evaluateJavaScript(script);
                return;
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
    const QMimeData *mime = global.clipboard->mimeData();
    if (!mime->hasText())
        return;
    QString text = mime->text();
    global.clipboard->clear();
    global.clipboard->setText(text, QClipboard::Clipboard);
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
    this->editor->triggerPageAction(QWebPage::ToggleBold);
    this->editor->setFocus();
    microFocusChanged();
}



// The toggled button was pressed/toggled
void NBrowserWindow::italicsButtonPressed() {
    this->editor->triggerPageAction(QWebPage::ToggleItalic);
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



// The left allign button was pressed
void NBrowserWindow::alignLeftButtonPressed() {
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('JustifyLeft', false, '');");
    editor->setFocus();
    microFocusChanged();
}



// The allign right button was pressed
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
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + todo + script_end);
    editor->setFocus();
    microFocusChanged();
}



// The font size button was pressed
void NBrowserWindow::fontSizeSelected(int index) {
    int size = buttonBar->fontSizes->itemData(index).toInt();

    if (size <= 0)
        return;

    QString text = editor->selectedText();
    if (text.trimmed() == "")
        return;

    QString newText = "<span style=\"font-size:" +QString::number(size) +"pt;\">"+text+"</span>";
    QString script = QString("document.execCommand('insertHtml', false, '"+newText+"');");
    QLOG_DEBUG() << script;
    editor->page()->mainFrame()->evaluateJavaScript(script);
    editor->setFocus();
    microFocusChanged();
}



void NBrowserWindow::insertHtml(QString html) {
    QString script = QString("document.execCommand('insertHtml', false, '"+html+"');");
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
    QString text = editor->selectedText();
    if (text.trimmed() == "" && currentHyperlink.trimmed() == "")
        return;

    InsertLinkDialog dialog(insertHyperlink);
    if (currentHyperlink != NULL && currentHyperlink != "") {
        dialog.setUrl(currentHyperlink);
    }
    dialog.exec();
    if (!dialog.okButtonPressed()) {
        return;
    }

    // Take care of inserting new links
    if (insertHyperlink) {
        QString selectedText = editor->selectedText();
        if (dialog.getUrl().trimmed() == "")
            return;
        QString dUrl = dialog.getUrl().trimmed().replace("'", "\\'");
        QString url = QString("<a href=\"") +dUrl
                +QString("\" title=") +dUrl
                +QString(" >") +selectedText +QString("</a>");
        QString script = QString("document.execCommand('insertHtml', false, '")+url+QString("');");
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
                n.guid;

        QString url = QString("<a href=\"") +href
                +QString("\" title=") +text
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

    QString newHTML = QString("<table border=\"1\" width=\"") +QString::number(width);
    if (percent)
        newHTML = newHTML +"%";
    newHTML = newHTML + "\"><tbody>";

    for (int i=0; i<rows; i++) {
        newHTML = newHTML +"<tr>";
        for (int j=0; j<cols; j++) {
            newHTML = newHTML +"<td>&nbsp;</td>";
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
        "           for (j=0; j<nodes.length; j=j+1) {"
        "              cell = document.createElement('TD');"
        "              cell.innerHTML='&nbsp;';"
        "              row.appendChild(cell);"
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
    QString js = "function insertTableColumn() {"
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
            "      var cell = workingNode.rows[i].insertCell(current+1); "
            "      cell.innerHTML = '&nbsp'; "
            "   }"
            "} insertTableColumn();";
        editor->page()->mainFrame()->evaluateJavaScript(js);
        contentChanged();
}


void NBrowserWindow::deleteTableRowButtonPressed() {
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
    for (; pos>0; pos=content.indexOf("<img ", pos+1) ) {
        int endPos = content.indexOf(">", pos);
        QString section = content.mid(pos, endPos-pos);
        if (section.indexOf("lid=\"" +QString::number(selectedFileLid) + "\"") > 0) {
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
     editor->deleteTableRowAction->setEnabled(false);
     editor->deleteTableColumnAction->setEnabled(false);
     editor->insertLinkAction->setText(tr("Insert Link"));
     editor->removeLinkAction->setEnabled(false);
     editor->insertQuickLinkAction->setEnabled(true);
     editor->rotateImageRightAction->setEnabled(false);
     editor->rotateImageLeftAction->setEnabled(false);

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

    QString js = QString("function getCursorPos() {")
        +QString("var cursorPos;")
        +QString("if (window.getSelection) {")
        +QString("   var selObj = window.getSelection();")
        +QString("   var selRange = selObj.getRangeAt(0);")
        +QString("   var workingNode = window.getSelection().anchorNode.parentNode;")
        +QString("   while(workingNode != null) { ")
        //+QString("      window.browserWindow.printNodeName(workingNode.nodeName);")
        +QString("      if (workingNode.nodeName=='TABLE') { if (workingNode.getAttribute('class').toLowerCase() == 'en-crypt-temp') window.browserWindow.insideEncryptionArea(); }")
        +QString("      if (workingNode.nodeName=='B') window.browserWindow.boldActive();")
        +QString("      if (workingNode.nodeName=='I') window.browserWindow.italicsActive();")
        +QString("      if (workingNode.nodeName=='U') window.browserWindow.underlineActive();")
        +QString("      if (workingNode.nodeName=='UL') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='OL') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='LI') window.browserWindow.setInsideList();")
        +QString("      if (workingNode.nodeName=='TBODY') window.browserWindow.setInsideTable();")
        +QString("      if (workingNode.nodeName=='A') {for(var x = 0; x < workingNode.attributes.length; x++ ) {if (workingNode.attributes[x].nodeName.toLowerCase() == 'href') window.browserWindow.setInsideLink(workingNode.attributes[x].nodeValue);}}")
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
        return;
    }
    QKeyEvent *down = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QCoreApplication::postEvent(editor->editorPage, down);
}



// Set the backgroud color of a note
 void NBrowserWindow::setBackgroundColor(QString value) {
     QString js = QString("function changeBackground(color) {")
         +QString("document.body.style.background = color;")
         +QString("}")
         +QString("changeBackground('" +value+"');");
     editor->page()->mainFrame()->evaluateJavaScript(js);
     editor->setFocus();
     microFocusChanged();
 }


 // The user clicked a link in the note
 void NBrowserWindow::linkClicked(const QUrl url) {
     if (url.toString().startsWith("latex://", Qt::CaseInsensitive)) {
         editLatex(url.toString().mid(8));
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


         // Setup a new filter
         FilterCriteria *criteria = new FilterCriteria();
         global.filterCriteria[global.filterPosition]->duplicate(*criteria);
         criteria->unsetSelectedNotes();
         criteria->unsetLid();
         criteria->setLid(newlid);
         global.appendFilter(criteria);
         global.filterPosition++;
         emit(evernoteLinkClicked(newlid, false));
         return;
     }
     if (url.toString().startsWith("nnres:", Qt::CaseInsensitive)) {
         if (url.toString().endsWith("/vnd.evernote.ink")) {
             QMessageBox::information(this, tr("Unable Open"), QString(tr("This is an ink note.\nInk notes are not supported since Evernote has not\n published any specifications on them\nand I'm too lazy to figure them out by myself.")));
             return;
         }
         QString fullName = url.toString().mid(6);
         int index = fullName.lastIndexOf(".");
         QString guid = "";
         QString type = "";
         if (index >-1) {
             type = fullName.mid(index);
             guid = fullName.mid(0,index).replace(global.fileManager.getDbaDirPath(),"");
         } else
             return;
         QString fileUrl = global.fileManager.getDbaDirPath()+guid +type;
         global.resourceWatcher.addPath(fileUrl);
         QDesktopServices::openUrl(fileUrl);
         return;
     }
     QDesktopServices::openUrl(url);
 }



 // show/hide view source window
void NBrowserWindow::showSource(bool value) {
     setSource();
     sourceEdit->setVisible(value);
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

    authorEditor.blockSignals(true);
    authorEditor.setText("");
    authorEditor.blockSignals(false);

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
    if (body > 0) {
        body = text.indexOf(">",body);
        if (body > 0) {
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




// Edit a latex formula
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
        text = text.replace("'", "\\'");
    QProcess latexProcess;
    QStringList args;
    args.append("-e");
    args.append(outfile);
    args.append(text);
    QString formula = "mimetex -e "+outfile +" '" +text +"'";
    QLOG_DEBUG() << "Formula:" << formula;
    //latexProcess.start(formula, QIODevice::ReadWrite|QIODevice::Unbuffered);
    latexProcess.start("mimetex", args, QIODevice::ReadWrite|QIODevice::Unbuffered);

    QLOG_DEBUG() << "Starting mimetex " << latexProcess.waitForStarted();
    QLOG_DEBUG() << "Stopping mimetex " << latexProcess.waitForFinished() << " Return Code: " << latexProcess.state();
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
    buffer.append("<a onmouseover=\"cursor:&apos;hand&apos; title=\"");
    buffer.append(text);
    buffer.append("\" href=\"latex://");
    buffer.append(QString::number(newlid));
    buffer.append("\">");
    buffer.append("<img src=\"file://");
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
        int endPos = oldHtml.indexOf("</a>", startPos);
        while (startPos > 0) {
            if (endPos > 0) {
                QString slice = oldHtml.mid(startPos, endPos-startPos+4);
                if (slice.indexOf("lid=\""+guid+"\"") && slice.indexOf("en-latex")) {
                    oldHtml.replace(slice, buffer);
                }
                startPos = oldHtml.indexOf("<a", endPos);
                editor->page()->mainFrame()->setHtml(oldHtml);
                editor->reload();
                contentChanged();
            }
        }
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
    QByteArray imageBa;
    QBuffer b(&imageBa);
    b.open(QIODevice::WriteOnly);
    img.save(&b, "JPG");

    QString script_start = "document.execCommand('insertHTML', false, '";
    QString script_end = "');";

    Resource newRes;
    qint32 rlid = createResource(newRes, 0, imageBa, "image/jpeg", false, "");
    if (rlid <= 0)
        return;

    // The resource is done, now we need to add it to the
    // note body
    QString g =  QString::number(rlid)+QString(".jpg");
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
    buffer.append(path);
    buffer.append("\" type=\"image/jpeg\" hash=\"");
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


// Print the contents of a note.  Basically it loops through the
// note and repaces the <object> tags with <img> tags.  The plugin
// object should be creating temporary images for the print.
void NBrowserWindow::printNote() {
    QString contents = editor->editorPage->mainFrame()->toHtml();

    // Start removing object tags
    int pos = contents.indexOf("<object");
    while (pos>=0) {
        int endPos = contents.indexOf(">", pos);
        QString lidString = contents.mid(contents.indexOf("lid=", pos)+5);
        lidString = lidString.mid(0,lidString.indexOf("\" "));
        contents = contents.mid(0,pos) + "<img src=\"file://" +
                global.fileManager.getTmpDirPath() + lidString +
                QString("-print.png\" width=\"10%\" height=\"10%\"></img>")+contents.mid(endPos+1);

        pos = contents.indexOf("<object", endPos);
    }

    // Load the print page.  When it is ready the printReady() slot will
    // do the actual print
    printPage->setContent(contents.toUtf8());
}


void NBrowserWindow::printReady(bool ok) {
    if (!ok) {
        QMessageBox msgBox;
        msgBox.setText(tr("Error loading document for printing.\nPrinting aborted."));
        msgBox.setWindowTitle(tr("Print Error"));
        msgBox.exec();
        return;
    }

    QPrinter *printer;

    if (fastPrint) {
        global.settings->beginGroup("Printer");
        QPrinter::Orientation orientation = static_cast<QPrinter::Orientation>(global.settings->value("orientation").toUInt());
        QString name = global.settings->value("printerName", "").toString();
        QPrinter::OutputFormat format = static_cast<QPrinter::OutputFormat>(global.settings->value("outputFormat").toUInt());
        QString fileName = global.settings->value("outputFileName", "").toString();
        global.settings->endGroup();

        bool error = false;
        printer = new QPrinter();
        printer->setOutputFormat(format);
        printer->setOrientation(orientation);
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
            delete printer;
        }
    }

    if (!fastPrint) {
        QPrintDialog dialog;
        if (dialog.exec() ==  QDialog::Accepted) {
            printer = dialog.printer();
            printPage->print(printer);
            global.settings->beginGroup("Printer");
            global.settings->setValue("orientation", printer->orientation());
            global.settings->setValue("printerName", printer->printerName());
            global.settings->setValue("outputFormat", printer->outputFormat());
            global.settings->setValue("outputFileName", printer->outputFileName());
            global.settings->endGroup();
        }
    } else
        printPage->print(printer);

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
    while (pos>0) {
        endPos = content.indexOf(">", pos);  // Find the matching end of the tag
        hashPos = content.indexOf(hashString, pos);
        if (hashPos < endPos && hashPos > 0) {  // If we found the hash, begin the update
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
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    file.close();

    QString script_start = "document.execCommand('insertHTML', false, '";
    QString script_end = "');";

    MimeReference mimeRef;
    QString extension = filename;
    int endPos = filename.lastIndexOf(".");
    if (endPos>0)
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

    if (mime == "application/pdf") {
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
    buffer.append(tmpFile);
    buffer.append("\" />");
    buffer.append("</a>");

    // Insert the actual attachment
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + buffer + script_end);
}



// Alarm has been completed
void NBrowserWindow::alarmCompleted() {
    QFont f = alarmText.font();
    bool completed = false;
    if (!f.strikeOut())
        completed = true;
    f.setStrikeOut(completed);
    alarmText.setFont(f);

    NoteTable noteTable(global.db);
    noteTable.setDirty(this->lid, true);
    noteTable.setReminderCompleted(this->lid, completed);
    global.reminderManager->remove(this->lid);
    emit(noteUpdated(this->lid));
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
}

void NBrowserWindow::alarmClear() {
    alarmText.setText("");
    alarmText.setVisible(false);

    NoteTable noteTable(global.db);
    noteTable.setDirty(this->lid, true);
    noteTable.removeReminder(this->lid);
    emit(noteUpdated(this->lid));
}

void NBrowserWindow::alarmMenuActivated() {
    QFont f = alarmText.font();
    f.setStrikeOut(false);
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
    for ( ;imagePos>0; ) {
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
    buffer.append("\" src=\"file://").append(global.fileManager.getImageDirPath("encrypt.png") +"\"");
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




// Send a signal that the note has been updated
void NBrowserWindow::sendTitleUpdateSignal() {
    NoteTable ntable(global.db);
    ntable.updateTitle(this->lid, this->noteTitle.text().trimmed(), true);
    emit(this->noteUpdated(lid));
    emit(this->updateNoteList(lid, NOTE_TABLE_TITLE_POSITION, this->noteTitle.text()));
    sendDateUpdateSignal();
}


// Send a signal that the note has been updated
void NBrowserWindow::sendNotebookUpdateSignal() {
    NoteTable ntable(global.db);

//    QString notebook = notebookMenu.d
//    ntable.updateNotebook(this->lid, this->noteTitle.text().trimmed(), true);
//    this->editor->isDirty = true;
    ntable.setDirty(this->lid, true);
    emit(this->noteUpdated(lid));
    qint32 lid = notebookMenu.notebookLid;
    QString name = notebookMenu.notebookName;
    emit(this->updateNoteList(lid, NOTE_TABLE_NOTEBOOK_POSITION, name));
    emit(this->updateNoteList(lid, NOTE_TABLE_NOTEBOOK_LID_POSITION, lid));
    sendDateUpdateSignal();
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
    ntable.setDirty(this->lid, true);
    emit(this->noteUpdated(lid));
    sendDateUpdateSignal();
}


// Send a signal that the note has been updated
void NBrowserWindow::sendUrlUpdateSignal() {
    NoteTable ntable(global.db);
    ntable.setDirty(this->lid, true);
    emit(this->noteUpdated(lid));
    sendDateUpdateSignal();
    emit(this->updateNoteList(lid, NOTE_TABLE_SOURCE_URL_POSITION, urlEditor.getText()));
}




void NBrowserWindow::spellCheckPressed() {
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
    SpellChecker checker;
    checker.setup();
    bool finished = false;

    for (int i=0; i<words.size() && !finished; i++) {
        QString currentWord = words[i];
        page->findText(currentWord);
        if (!checker.spellCheck(currentWord, rwords) && !ignoreWords.contains(currentWord)) {
            SpellCheckDialog dialog(currentWord, rwords, this);
            dialog.move(0,0);
            dialog.exec();
            if (dialog.cancelPressed)
                finished = true;
            if (dialog.ignoreAllPressed)
                ignoreWords.append(currentWord);
            if (dialog.replacePressed)  {
                QClipboard *clipboard = global.clipboard;
                clipboard->setText(dialog.replacement);
                pasteButtonPressed();
            }
            if (dialog.addToDictionaryPressed) {
                checker.addWord(currentWord);
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
    for (int i=0; i<urlList.size(); i++) {
        QString file  = urlList[i].toString();
        if (file.toLower().startsWith("file://"))
            attachFileSelected(file.mid(7));
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
    size.chop(2);  // Remove px from the end
    int idx = buttonBar->fontSizes->findData(size, Qt::UserRole);
    if (idx > 0) {
        buttonBar->fontSizes->blockSignals(true);
        buttonBar->fontSizes->setCurrentIndex(idx);
        buttonBar->fontSizes->blockSignals(false);
    }
}


void NBrowserWindow::changeDisplayFontName(QString name) {
    if (name.startsWith("'")) {
            name = name.mid(1);
            name.chop(1);
    }
    buttonBar->fontNames->blockSignals(true);
    int idx = buttonBar->fontNames->findData(name, Qt::UserRole);
    if (idx >=0)
        buttonBar->fontNames->setCurrentIndex(idx);
    buttonBar->fontNames->blockSignals(false);
}

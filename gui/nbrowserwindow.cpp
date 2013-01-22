#include "nbrowserwindow.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "gui/browserWidgets/urleditor.h"
#include "sql/tagtable.h"
#include "html/noteformatter.h"
#include "html/enmlformatter.h"
#include "sql/resourcetable.h"
#include "global.h"

#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFontDatabase>
#include <QSplitter>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include "gui/browserWidgets/colormenu.h"
#include "gui/browserWidgets/toolbarwidgetaction.h"
#include "dialog/insertlinkdialog.h"
#include "dialog/tabledialog.h"

extern Global global;

NBrowserWindow::NBrowserWindow(QWidget *parent) :
    QWidget(parent)
{

//    this->setStyleSheet("margins:0px;");
    QHBoxLayout *line1Layout = new QHBoxLayout();
    QVBoxLayout *layout = new QVBoxLayout();   // Note content layout

    // Setup line #1 of the window.  The text & notebook
    layout->addLayout(line1Layout);
    line1Layout->addWidget(&noteTitle);
    line1Layout->addWidget(&notebookMenu);
    line1Layout->addWidget(&expandButton);


    // Add the third layout display (which actually appears on line 2)
    layout->addLayout(&line3Layout);
    line3Layout.addWidget(&dateEditor);

    // Add the second layout display (which actually appears on line 3)
    layout->addLayout(&line2Layout);
    line2Layout.addWidget(&urlEditor,1);
    line2Layout.addWidget(&tagEditor, 3);

    // Setup the toolbar button
    fontNames = new QComboBox();
    fontSize = new QComboBox();
    this->loadFontNames();

    editor = new NWebView(this);
    editor->setTitleEditor(&noteTitle);
    layout->addWidget(&buttonBar);
    setupToolBar();

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
    connect(&notebookMenu, SIGNAL(notebookChanged()), this, SLOT(sendUpdateSignal()));
    connect(&urlEditor, SIGNAL(textUpdated()), this, SLOT(sendUpdateSignal()));
    connect(&noteTitle, SIGNAL(titleChanged()), this, SLOT(sendUpdateSignal()));
    connect(&dateEditor, SIGNAL(valueChanged()), this, SLOT(sendUpdateSignal()));
    connect(&tagEditor, SIGNAL(tagsUpdated()), this, SLOT(sendUpdateSignal()));
    connect(&tagEditor, SIGNAL(newTagCreated(qint32)), this, SLOT(newTagAdded(qint32)));
    connect(editor, SIGNAL(noteChanged()), this, SLOT(noteContentUpdated()));

    connect(editor->page(), SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
    connect(editor->page(), SIGNAL(microFocusChanged()), this, SLOT(microFocusChanged()));

    editor->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(editor->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(exposeToJavascript()));
    connect(editor->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), editor, SLOT(exposeToJavascript()));

    lid = -1;
}



// Setup the toolbar window of the editor
void NBrowserWindow::setupToolBar() {
    undoButtonAction = new ToolbarWidgetAction();
    undoButtonAction->setType(ToolbarWidgetAction::Undo);
    undoButtonAction->createWidget(0);
    buttonBar.addAction(undoButtonAction);

    redoButtonAction = new ToolbarWidgetAction();
    redoButtonAction->setType(ToolbarWidgetAction::Redo);
    redoButtonAction->createWidget(0);
    buttonBar.addAction(redoButtonAction);

    buttonBar.addSeparator();

    cutButtonAction = new ToolbarWidgetAction();
    cutButtonAction->setType(ToolbarWidgetAction::Cut);
    cutButtonAction->createWidget(0);
    buttonBar.addAction(cutButtonAction);

    copyButtonAction = new ToolbarWidgetAction();
    copyButtonAction->setType(ToolbarWidgetAction::Copy);
    copyButtonAction->createWidget(0);
    buttonBar.addAction(copyButtonAction);

    pasteButtonAction = new ToolbarWidgetAction();
    pasteButtonAction->setType(ToolbarWidgetAction::Paste);
    pasteButtonAction->createWidget(0);
    buttonBar.addAction(pasteButtonAction);

    buttonBar.addSeparator();

    boldButtonAction = new ToolbarWidgetAction();
    boldButtonAction->setType(ToolbarWidgetAction::Bold);
    boldButtonAction->createWidget(0);
    buttonBar.addAction(boldButtonAction);


    italicsButtonAction = new ToolbarWidgetAction();
    italicsButtonAction->setType(ToolbarWidgetAction::Italics);
    italicsButtonAction->createWidget(0);
    buttonBar.addAction(italicsButtonAction);

    underlineButtonAction = new ToolbarWidgetAction();
    underlineButtonAction->setType(ToolbarWidgetAction::Underline);
    underlineButtonAction->createWidget(0);
    buttonBar.addAction(underlineButtonAction);

    strikethroughButtonAction = new ToolbarWidgetAction();
    strikethroughButtonAction->setType(ToolbarWidgetAction::Strikethrough);
    strikethroughButtonAction->createWidget(0);
    buttonBar.addAction(strikethroughButtonAction);

    buttonBar.addSeparator();

    leftAlignButtonAction = new ToolbarWidgetAction();
    leftAlignButtonAction->setType(ToolbarWidgetAction::AlignLeft);
    leftAlignButtonAction->createWidget(0);
    buttonBar.addAction(leftAlignButtonAction);

    centerAlignButtonAction = new ToolbarWidgetAction();
    centerAlignButtonAction->setType(ToolbarWidgetAction::AlignCenter);
    centerAlignButtonAction->createWidget(0);
    buttonBar.addAction(centerAlignButtonAction);

    rightAlignButtonAction = new ToolbarWidgetAction();
    rightAlignButtonAction->setType(ToolbarWidgetAction::AlignRight);
    rightAlignButtonAction->createWidget(0);
    buttonBar.addAction(rightAlignButtonAction);

    buttonBar.addSeparator();

    hlineButtonAction = new ToolbarWidgetAction();
    hlineButtonAction->setType(ToolbarWidgetAction::HorizontalLine);
    hlineButtonAction->createWidget(0);
    buttonBar.addAction(hlineButtonAction);

    shiftRightButtonAction = new ToolbarWidgetAction();
    shiftRightButtonAction->setType(ToolbarWidgetAction::ShiftRight);
    shiftRightButtonAction->createWidget(0);
    buttonBar.addAction(shiftRightButtonAction);

    shiftLeftButtonAction = new ToolbarWidgetAction();
    shiftLeftButtonAction->setType(ToolbarWidgetAction::ShiftLeft);
    shiftLeftButtonAction->createWidget(0);
    buttonBar.addAction(shiftLeftButtonAction);

    bulletListButtonAction = new ToolbarWidgetAction();
    bulletListButtonAction->setType(ToolbarWidgetAction::BulletList);
    bulletListButtonAction->createWidget(0);
    buttonBar.addAction(bulletListButtonAction);

    numberListButtonAction = new ToolbarWidgetAction();
    numberListButtonAction->setType(ToolbarWidgetAction::NumberList);
    numberListButtonAction->createWidget(0);
    buttonBar.addAction(numberListButtonAction);

    buttonBar.addSeparator();
    buttonBar.addWidget(fontNames);
    buttonBar.addWidget(fontSize);

    fontColor = new QToolButton();
    fontColor->setIcon(QIcon(":fontColor.png"));
    fontColor->setToolTip(tr("Font Color"));
    fontColor->setPopupMode(QToolButton::MenuButtonPopup);
    fontColor->setMenu(fontColorMenu.getMenu());
    fontColor->setAutoRaise(false);
    buttonBar.addWidget(fontColor);

    highlightColor = new QToolButton();
    highlightColor->setIcon(QIcon(":fontHighlight.png"));
    highlightColor->setToolTip(tr("Highlight Color"));
    highlightColor->setPopupMode(QToolButton::MenuButtonPopup);
    highlightColor->setAutoRaise(false);
    highlightColor->setMenu(highlightColorMenu.getMenu());
    highlightColorMenu.setDefault(QColor("yellow"));
    buttonBar.addWidget(highlightColor);

    buttonBar.addSeparator();

//    spellButtonAction = new ToolbarWidgetAction();
//    spellButtonAction->setType(ToolbarWidgetAction::SpellCheck);
//    spellButtonAction->createWidget(0);
//    buttonBar.addAction(spellButtonAction);
//    spellButtonAction->setVisible(false);

    todoButtonAction = new ToolbarWidgetAction();
    todoButtonAction->setType(ToolbarWidgetAction::Todo);
    todoButtonAction->createWidget(0);
    buttonBar.addAction(todoButtonAction);

    // Toolbar action
    connect(undoButtonAction->button, SIGNAL(clicked()), this, SLOT(undoButtonPressed()));
    connect(redoButtonAction->button, SIGNAL(clicked()), this, SLOT(redoButtonPressed()));
    connect(cutButtonAction->button, SIGNAL(clicked()), this, SLOT(cutButtonPressed()));
    connect(copyButtonAction->button, SIGNAL(clicked()), this, SLOT(copyButtonPressed()));
    connect(pasteButtonAction->button, SIGNAL(clicked()), this, SLOT(pasteButtonPressed()));
    connect(boldButtonAction->button, SIGNAL(clicked()), this, SLOT(boldButtonPressed()));
    connect(italicsButtonAction->button, SIGNAL(clicked()), this, SLOT(italicsButtonPressed()));
    connect(underlineButtonAction->button, SIGNAL(clicked()), this, SLOT(underlineButtonPressed()));
    connect(leftAlignButtonAction->button, SIGNAL(clicked()), this, SLOT(alignLeftButtonPressed()));
    connect(rightAlignButtonAction->button, SIGNAL(clicked()), this, SLOT(alignRightButtonPressed()));
    connect(centerAlignButtonAction->button, SIGNAL(clicked()), this, SLOT(alignCenterButtonPressed()));
    connect(strikethroughButtonAction->button, SIGNAL(clicked()), this, SLOT(strikethroughButtonPressed()));
    connect(hlineButtonAction->button, SIGNAL(clicked()), this, SLOT(horizontalLineButtonPressed()));
    connect(shiftRightButtonAction->button, SIGNAL(clicked()), this, SLOT(shiftRightButtonPressed()));
    connect(shiftLeftButtonAction->button, SIGNAL(clicked()), this, SLOT(shiftLeftButtonPressed()));
    connect(bulletListButtonAction->button, SIGNAL(clicked()), this, SLOT(bulletListButtonPressed()));
    connect(numberListButtonAction->button, SIGNAL(clicked()), this, SLOT(numberListButtonPressed()));
    connect(todoButtonAction->button, SIGNAL(clicked()), this, SLOT(todoButtonPressed()));
    connect(fontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(fontSizeSelected(int)));
    connect(fontNames, SIGNAL(currentIndexChanged(int)), this, SLOT(fontNameSelected(int)));
    connect(fontColor->menu(), SIGNAL(triggered(QAction*)), this, SLOT(fontColorClicked()));
    connect(highlightColor->menu(), SIGNAL(triggered(QAction*)), this, SLOT(fontHilightClicked()));
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

    // First, make sure we have a valid lid
    if (lid == -1) {
        clear();
        return;
    }

    // let's load the new note
    this->lid = lid;
    this->editor->isDirty = false;

    NoteTable noteTable;
    Note n;

    noteTable.get(n, this->lid, false, false);
    QByteArray content;

    if (!global.cache.contains(lid)) {
        NoteFormatter formatter;
        formatter.setNote(n, false);
        content = formatter.rebuildNoteHTML();
        NoteCache *newCache = new NoteCache();
        newCache->noteContent = content;
        global.cache.insert(lid, newCache);
    } else {
        NoteCache *c = global.cache[lid];
        content = c->noteContent;
    }


    noteTitle.setTitle(lid, QString::fromStdString(n.title), QString::fromStdString(n.title));
    dateEditor.setNote(lid, n);
    editor->setContent(content);

    // Set the tags
    tagEditor.clear();
    QStringList names;
    for (unsigned int i=0; i<n.tagNames.size(); i++) {
        names << QString::fromStdString(n.tagNames[i]);
    }
    tagEditor.setTags(names);
    tagEditor.setCurrentLid(lid);

    this->lid = lid;
    notebookMenu.setCurrentNotebook(lid, n);
    if (n.__isset.attributes && n.attributes.__isset.sourceURL)
        urlEditor.setUrl(lid, QString::fromStdString(n.attributes.sourceURL));
    else
        urlEditor.setUrl(lid, "");
    setSource();
    editor->page()->setContentEditable(true);
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
        dateEditor.show();
        break;
    }
}



// Send a signal that the note has been updated
void NBrowserWindow::sendUpdateSignal() {
    emit(this->noteUpdated(lid));
}



// Send a signal that a tag has been added to a note
void NBrowserWindow::newTagAdded(qint32 lid) {
    emit(tagAdded(lid));
}



// Add a tag to a note
void NBrowserWindow::addTagName(qint32 lid) {
    TagTable table;
    Tag t;
    table.get(t, lid);
    tagEditor.addTag(QString::fromStdString(t.name));
}



// Rename a tag in a note.
void NBrowserWindow::tagRenamed(qint32 lid, QString oldName, QString newName) {
    tagEditor.tagRenamed(lid, oldName, newName);
}



// Remove a tag in a note
void NBrowserWindow::tagDeleted(qint32 lid, QString name) {
    lid = lid;  /* suppress unused */
    tagEditor.removeTag(name);
}



// A notebook was renamed
void NBrowserWindow::notebookRenamed(qint32 lid, QString oldName, QString newName) {
    lid = lid;  /* suppress unused */
    oldName = oldName;  /* suppress unused */
    newName = newName;  /* suppress unused */
    notebookMenu.reloadData();
}




// A notebook was deleted
void NBrowserWindow::notebookDeleted(qint32 lid, QString name) {
    lid = lid;  /* suppress unused */
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was renamed
void NBrowserWindow::stackRenamed(QString oldName, QString newName) {
    oldName = oldName;  /* suppress unused */
    newName = newName;  /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was deleted
void NBrowserWindow::stackDeleted(QString name) {
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}



// A stack was added
void NBrowserWindow::stackAdded(QString name) {
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}



// A notebook was added
void NBrowserWindow::notebookAdded(qint32 lid) {
    lid = lid;  /* suppress unused */
    notebookMenu.reloadData();
}


// A note was synchronized with Evernote's servers
void NBrowserWindow::noteSyncUpdate(qint32 lid) {
    if (lid != this->lid)
        return;
    setContent(lid);
}




// A note's content was updated
void NBrowserWindow::noteContentUpdated() {
    if (editor->isDirty) {
        NoteTable noteTable;
        noteTable.setDirty(this->lid, true);
    }
    if (sourceEdit->isVisible()) {
        sourceEditorTimer->stop();
        sourceEditorTimer->setInterval(500);
        sourceEditorTimer->setSingleShot(true);
        sourceEditorTimer->start();
    }
}




// Save the note's content
void NBrowserWindow::saveNoteContent() {
    if (this->editor->isDirty) {
        QString contents = editor->editorPage->mainFrame()->toHtml();
        EnmlFormatter formatter;
        formatter.setHtml(contents);
        formatter.rebuildNoteEnml();
        NoteTable table;
        table.updateNoteContent(lid, formatter.getEnml());

        NoteCache* cache = global.cache[lid];
        if (cache != NULL) {
            QByteArray b;
            b.append(contents);
            cache->noteContent = b;
            global.cache.remove(lid);
            global.cache.insert(lid, cache);
        }
    }
}



// Load the list of font names
void NBrowserWindow::loadFontNames() {
    QFontDatabase fonts;
    QStringList fontFamilies = fonts.families();
    for (int i = 0; i < fontFamilies.size(); i++) {
        fontNames->addItem(fontFamilies[i], fontFamilies[i]);
        if (i == 0) {
            loadFontSizeCombobox(fontFamilies[i]);
        }
    }
}



// Load the list of font sizes
void NBrowserWindow::loadFontSizeCombobox(QString name) {
    QFontDatabase fdb;
    fontSize->clear();
    QList<int> sizes = fdb.pointSizes(name);
    for (int i=0; i<sizes.size(); i++) {
        fontSize->addItem(QString::number(sizes[i]), sizes[i]);
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
    this->editor->triggerPageAction(QWebPage::Paste);
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
    this->sendUpdateSignal();
}


// The todo button was pressed
void NBrowserWindow::todoButtonPressed() {
    QString script_start="document.execCommand('insertHtml', false, '";
    QString script_end = "');";
    QString todo =
            "<input TYPE=\"CHECKBOX\" value=\"false\" " +
            QString("onMouseOver=\"style.cursor=\\'hand\\'\" ") +
            QString("onClick=\"value=checked; window.browserWindow.contentChanged(); \" />");
    editor->page()->mainFrame()->evaluateJavaScript(
            script_start + todo + script_end);
    editor->setFocus();
    microFocusChanged();
}



// The font size button was pressed
void NBrowserWindow::fontSizeSelected(int index) {
    int size = fontSize->itemData(index).toInt();

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



// The font name list was selected
void NBrowserWindow::fontNameSelected(int index) {
    QString font = fontNames->itemData(index).toString();
    loadFontSizeCombobox(font);
    this->editor->page()->mainFrame()->evaluateJavaScript(
            "document.execCommand('fontName', false, '"+font+"');");
    editor->setFocus();
    microFocusChanged();
}



// The font highlight color was pressed
void NBrowserWindow::fontHilightClicked() {
    QColor *color = highlightColorMenu.getColor();
    if (color->isValid()) {
        this->editor->page()->mainFrame()->evaluateJavaScript(
                "document.execCommand('backColor', false, '"+color->name()+"');");
        editor->setFocus();
        microFocusChanged();
    }
}



// The font color was pressed
void NBrowserWindow::fontColorClicked() {
    QColor *color = fontColorMenu.getColor();
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


void NBrowserWindow::insertQuickLinkButtonPressed() {

}


void NBrowserWindow::insertLatexButtonPressed() {

}


void NBrowserWindow::encryptButtonPressed() {

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
    QString newhash =  b.toHex();
    updateImageHash(newhash, b);

    // Reload the web page
    editor->triggerPageAction(QWebPage::ReloadAndBypassCache);
    contentChanged();
}


void NBrowserWindow::updateImageHash(QString newhash) {
    QString content = editor->page()->mainFrame()->toHtml();
    int pos = content.indexOf("<img ");
    for (; pos>0; pos=content.indexOf("<img ", pos+1) ) {
        int endPos = content.indexOf(">", pos);
        QString section = content.mid(pos, endPos-pos);
        if (section.indexOf("lid=\"" +QString::number(selectedFileLid) + "\"") > 0) {
            ResourceTable rtable;
            QString oldhash = section.mid(section.indexOf("hash=\"")+6);
            oldhash = oldhash.mid(0,oldhash.indexOf("\""));
            section.replace(oldhash, newhash);
            QString newcontent = content.mid(0,pos) +section +content.mid(endPos);
            QLOG_DEBUG() << "***\n\nOLD\n\n***\n" << content;
            QLOG_DEBUG() << "***\n\nNEW\n\n***\n" << newcontent;
            editor->page()->mainFrame()->setHtml(newcontent);
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
    selectedFileName = f;
    selectedFileLid = l.toInt();
}




//****************************************************************
//* MicroFocus changed
//****************************************************************
 void NBrowserWindow::microFocusChanged() {
     boldButtonAction->button->setDown(false);
     italicsButtonAction->button->setDown(false);
     underlineButtonAction->button->setDown(false);
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
     editor->insertQuickLinkAction->setEnabled(false);
     editor->rotateImageRightAction->setEnabled(false);
     editor->rotateImageLeftAction->setEnabled(false);

     insertHyperlink = true;
     currentHyperlink ="";
     insideList = false;
     insideTable = false;
     insideEncryption = false;
     forceTextPaste = false;

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
                 "   window.jambi.setTableCursorPositionTab(rowCount, colCount, tableRows, tableColumns);"
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
                 "   window.jambi.setTableCursorPositionBackTab(rowCount, colCount, tableRows, tableColumns);"
                 "} getCursorPosition();";
         editor->page()->mainFrame()->evaluateJavaScript(js);
     }
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
     if (url.toString().startsWith("latex", Qt::CaseInsensitive)) {
         int position = url.toString().lastIndexOf(".");
         QString guid = url.toString().mid(0,position);
         position = guid.lastIndexOf("/");
         guid = guid.right(position+1);
         editLatex(guid);
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
         NoteTable ntable;
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
             type = fullName.mid(index+1);
             guid = fullName.mid(0,index);
         }
         index = guid.indexOf(global.attachmentNameDelimeter);
         if (index > -1) {
             guid = guid.mid(0,index);
         }


         QUrl shortUrl = url.toString().mid(6);
         QUrl longUrl = QString("file://") +global.fileManager.getTmpDirPath()+url.toString().mid(6);
         QString fileUrl = global.fileManager.getDbaDirPath() +guid + QString(".") +type;
         // If we can't open it, then prompt the user to save it.
         QDesktopServices::openUrl(fileUrl);
         return;
     }
     QDesktopServices::openUrl(url);
 }



 // show/hide view source window
void NBrowserWindow::showSource(bool value) {
     setSource();
     sourceEdit->setVisible(value);
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
}



// Set the source for the "show source" button
void NBrowserWindow::setSource() {
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
    sourceEdit->setReadOnly(true);
 //   sourceEdit.setReadOnly(!getBrowser().page().isContentEditable());
    sourceEdit->blockSignals(false);
}



// Expose the programs to the javascript process
void NBrowserWindow::exposeToJavascript() {
    editor->page()->mainFrame()->addToJavaScriptWindowObject("browserWindow", this);
}



// If we are within bold text, set the bold button active
void NBrowserWindow::boldActive() {
    boldButtonAction->button->setDown(true);
}



// If we are within italics text, make the text button active
void NBrowserWindow::italicsActive() {
    italicsButtonAction->button->setDown(true);
}



// If we are within encrypted text, make sure we force a paste text
void NBrowserWindow::insideEncryptionArea() {
    insideEncryption = true;
    forceTextPaste = true;
}



// If we are within underlined text, make the button active
void NBrowserWindow::underlineActive() {
    underlineButtonAction->button->setDown(true);
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
    currentHyperlink = link;
    insertHyperlink = false;
}




// Edit a latex formula
void NBrowserWindow::editLatex(QString guid) {

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


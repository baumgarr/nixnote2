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




#include "editorbuttonbar.h"
#include <QMenu>
#include <QContextMenuEvent>
#include "global.h"
#include <QFontDatabase>
#include <QPushButton>
#include <QLineEdit>

extern Global global;

EditorButtonBar::EditorButtonBar(QWidget *parent) :
    QToolBar(parent)
{
    contextMenu = new QMenu();
    undoVisible = contextMenu->addAction(tr("Undo"));
    redoVisible = contextMenu->addAction(tr("Redo"));
    cutVisible  = contextMenu->addAction(tr("Cut"));
    copyVisible = contextMenu->addAction(tr("Copy"));
    pasteVisible = contextMenu->addAction(tr("Paste"));
    removeFormatVisible = contextMenu->addAction(tr("Remove Formatting"));
    boldVisible = contextMenu->addAction(tr("Bold"));
    italicVisible = contextMenu->addAction(tr("Italics"));
    superscriptVisible = contextMenu->addAction(tr("Superscript"));
    subscriptVisible = contextMenu->addAction(tr("Subscript"));
    underlineVisible = contextMenu->addAction(tr("Underline"));
    strikethroughVisible = contextMenu->addAction(tr("Strikethrough"));
    leftJustifyVisible = contextMenu->addAction(tr("Align Left"));
    centerJustifyVisible = contextMenu->addAction(tr("Align Center"));
    fullJustifyVisible = contextMenu->addAction(tr("Align Full"));
    rightJustifyVisible = contextMenu->addAction(tr("Align Right"));
    hlineVisible = contextMenu->addAction(tr("Horizontal Line"));
    insertDatetimeVisible = contextMenu->addAction(tr("Insert Date && Time"));
    shiftRightVisible = contextMenu->addAction(tr("Shift Right"));
    shiftLeftVisible = contextMenu->addAction(tr("Shift Left"));
    bulletListVisible = contextMenu->addAction(tr("Bullet List"));
    numberListVisible = contextMenu->addAction(tr("Number List"));
    fontVisible = contextMenu->addAction(tr("Font"));
    fontSizeVisible = contextMenu->addAction(tr("Font Size"));
    fontColorVisible = contextMenu->addAction(tr("Font Color"));
    highlightVisible = contextMenu->addAction(tr("Highlight"));
    todoVisible = contextMenu->addAction(tr("To-do"));
    spellCheckButtonVisible = contextMenu->addAction(tr("Spell Check"));
    insertTableButtonVisible = contextMenu->addAction(tr("Insert Table"));
    htmlEntitiesButtonVisible = contextMenu->addAction(tr("HTML Entities"));
    formatCodeButtonVisible = contextMenu->addAction(tr("Format Code Block"));

    undoVisible->setCheckable(true);
    redoVisible->setCheckable(true);
    cutVisible->setCheckable(true);
    copyVisible->setCheckable(true);
    pasteVisible->setCheckable(true);
    removeFormatVisible->setCheckable(true);
    boldVisible->setCheckable(true);
    italicVisible->setCheckable(true);
    underlineVisible->setCheckable(true);
    strikethroughVisible->setCheckable(true);
    superscriptVisible->setCheckable(true);
    subscriptVisible->setCheckable(true);
    leftJustifyVisible->setCheckable(true);
    centerJustifyVisible->setCheckable(true);
    fullJustifyVisible->setCheckable(true);
    rightJustifyVisible->setCheckable(true);
    hlineVisible->setCheckable(true);
    shiftRightVisible->setCheckable(true);
    shiftLeftVisible->setCheckable(true);
    bulletListVisible->setCheckable(true);
    numberListVisible->setCheckable(true);
    todoVisible->setCheckable(true);
    fontColorVisible->setCheckable(true);
    highlightVisible->setCheckable(true);
    fontColorVisible->setCheckable(true);
    fontSizeVisible->setCheckable(true);
    fontVisible->setCheckable(true);
    spellCheckButtonVisible->setCheckable(true);
    spellCheckButtonVisible->setChecked(true);
    insertTableButtonVisible->setCheckable(true);
    htmlEntitiesButtonVisible->setCheckable(true);
    insertDatetimeVisible->setCheckable(true);
    formatCodeButtonVisible->setCheckable(true);

    connect(undoVisible, SIGNAL(triggered()), this, SLOT(toggleUndoButtonVisible()));
    connect(redoVisible, SIGNAL(triggered()), this, SLOT(toggleRedoButtonVisible()));
    connect(cutVisible, SIGNAL(triggered()), this, SLOT(toggleCutButtonVisible()));
    connect(copyVisible, SIGNAL(triggered()), this, SLOT(toggleCopyButtonVisible()));
    connect(pasteVisible, SIGNAL(triggered()), this, SLOT(togglePasteButtonVisible()));
    connect(removeFormatVisible, SIGNAL(triggered()), this, SLOT(toggleRemoveFormatVisible()));
    connect(boldVisible, SIGNAL(triggered()), this, SLOT(toggleBoldButtonVisible()));
    connect(italicVisible, SIGNAL(triggered()), this, SLOT(toggleItalicButtonVisible()));
    connect(underlineVisible, SIGNAL(triggered()), this, SLOT(toggleUnderlineButtonVisible()));
    connect(strikethroughVisible, SIGNAL(triggered()), this, SLOT(toggleStrikethroughButtonVisible()));
    connect(superscriptVisible, SIGNAL(triggered()), this, SLOT(toggleSuperscriptButtonVisible()));
    connect(subscriptVisible, SIGNAL(triggered()), this, SLOT(toggleSubscriptButtonVisible()));
    connect(insertDatetimeVisible, SIGNAL(triggered()), this, SLOT(toggleInsertDatetimeVisible()));
    connect(leftJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleLeftJustifyButtonVisible()));
    connect(centerJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleCenterJustifyButtonVisible()));
    connect(fullJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleFullJustifyButtonVisible()));
    connect(rightJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleRightJustifyButtonVisible()));
    connect(hlineVisible, SIGNAL(triggered()), this, SLOT(toggleHlineButtonVisible()));
    connect(shiftRightVisible, SIGNAL(triggered()), this, SLOT(toggleShiftRightButtonVisible()));
    connect(shiftLeftVisible, SIGNAL(triggered()), this, SLOT(toggleShiftLeftButtonVisible()));
    connect(bulletListVisible, SIGNAL(triggered()), this, SLOT(toggleBulletListButtonVisible()));
    connect(numberListVisible, SIGNAL(triggered()), this, SLOT(toggleNumberListButtonVisible()));
    connect(fontVisible, SIGNAL(triggered()), this, SLOT(toggleFontButtonVisible()));
    connect(fontSizeVisible, SIGNAL(triggered()), this, SLOT(toggleFontSizeButtonVisible()));
    connect(todoVisible, SIGNAL(triggered()), this, SLOT(toggleTodoButtonVisible()));
    connect(highlightVisible, SIGNAL(triggered()), this, SLOT(toggleHighlightColorVisible()));
    connect(fontColorVisible, SIGNAL(triggered()), this, SLOT(toggleFontColorVisible()));
    connect(insertTableButtonVisible, SIGNAL(triggered()), this, SLOT(toggleInsertTableButtonVisible()));
    connect(spellCheckButtonVisible, SIGNAL(triggered()), this, SLOT(toggleSpellCheckButtonVisible()));
    connect(htmlEntitiesButtonVisible, SIGNAL(triggered()), this, SLOT(toggleHtmlEntitiesButtonVisible()));
    connect(formatCodeButtonVisible, SIGNAL(triggered()), this, SLOT(toggleFormatCodeButtonVisible()));


  undoButtonAction = this->addAction(global.getIconResource(":undoIcon"), tr("Undo"));
  undoButtonShortcut = new QShortcut(this);
  setupShortcut(undoButtonShortcut, "Edit_Undo");

  redoButtonAction = this->addAction(global.getIconResource(":redoIcon"), tr("Redo"));
  redoButtonShortcut = new QShortcut(this);
  setupShortcut(redoButtonShortcut, "Edit_Redo");
  redoButtonShortcut->setContext(Qt::WidgetShortcut);

  cutButtonAction = this->addAction(global.getIconResource(":cutIcon"), tr("Cut"));
  cutButtonShortcut = new QShortcut(this);
  setupShortcut(cutButtonShortcut, "Edit_Cut");

  copyButtonAction = this->addAction(global.getIconResource(":copyIcon"), tr("Copy"));
  copyButtonShortcut = new QShortcut(this);
  setupShortcut(copyButtonShortcut, "Edit_Copy");

  pasteButtonAction = this->addAction(global.getIconResource(":pasteIcon"), tr("Paste"));
  //this->setupShortcut(pasteButtonAction, "Edit_Paste");  // This is captured in NWebView via a keyevent statement

  removeFormatButtonAction = this->addAction(global.getIconResource(":eraserIcon"), tr("Remove Formatting"));
  removeFormatButtonShortcut = new QShortcut(this);
  this->setupShortcut(removeFormatButtonShortcut, "Edit_Remove_Formatting");

  boldButtonWidget = new QToolButton(this);
  boldButtonWidget->setIcon(global.getIconResource(":boldIcon"));
  boldButtonWidget->setText(tr("Bold"));
  boldButtonAction = this->addWidget(boldButtonWidget);
  boldButtonShortcut = new QShortcut(this);
  this->setupShortcut(boldButtonShortcut, "Format_Bold");

  italicButtonWidget = new QToolButton(this);
  italicButtonWidget->setIcon(global.getIconResource(":italicsIcon"));
  italicButtonWidget->setText(tr("Italics"));
  italicButtonWidget->setToolTip(tr("Italics"));
  italicButtonAction = this->addWidget(italicButtonWidget);
  italicButtonShortcut = new QShortcut(this);
  this->setupShortcut(italicButtonShortcut, "Format_Italic");

  underlineButtonWidget = new QToolButton(this);
  underlineButtonWidget->setIcon(global.getIconResource(":underlineIcon"));
  underlineButtonWidget->setText(tr("Underline"));
  underlineButtonWidget->setToolTip(tr("Underline"));
  underlineButtonAction = this->addWidget(underlineButtonWidget);
  underlineButtonShortcut = new QShortcut(this);
  this->setupShortcut(underlineButtonShortcut, "Format_Underline");

  strikethroughButtonAction = this->addAction(global.getIconResource(":strikethroughIcon"), tr("Strikethrough"));
  strikethroughButtonShortcut = new QShortcut(this);
  this->setupShortcut(strikethroughButtonShortcut, "Format_Strikethrough");

  superscriptButtonAction = this->addAction(global.getIconResource(":superscriptIcon"), tr("Superscript"));
  superscriptButtonShortcut = new QShortcut(this);
  this->setupShortcut(superscriptButtonShortcut, "Format_Superscript");

  subscriptButtonAction = this->addAction(global.getIconResource(":subscriptIcon"), tr("Subscript"));
  subscriptButtonShortcut = new QShortcut(this);
  this->setupShortcut(subscriptButtonShortcut, "Format_Subscript");

  centerJustifyButtonAction = this->addAction(global.getIconResource(":centerAlignIcon"), tr("Center Justify"));
  centerJustifyButtonShortcut = new QShortcut(this);
  this->setupShortcut(centerJustifyButtonShortcut, "Format_Alignment_Center");

  fullJustifyButtonAction = this->addAction(global.getIconResource(":fullAlignIcon"), tr("Fully Justify"));
  fullJustifyButtonShortcut = new QShortcut(this);
  this->setupShortcut(fullJustifyButtonShortcut, "Format_Alignment_Full");

  rightJustifyButtonAction = this->addAction(global.getIconResource(":rightAlignIcon"), tr("Right Justify"));
  rightJustifyButtonShortcut = new QShortcut(this);
  this->setupShortcut(rightJustifyButtonShortcut, "Format_Alignment_Right");

  leftJustifyButtonAction = this->addAction(global.getIconResource(":leftAlignIcon"), tr("Left Justify"));
  leftJustifyButtonShortcut = new QShortcut(this);
  this->setupShortcut(leftJustifyButtonShortcut, "Format_Alignment_Left");

  hlineButtonAction = this->addAction(global.getIconResource(":hlineIcon"), tr("Horizontal Line"));
  hlineButtonShortcut = new QShortcut(this);
  this->setupShortcut(hlineButtonShortcut, "Format_Horizontal_Line");

  insertDatetimeButtonWidget = new QToolButton(this);
  insertDatetimeButtonWidget->setIcon(global.getIconResource(":dateTime"));
  insertDatetimeButtonWidget->setText(tr("Insert Date & Time"));
  insertDatetimeButtonWidget->setToolTip(tr("Insert Date & Time"));
  insertDatetimeButtonAction = this->addWidget(insertDatetimeButtonWidget);
  insertDatetimeButtonShortcut = new QShortcut(this);
  insertDatetimeButtonShortcut->setKey(insertDatetimeButtonWidget->shortcut());

  shiftRightButtonAction = this->addAction(global.getIconResource(":shiftRightIcon"), tr("Shift Right"));
  shiftRightButtonShortcut = new QShortcut(this);
  this->setupShortcut(shiftRightButtonShortcut, "Format_Indent_Increase");

  shiftLeftButtonAction = this->addAction(global.getIconResource(":shiftLeftIcon"), tr("Shift Left"));
  shiftLeftButtonShortcut = new QShortcut(this);
  this->setupShortcut(shiftLeftButtonShortcut, "Format_Indent_Decrease");

  bulletListButtonAction = this->addAction(global.getIconResource(":bulletListIcon"), tr("Bullet List"));
  bulletListButtonShortcut = new QShortcut(this);
  this->setupShortcut(bulletListButtonShortcut, "Format_List_Bullet");

  numberListButtonAction = this->addAction(global.getIconResource(":numberListIcon"), tr("Number List"));
  numberListButtonShortcut = new QShortcut(this);
  this->setupShortcut(numberListButtonShortcut, "Format_List_Numbered");

  fontNames = new FontNameComboBox(this);
  fontSizes = new FontSizeComboBox(this);

  loadFontNames();
  fontButtonAction = addWidget(fontNames);
  fontSizeButtonAction = addWidget(fontSizes);

  fontColorMenuWidget = new ColorMenu();
  fontColorButtonWidget = new QToolButton(this);
  fontColorButtonWidget->setAutoRaise(false);
  fontColorButtonWidget->setMenu(fontColorMenuWidget->getMenu());
  fontColorButtonWidget->setIcon(global.getIconResource(":fontColorIcon"));
  fontColorButtonWidget->setToolTip(tr("Font Color"));
  fontColorAction = this->addWidget(fontColorButtonWidget);

  highlightColorMenuWidget = new ColorMenu();
  highlightColorMenuWidget->setDefault(Qt::yellow);
  highlightColorButtonWidget = new QToolButton(this);
  highlightColorButtonWidget->setAutoRaise(false);
  highlightColorButtonWidget->setMenu(highlightColorMenuWidget->getMenu());
  highlightColorButtonWidget->setIcon(global.getIconResource(":fontHighlightIcon"));
  highlightColorButtonWidget->setToolTip(tr("Highlight"));
  highlightColorAction = this->addWidget(highlightColorButtonWidget);
  highlightColorShortcut = new QShortcut(this);
  this->setupShortcut(highlightColorShortcut, "Format_Highlight");

  todoButtonAction = this->addAction(global.getIconResource(":todoIcon"), tr("Todo"));
  todoButtonShortcut = new QShortcut(this);
  this->setupShortcut(todoButtonShortcut, "Edit_Insert_Todo");

  spellCheckButtonAction = this->addAction(global.getIconResource(":spellCheckIcon"), tr("Spell Check"));
  spellCheckButtonShortcut = new QShortcut(this);
  this->setupShortcut(spellCheckButtonShortcut, "Tools_Spell_Check");

  insertTableButtonAction = this->addAction(global.getIconResource(":gridIcon"), tr("Insert Table"));
  insertTableButtonShortcut = new QShortcut(this);
  this->setupShortcut(insertTableButtonShortcut, "Edit_Insert_Table");

  htmlEntitiesButtonAction = this->addAction(global.getIconResource(":htmlentitiesIcon"), tr("Insert HTML Entities"));
  htmlEntitiesButtonShortcut = new QShortcut(this);
  setupShortcut(htmlEntitiesButtonShortcut, "Edit_Insert_Html_Entities");
  htmlEntitiesButtonShortcut->setContext(Qt::WidgetShortcut);


  formatCodeButtonAction = this->addAction(global.getIconResource(":formatCodeIcon"), tr("Format Code Block"));
  formatCodeButtonShortcut = new QShortcut(this);
  this->setupShortcut(formatCodeButtonShortcut, "Format_Code_Block");

  QString css = global.getThemeCss("editorButtonBarCss");
  if (css!="")
      this->setStyleSheet(css);


}


EditorButtonBar::~EditorButtonBar() {
    delete undoVisible;
    delete redoVisible;
    delete cutVisible;
    delete copyVisible;
    delete pasteVisible;
    delete boldVisible;
    delete italicVisible;
    delete underlineVisible;
    delete strikethroughVisible;
    delete leftJustifyVisible;
    delete centerJustifyVisible;
    delete formatCodeButtonVisible;
    delete fullJustifyVisible;
    delete rightJustifyVisible;
    delete hlineVisible;
    delete shiftRightVisible;
    delete shiftLeftVisible;
    delete bulletListVisible;
    delete numberListVisible;
    delete fontVisible;
    delete fontSizeVisible;
    delete todoVisible;
}


void EditorButtonBar::contextMenuEvent(QContextMenuEvent *event) {
    contextMenu->exec(event->globalPos());
}


void EditorButtonBar::saveVisibleButtons() {
    global.settings->beginGroup("SaveState");

    bool value;
    value = undoButtonAction->isVisible();
    global.settings->setValue("undoButtonVisible", value);

    value = redoButtonAction->isVisible();
    global.settings->setValue("redoButtonVisible", value);

    value = cutButtonAction->isVisible();
    global.settings->setValue("cutButtonVisible", value);

    value = copyButtonAction->isVisible();
    global.settings->setValue("copyButtonVisible", value);

    value = pasteButtonAction->isVisible();
    global.settings->setValue("pasteButtonVisible", value);

    value = removeFormatButtonAction->isVisible();
    global.settings->setValue("removeFormatButtonVisible", value);

    value = boldButtonAction->isVisible();
    global.settings->setValue("boldButtonVisible", value);

    value = italicButtonAction->isVisible();
    global.settings->setValue("italicButtonVisible", value);

    value = hlineButtonAction->isVisible();
    global.settings->setValue("hlineButtonVisible", value);

    value = underlineButtonAction->isVisible();
    global.settings->setValue("underlineButtonVisible", value);

    value = strikethroughButtonAction->isVisible();
    global.settings->setValue("strikethroughButtonVisible", value);

    value = superscriptButtonAction->isVisible();
    global.settings->setValue("superscriptButtonVisible", value);

    value = subscriptButtonAction->isVisible();
    global.settings->setValue("subscriptButtonVisible", value);

    value = leftJustifyButtonAction->isVisible();
    global.settings->setValue("leftJustifyButtonVisible", value);

    value = rightJustifyButtonAction->isVisible();
    global.settings->setValue("rightJustifyButtonVisible", value);

    value = centerJustifyButtonAction->isVisible();
    global.settings->setValue("centerJustifyButtonVisible", value);

    value = fullJustifyButtonAction->isVisible();
    global.settings->setValue("fullJustifyButtonVisible", value);

    value = shiftLeftButtonAction->isVisible();
    global.settings->setValue("shiftLeftButtonVisible", value);

    value = shiftRightButtonAction->isVisible();
    global.settings->setValue("shiftRightButtonVisible", value);

    value = bulletListButtonAction->isVisible();
    global.settings->setValue("bulletListButtonVisible", value);

    value = numberListButtonAction->isVisible();
    global.settings->setValue("numberListButtonVisible", value);

    value = fontButtonAction->isVisible();
    global.settings->setValue("fontButtonVisible", value);

    value = fontSizeButtonAction->isVisible();
    global.settings->setValue("fontSizeButtonVisible", value);

    value = highlightColorAction->isVisible();
    global.settings->setValue("highlightButtonVisible", value);

    value = fontColorAction->isVisible();
    global.settings->setValue("fontColorButtonVisible", value);

    value = todoButtonAction->isVisible();
    global.settings->setValue("todoButtonVisible", value);

    value = insertTableButtonAction->isVisible();
    global.settings->setValue("insertTableButtonVisible", value);

    value = spellCheckButtonAction->isVisible();
    global.settings->setValue("spellCheckButtonVisible", value);

    value = htmlEntitiesButtonAction->isVisible();
    global.settings->setValue("htmlEntitiesButtonVisible", value);

    value = insertDatetimeButtonAction->isVisible();
    global.settings->setValue("insertDatetimeButtonVisible", value);

    value = formatCodeButtonAction->isVisible();
    global.settings->setValue("formatCodeButtonVisible", value);

    global.settings->endGroup();
}


void EditorButtonBar::setupVisibleButtons() {
    global.settings->beginGroup("SaveState");

    undoButtonAction->setVisible(global.settings->value("undoButtonVisible", true).toBool());
    undoVisible->setChecked(undoButtonAction->isVisible());

    redoButtonAction->setVisible(global.settings->value("redoButtonVisible", true).toBool());
    redoVisible->setChecked(redoButtonAction->isVisible());

    cutButtonAction->setVisible(global.settings->value("cutButtonVisible", true).toBool());
    cutVisible->setChecked(cutButtonAction->isVisible());

    copyButtonAction->setVisible(global.settings->value("copyButtonVisible", true).toBool());
    copyVisible->setChecked(copyButtonAction->isVisible());

    pasteButtonAction->setVisible(global.settings->value("pasteButtonVisible", true).toBool());
    pasteVisible->setChecked(pasteButtonAction->isVisible());

    removeFormatButtonAction->setVisible(global.settings->value("removeFormatButtonVisible", true).toBool());
    removeFormatVisible->setChecked(removeFormatButtonAction->isVisible());

    boldButtonAction->setVisible(global.settings->value("boldButtonVisible", true).toBool());
    boldVisible->setChecked(boldButtonAction->isVisible());

    italicButtonAction->setVisible(global.settings->value("italicButtonVisible", true).toBool());
    italicVisible->setChecked(italicButtonAction->isVisible());

    underlineButtonAction->setVisible(global.settings->value("underlineButtonVisible", true).toBool());
    underlineVisible->setChecked(underlineButtonAction->isVisible());

    strikethroughButtonAction->setVisible(global.settings->value("strikethroughButtonVisible", true).toBool());
    strikethroughVisible->setChecked(strikethroughButtonAction->isVisible());

    superscriptButtonAction->setVisible(global.settings->value("superscriptButtonVisible", true).toBool());
    superscriptVisible->setChecked(superscriptButtonAction->isVisible());

    subscriptButtonAction->setVisible(global.settings->value("subscriptButtonVisible", true).toBool());
    subscriptVisible->setChecked(subscriptButtonAction->isVisible());

    hlineButtonAction->setVisible(global.settings->value("hlineButtonVisible", true).toBool());
    hlineVisible->setChecked(hlineButtonAction->isVisible());

    leftJustifyButtonAction->setVisible(global.settings->value("leftJustifyButtonVisible", true).toBool());
    leftJustifyVisible->setChecked(leftJustifyButtonAction->isVisible());

    centerJustifyButtonAction->setVisible(global.settings->value("centerJustifyButtonVisible", true).toBool());
    centerJustifyVisible->setChecked(centerJustifyButtonAction->isVisible());

    fullJustifyButtonAction->setVisible(global.settings->value("fullJustifyButtonVisible", true).toBool());
    fullJustifyVisible->setChecked(fullJustifyButtonAction->isVisible());

    rightJustifyButtonAction->setVisible(global.settings->value("rightJustifyButtonVisible", true).toBool());
    rightJustifyVisible->setChecked(rightJustifyButtonAction->isVisible());

    shiftLeftButtonAction->setVisible(global.settings->value("shiftLeftButtonVisible", true).toBool());
    shiftLeftVisible->setChecked(shiftLeftButtonAction->isVisible());

    shiftRightButtonAction->setVisible(global.settings->value("shiftRightButtonVisible", true).toBool());
    shiftRightVisible->setChecked(shiftRightButtonAction->isVisible());

    bulletListButtonAction->setVisible(global.settings->value("bulletListButtonVisible", true).toBool());
    bulletListVisible->setChecked(bulletListButtonAction->isVisible());

    numberListButtonAction->setVisible(global.settings->value("numberListButtonVisible", true).toBool());
    numberListVisible->setChecked(numberListButtonAction->isVisible());

    fontButtonAction->setVisible(global.settings->value("fontButtonVisible", true).toBool());
    fontVisible->setChecked(fontButtonAction->isVisible());

    fontSizeButtonAction->setVisible(global.settings->value("fontSizeButtonVisible", true).toBool());
    fontSizeVisible->setChecked(fontSizeButtonAction->isVisible());

    todoButtonAction->setVisible(global.settings->value("todoButtonVisible", true).toBool());
    todoVisible->setChecked(todoButtonAction->isVisible());

    insertTableButtonAction->setVisible(global.settings->value("insertTableButtonVisible", true).toBool());
    insertTableButtonVisible->setChecked(insertTableButtonAction->isVisible());

    fontColorAction ->setVisible(global.settings->value("fontColorButtonVisible", true).toBool());
    fontColorVisible->setChecked(fontColorAction->isVisible());

    highlightColorAction->setVisible(global.settings->value("highlightButtonVisible", true).toBool());
    highlightVisible->setChecked(highlightColorAction->isVisible());

    spellCheckButtonAction->setVisible(global.settings->value("spelLCheckButtonVisible", true).toBool());
    spellCheckButtonVisible->setChecked(spellCheckButtonAction->isVisible());

    htmlEntitiesButtonAction->setVisible(global.settings->value("htmlEntitiesButtonVisible", true).toBool());
    htmlEntitiesButtonVisible->setChecked(htmlEntitiesButtonAction->isVisible());

    insertDatetimeButtonAction->setVisible(global.settings->value("insertDatetimeButtonVisible", true).toBool());
    insertDatetimeVisible->setChecked(insertDatetimeButtonAction->isVisible());

    formatCodeButtonAction->setVisible(global.settings->value("formatCodeButtonVisible", true).toBool());
    formatCodeButtonVisible->setChecked(formatCodeButtonAction->isVisible());

    global.settings->endGroup();
}



void EditorButtonBar::toggleUndoButtonVisible() {
    undoButtonAction->setVisible(undoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleRedoButtonVisible()  {
    redoButtonAction->setVisible(redoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCutButtonVisible() {
    cutButtonAction->setVisible(cutVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCopyButtonVisible() {
    copyButtonAction->setVisible(copyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::togglePasteButtonVisible() {
    pasteButtonAction->setVisible(pasteVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleRemoveFormatVisible() {
    removeFormatButtonAction->setVisible(removeFormatVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleBoldButtonVisible() {
    boldButtonAction->setVisible(boldVisible->isChecked());
    saveVisibleButtons();
    }
void EditorButtonBar::toggleItalicButtonVisible() {
    italicButtonAction->setVisible(italicVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleUnderlineButtonVisible() {
    underlineButtonAction->setVisible(underlineVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleStrikethroughButtonVisible() {
    strikethroughButtonAction->setVisible(strikethroughVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleSuperscriptButtonVisible() {
    superscriptButtonAction->setVisible(superscriptVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleSubscriptButtonVisible() {
    subscriptButtonAction->setVisible(subscriptVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleInsertDatetimeVisible() {
    insertDatetimeButtonAction->setVisible(insertDatetimeVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleLeftJustifyButtonVisible() {
    leftJustifyButtonAction->setVisible(leftJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCenterJustifyButtonVisible() {
    centerJustifyButtonAction->setVisible(centerJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFullJustifyButtonVisible() {
    fullJustifyButtonAction->setVisible(fullJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleRightJustifyButtonVisible() {
    rightJustifyButtonAction->setVisible(rightJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleHlineButtonVisible() {
    hlineButtonAction->setVisible(hlineVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleShiftRightButtonVisible() {
    shiftRightButtonAction->setVisible(shiftRightVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleShiftLeftButtonVisible() {
    shiftLeftButtonAction->setVisible(shiftLeftVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleBulletListButtonVisible() {
    bulletListButtonAction->setVisible(bulletListVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleNumberListButtonVisible() {
    numberListButtonAction->setVisible(numberListVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontButtonVisible() {
    fontButtonAction->setVisible(fontVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontSizeButtonVisible() {
    fontSizeButtonAction->setVisible(fontSizeVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleTodoButtonVisible() {
    todoButtonAction->setVisible(todoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleHighlightColorVisible() {
    highlightColorAction->setVisible(highlightVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontColorVisible() {
    fontColorAction->setVisible(fontColorVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleInsertTableButtonVisible() {
    insertTableButtonAction->setVisible(insertTableButtonVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleSpellCheckButtonVisible() {
    spellCheckButtonAction->setVisible(spellCheckButtonVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleHtmlEntitiesButtonVisible() {
    htmlEntitiesButtonAction->setVisible(htmlEntitiesButtonVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFormatCodeButtonVisible() {
    formatCodeButtonAction->setVisible(formatCodeButtonVisible->isChecked());
    saveVisibleButtons();
}


// Load the list of font names
void EditorButtonBar::loadFontNames() {
    if (global.forceWebFonts){
        QStringList fontFamilies;
        fontFamilies.append("Gotham");
        fontFamilies.append("Georgia");
        fontFamilies.append("Helvetica");
        fontFamilies.append("Courier New");
        fontFamilies.append("Times New Roman");
        fontFamilies.append("Times");
        fontFamilies.append("Trebuchet");
        fontFamilies.append("Verdana");
        fontFamilies.sort();
        bool first = true;

        for (int i = 0; i < fontFamilies.size(); i++) {
            fontNames->addItem(fontFamilies[i], fontFamilies[i].toLower());
            QFont f;
            global.getGuiFont(f);
            f.setFamily(fontFamilies[i]);
            fontNames->setItemData(i, QVariant(f), Qt::FontRole);
            if (first) {
                loadFontSizeComboBox(fontFamilies[i]);
                first=false;
            }
        }
        return;
    }

    // Load up the list of font names
    QFontDatabase fonts;
    QStringList fontFamilies = fonts.families();
    fontFamilies.append(tr("Times"));
    fontFamilies.sort();
    bool first = true;

    for (int i = 0; i < fontFamilies.size(); i++) {
        fontNames->addItem(fontFamilies[i], fontFamilies[i].toLower());
        QFont f;
        global.getGuiFont(f);
        f.setFamily(fontFamilies[i]);
        if (global.previewFontsInDialog())
           fontNames->setItemData(i, QVariant(f), Qt::FontRole);
        if (first) {
            loadFontSizeComboBox(fontFamilies[i]);
            first=false;
        }
    }
}




// Load the list of font sizes
void EditorButtonBar::loadFontSizeComboBox(QString name) {
    QFontDatabase fdb;
    fontSizes->clear();
    QList<int> sizes = fdb.smoothSizes(name, "Normal");
    for (int i=0; i<sizes.size(); i++) {
        fontSizes->addItem(QString::number(sizes[i]), sizes[i]);
    }
    if (sizes.size() == 0) {
        fontSizes->addItem("8", 8);
        fontSizes->addItem("9", 9);
        fontSizes->addItem("10", 10);
        fontSizes->addItem("11", 11);
        fontSizes->addItem("12", 12);
        fontSizes->addItem("14", 14);
        fontSizes->addItem("15", 15);
        fontSizes->addItem("16", 16);
        fontSizes->addItem("17", 17);
        fontSizes->addItem("18", 18);
        fontSizes->addItem("19", 19);
        fontSizes->addItem("20", 20);
        fontSizes->addItem("21", 21);
        fontSizes->addItem("22", 22);
    }

}


//void EditorButtonBar::setupShortcut(QAction *action, QString text) {
//    if (!global.shortcutKeys->containsAction(&text))
//        return;
//    QKeySequence key(global.shortcutKeys->getShortcut(&text));
//    action->setShortcut(key);
//}



void EditorButtonBar::setupShortcut(QShortcut *shortcut, QString text) {
    if (!global.shortcutKeys->containsAction(&text))
        return;
    QKeySequence key(global.shortcutKeys->getShortcut(&text));
    shortcut->setKey(key);
}


//void EditorButtonBar::setupShortcut(QToolButton *action, QString text) {
//    if (!global.shortcutKeys->containsAction(&text))
//        return;
//    QKeySequence key(global.shortcutKeys->getShortcut(&text));
//    action->setShortcut(key);
//}


void EditorButtonBar::reloadIcons() {
    undoButtonAction->setIcon(global.getIconResource(":undoIcon"));
    redoButtonAction->setIcon(global.getIconResource(":redoIcon"));
    cutButtonAction->setIcon(global.getIconResource(":cutIcon"));
    copyButtonAction->setIcon(global.getIconResource(":copyIcon"));
    pasteButtonAction->setIcon(global.getIconResource(":pasteIcon"));
    removeFormatButtonAction->setIcon(global.getIconResource(":eraserIcon"));
    boldButtonWidget->setIcon(global.getIconResource(":boldIcon"));
    italicButtonWidget->setIcon(global.getIconResource(":italicsIcon"));
    underlineButtonWidget->setIcon(global.getIconResource(":underlineIcon"));
    strikethroughButtonAction->setIcon(global.getIconResource(":strikethroughIcon"));
    leftJustifyButtonAction->setIcon(global.getIconResource(":leftAlignIcon"));
    rightJustifyButtonAction->setIcon(global.getIconResource(":rightAlignIcon"));
    centerJustifyButtonAction->setIcon(global.getIconResource(":centerAlignIcon"));
    fullJustifyButtonAction->setIcon(global.getIconResource(":fullAlignIcon"));
    hlineButtonAction->setIcon(global.getIconResource(":hlineIcon"));
    shiftRightButtonAction->setIcon(global.getIconResource(":shiftRightIcon"));
    shiftLeftButtonAction->setIcon(global.getIconResource(":shiftLeftIcon"));
    bulletListButtonAction->setIcon(global.getIconResource(":bulletListIcon"));
    numberListButtonAction->setIcon(global.getIconResource(":numberListIcon"));
    fontColorButtonWidget->setIcon(global.getIconResource(":fontColorIcon"));
    highlightColorButtonWidget->setIcon(global.getIconResource(":fontHighlightIcon"));
    todoButtonAction->setIcon(global.getIconResource(":todoIcon"));
    spellCheckButtonAction->setIcon(global.getIconResource(":spellCheckIcon"));
    insertTableButtonAction->setIcon(global.getIconResource(":gridIcon"));
    htmlEntitiesButtonAction->setIcon(global.getIconResource(":htmlentitiesIcon"));
    formatCodeButtonAction->setIcon(global.getIconResource(":formatCodeIcon"));
}

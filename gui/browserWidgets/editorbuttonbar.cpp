#include "editorbuttonbar.h"
#include <QMenu>
#include <QContextMenuEvent>
#include "global.h"

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
    boldVisible = contextMenu->addAction(tr("Bold"));
    italicVisible = contextMenu->addAction(tr("Italics"));
    underlineVisible = contextMenu->addAction(tr("Underline"));
    strikethroughVisible = contextMenu->addAction(tr("Strikethrough"));
    leftJustifyVisible = contextMenu->addAction(tr("Align Left"));
    centerJustifyVisible = contextMenu->addAction(tr("Align Center"));
    rightJustifyVisible = contextMenu->addAction(tr("Align Right"));
    hlineVisible = contextMenu->addAction(tr("Horizontal Line"));
    shiftRightVisible = contextMenu->addAction(tr("Shift Right"));
    shiftLeftVisible = contextMenu->addAction(tr("Shift Left"));
    buttonListVisible = contextMenu->addAction(tr("Bullet List"));
    numberListVisible = contextMenu->addAction(tr("Number List"));
    fontVisible = contextMenu->addAction(tr("Font"));
    fontSizeVisible = contextMenu->addAction(tr("Font Size"));
    fontColorVisible = contextMenu->addAction(tr("Font Color"));
    highlightVisible = contextMenu->addAction(tr("Highlight"));
    todoVisible = contextMenu->addAction(tr("Todo"));

    undoVisible->setCheckable(true);
    redoVisible->setCheckable(true);
    cutVisible->setCheckable(true);
    copyVisible->setCheckable(true);
    pasteVisible->setCheckable(true);
    boldVisible->setCheckable(true);
    italicVisible->setCheckable(true);
    underlineVisible->setCheckable(true);
    strikethroughVisible->setCheckable(true);
    leftJustifyVisible->setCheckable(true);
    centerJustifyVisible->setCheckable(true);
    rightJustifyVisible->setCheckable(true);
    hlineVisible->setCheckable(true);
    shiftRightVisible->setCheckable(true);
    shiftLeftVisible->setCheckable(true);
    buttonListVisible->setCheckable(true);
    numberListVisible->setCheckable(true);
    todoVisible->setCheckable(true);
    fontColorVisible->setCheckable(true);
    highlightVisible->setCheckable(true);
    fontColorVisible->setCheckable(true);
    fontSizeVisible->setCheckable(true);
    fontVisible->setCheckable(true);

    connect(undoVisible, SIGNAL(triggered()), this, SLOT(toggleUndoButtonVisible()));
    connect(redoVisible, SIGNAL(triggered()), this, SLOT(toggleRedoButtonVisible()));
    connect(cutVisible, SIGNAL(triggered()), this, SLOT(toggleCutButtonVisible()));
    connect(copyVisible, SIGNAL(triggered()), this, SLOT(toggleCopyButtonVisible()));
    connect(pasteVisible, SIGNAL(triggered()), this, SLOT(togglePasteButtonVisible()));
    connect(boldVisible, SIGNAL(triggered()), this, SLOT(toggleBoldButtonVisible()));
    connect(italicVisible, SIGNAL(triggered()), this, SLOT(toggleItalicButtonVisible()));
    connect(underlineVisible, SIGNAL(triggered()), this, SLOT(toggleUnderlineButtonVisible()));
    connect(strikethroughVisible, SIGNAL(triggered()), this, SLOT(toggleStrikethroughButtonVisible()));
    connect(leftJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleLeftJustifyButtonVisible()));
    connect(centerJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleCenterJustifyButtonVisible()));
    connect(rightJustifyVisible, SIGNAL(triggered()), this, SLOT(toggleRightJustifyButtonVisible()));
    connect(hlineVisible, SIGNAL(triggered()), this, SLOT(toggleHlineButtonVisible()));
    connect(shiftRightVisible, SIGNAL(triggered()), this, SLOT(toggleShiftRightButtonVisible()));
    connect(shiftLeftVisible, SIGNAL(triggered()), this, SLOT(toggleShiftLeftButtonVisible()));
    connect(buttonListVisible, SIGNAL(triggered()), this, SLOT(toggleButtonListButtonVisible()));
    connect(numberListVisible, SIGNAL(triggered()), this, SLOT(toggleNumberListButtonVisible()));
//    connect(fontVisible, SIGNAL(triggered()), this, SLOT(toggleFontButtonVisible()));
//    connect(fontSizeVisible, SIGNAL(triggered()), this, SLOT(toggleFontSizeButtonVisible()));
  connect(todoVisible, SIGNAL(triggered()), this, SLOT(toggleTodoButtonVisible()));
//    connect(highlightVisible, SIGNAL(triggered()), this, SLOT(toggleHlineButtonVisible()));
//    connect(fontColorVisible, SIGNAL(triggered()), this, SLOT(toggleFontColorVisible()));

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
    delete rightJustifyVisible;
    delete hlineVisible;
    delete shiftRightVisible;
    delete shiftLeftVisible;
    delete buttonListVisible;
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
    value = undoButton->isVisible();
    global.settings->setValue("undoButtonVisible", value);

    value = redoButton->isVisible();
    global.settings->setValue("redoButtonVisible", value);

    value = cutButton->isVisible();
    global.settings->setValue("cutButtonVisible", value);

    value = copyButton->isVisible();
    global.settings->setValue("copyButtonVisible", value);

    value = pasteButton->isVisible();
    global.settings->setValue("pasteButtonVisible", value);

    value = boldButton->isVisible();
    global.settings->setValue("boldButtonVisible", value);

    value = italicButton->isVisible();
    global.settings->setValue("italicButtonVisible", value);

    value = underlineButton->isVisible();
    global.settings->setValue("hlineButtonVisible", value);

    value = underlineButton->isVisible();
    global.settings->setValue("underlineButtonVisible", value);

    value = strikethroughButton->isVisible();
    global.settings->setValue("strikethroughButtonVisible", value);

    value = leftJustifyButton->isVisible();
    global.settings->setValue("leftJustifyButtonVisible", value);

    value = rightJustifyButton->isVisible();
    global.settings->setValue("rightJustifyButtonVisible", value);

    value = centerJustifyButton->isVisible();
    global.settings->setValue("centerJustifyButtonVisible", value);

    value = shiftLeftButton->isVisible();
    global.settings->setValue("shiftLeftButtonVisible", value);

    value = shiftRightButton->isVisible();
    global.settings->setValue("shiftRightButtonVisible", value);

    value = buttonListButton->isVisible();
    global.settings->setValue("bulletListButtonVisible", value);

    value = numberListButton->isVisible();
    global.settings->setValue("numberListButtonVisible", value);

    value = fontButton->isVisible();
    global.settings->setValue("fontButtonVisible", value);

    value = fontSizeButton->isVisible();
    global.settings->setValue("fontSizeButtonVisible", value);

    value = highlightColor->isVisible();
    global.settings->setValue("highlightButtonVisible", value);

    value = fontColor->isVisible();
    global.settings->setValue("fontColorButtonVisible", value);

    value = todoButton->isVisible();
    global.settings->setValue("todoButtonVisible", value);

    global.settings->endGroup();
}


void EditorButtonBar::setupVisibleButtons() {
    global.settings->beginGroup("SaveState");

    undoButton->setVisible(global.settings->value("undoButtonVisible", true).toBool());
    undoVisible->setChecked(undoButton->isVisible());

    redoButton->setVisible(global.settings->value("redoButtonVisible", true).toBool());
    redoVisible->setChecked(redoButton->isVisible());

    cutButton->setVisible(global.settings->value("cutButtonVisible", true).toBool());
    cutVisible->setChecked(cutButton->isVisible());

    copyButton->setVisible(global.settings->value("copyButtonVisible", true).toBool());
    copyVisible->setChecked(copyButton->isVisible());

    pasteButton->setVisible(global.settings->value("pasteButtonVisible", true).toBool());
    pasteVisible->setChecked(pasteButton->isVisible());

    boldButton->setVisible(global.settings->value("boldButtonVisible", true).toBool());
    boldVisible->setChecked(boldButton->isVisible());

    italicButton->setVisible(global.settings->value("italicButtonVisible", true).toBool());
    italicVisible->setChecked(italicButton->isVisible());

    underlineButton->setVisible(global.settings->value("underlineButtonVisible", true).toBool());
    underlineVisible->setChecked(underlineButton->isVisible());

    strikethroughButton->setVisible(global.settings->value("strikethroughButtonVisible", true).toBool());
    strikethroughVisible->setChecked(strikethroughButton->isVisible());

    hlineButton->setVisible(global.settings->value("hlineButtonVisible", true).toBool());
    hlineVisible->setChecked(hlineButton->isVisible());

    leftJustifyButton->setVisible(global.settings->value("leftJustifyButtonVisible", true).toBool());
    leftJustifyVisible->setChecked(leftJustifyButton->isVisible());

    centerJustifyButton->setVisible(global.settings->value("centerJustifyButtonVisible", true).toBool());
    centerJustifyVisible->setChecked(centerJustifyButton->isVisible());

    rightJustifyButton->setVisible(global.settings->value("rightJustifyButtonVisible", true).toBool());
    rightJustifyVisible->setChecked(rightJustifyButton->isVisible());

    shiftLeftButton->setVisible(global.settings->value("shiftLeftButtonVisible", true).toBool());
    shiftLeftVisible->setChecked(shiftLeftButton->isVisible());

    shiftRightButton->setVisible(global.settings->value("shiftRightButtonVisible", true).toBool());
    shiftRightVisible->setChecked(shiftRightButton->isVisible());

    buttonListButton->setVisible(global.settings->value("bulletListButtonVisible", true).toBool());
    buttonListVisible->setChecked(buttonListButton->isVisible());

    numberListButton->setVisible(global.settings->value("numberListButtonVisible", true).toBool());
    numberListVisible->setChecked(buttonListButton->isVisible());

    fontButton->setVisible(global.settings->value("fontButtonVisible", true).toBool());
    fontVisible->setChecked(fontButton->isVisible());

    fontSizeButton->setVisible(global.settings->value("fontSizeButtonVisible", true).toBool());
    fontSizeVisible->setChecked(fontSizeButton->isVisible());

    todoButton->setVisible(global.settings->value("todoButtonVisible", true).toBool());
    todoVisible->setChecked(todoButton->isVisible());

    fontColor->setVisible(global.settings->value("fontColorButtonVisible", true).toBool());
    fontColorVisible->setChecked(fontColor->isVisible());

    highlightColor->setVisible(global.settings->value("highlightButtonVisible", true).toBool());
    highlightVisible->setChecked(highlightColor->isVisible());

    global.settings->endGroup();
}



void EditorButtonBar::toggleUndoButtonVisible() {
    undoButton->setVisible(undoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleRedoButtonVisible()  {
    redoButton->setVisible(redoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCutButtonVisible() {
    cutButton->setVisible(cutVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCopyButtonVisible() {
    copyButton->setVisible(copyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::togglePasteButtonVisible() {
    pasteButton->setVisible(pasteVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleBoldButtonVisible() {
    boldButton->setVisible(boldVisible->isChecked());
    saveVisibleButtons();
    }
void EditorButtonBar::toggleItalicButtonVisible() {
    italicButton->setVisible(italicVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleUnderlineButtonVisible() {
    underlineButton->setVisible(underlineVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleStrikethroughButtonVisible() {
    strikethroughButton->setVisible(strikethroughVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleLeftJustifyButtonVisible() {
    leftJustifyButton->setVisible(leftJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleCenterJustifyButtonVisible() {
    centerJustifyButton->setVisible(centerJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleRightJustifyButtonVisible() {
    rightJustifyButton->setVisible(rightJustifyVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleHlineButtonVisible() {
    hlineButton->setVisible(hlineVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleShiftRightButtonVisible() {
    shiftRightButton->setVisible(shiftRightVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleShiftLeftButtonVisible() {
    shiftLeftButton->setVisible(shiftLeftVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleButtonListButtonVisible() {
    buttonListButton->setVisible(buttonListVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleNumberListButtonVisible() {
    numberListButton->setVisible(numberListVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontButtonVisible() {
    fontButton->setVisible(fontVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontSizeButtonVisible() {
    fontSizeButton->setVisible(fontSizeVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleTodoButtonVisible() {
    todoButton->setVisible(todoVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleHighlightColorVisible() {
    highlightColor->setVisible(highlightVisible->isChecked());
    saveVisibleButtons();
}
void EditorButtonBar::toggleFontColorVisible() {
    fontColor->setVisible(fontColorVisible->isChecked());
    saveVisibleButtons();
}



void EditorButtonBar::addAction(ToolbarWidgetAction *action) {
    QToolBar::addAction(action);
    switch(action->buttonType) {
    case(ToolbarWidgetAction::Undo):
        undoButton = action;
        break;
    case(ToolbarWidgetAction::Redo):
        redoButton = action;
        break;
    case(ToolbarWidgetAction::Cut):
        cutButton = action;
        break;
    case(ToolbarWidgetAction::Copy):
        copyButton = action;
        break;
    case(ToolbarWidgetAction::Paste):
        pasteButton = action;
        break;
    case(ToolbarWidgetAction::Bold):
        boldButton = action;
        break;
    case(ToolbarWidgetAction::Italics):
        italicButton = action;
        break;
    case(ToolbarWidgetAction::Strikethrough):
        strikethroughButton = action;
        break;
    case(ToolbarWidgetAction::Underline):
        underlineButton = action;
        break;
    case(ToolbarWidgetAction::AlignLeft):
        leftJustifyButton = action;
        break;
    case(ToolbarWidgetAction::AlignRight):
        rightJustifyButton = action;
        break;
    case(ToolbarWidgetAction::AlignCenter):
        centerJustifyButton = action;
        break;
    case(ToolbarWidgetAction::HorizontalLine):
        hlineButton = action;
        break;
    case(ToolbarWidgetAction::ShiftRight):
        shiftRightButton = action;
        break;
    case(ToolbarWidgetAction::ShiftLeft):
        shiftLeftButton = action;
        break;
    case(ToolbarWidgetAction::NumberList):
        numberListButton = action;
        break;
    case(ToolbarWidgetAction::BulletList):
        buttonListButton = action;
        break;
    case (ToolbarWidgetAction::Todo):
        todoButton = action;
        break;
    case (ToolbarWidgetAction::SpellCheck) :
        spellButton = action;
        break;
    }
}



void EditorButtonBar::addFontName(QComboBox *fontList) {
    fontButton = addWidget(fontList);
    connect(fontVisible, SIGNAL(triggered()), this, SLOT(toggleFontButtonVisible()));
}

void EditorButtonBar::addFontSize(QComboBox *fontSize) {
    fontSizeButton = addWidget(fontSize);
    connect(fontSizeVisible, SIGNAL(triggered()), this, SLOT(toggleFontSizeButtonVisible()));

}

void EditorButtonBar::addHighlightColor(QToolButton *button) {
    highlightColor= addWidget(button);
    connect(highlightVisible, SIGNAL(triggered()), this, SLOT(toggleHighlightColorVisible()));
}

void EditorButtonBar::addFontColor(QToolButton *button) {
    fontColor = addWidget(button);
    connect(fontColorVisible, SIGNAL(triggered()), this, SLOT(toggleFontColorVisible()));
}

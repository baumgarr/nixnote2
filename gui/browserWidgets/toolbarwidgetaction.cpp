#include "toolbarwidgetaction.h"
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include "gui/nbrowserwindow.h"

ToolbarWidgetAction::ToolbarWidgetAction(QWidget *parent) :
    QWidgetAction(parent)
{
    browser = (NBrowserWindow*)parent;
}


void ToolbarWidgetAction::setType(Button type) {
    buttonType = type;
}

QWidget *ToolbarWidgetAction::createWidget(QWidget *parent) {
    switch(buttonType) {
    case(Undo):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":undo.png"));
        button->setToolTip(tr("Undo"));
        break;
    case(Redo):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":redo.png"));
        button->setToolTip(tr("Redo"));
        break;
    case(Cut):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":cut.png"));
        button->setToolTip(tr("Copy"));
        break;
    case(Copy):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":copy.png"));
        button->setToolTip(tr("Copy"));
        break;
    case(Paste):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":paste.png"));
        button->setToolTip(tr("Paste"));
        break;
    case(Bold):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":bold.png"));
        button->setToolTip(tr("Bold"));
        break;
    case(Italics):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":italics.png"));
        button->setToolTip(tr("Italics"));
        break;
    case(Strikethrough):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":strikethrough.png"));
        button->setToolTip(tr("Strikethrough"));
        break;
    case(Underline):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":underline.png"));
        button->setToolTip(tr("Underline"));
        break;
    case(AlignLeft):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":leftAlign.png"));
        button->setToolTip(tr("Left Justify"));
        break;
    case(AlignRight):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":rightAlign.png"));
        button->setToolTip(tr("Right Justify"));
        break;
    case(AlignCenter):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":centerAlign.png"));
        button->setToolTip(tr("Center Justify"));
        break;
    case(HorizontalLine):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":hline.png"));
        button->setToolTip(tr("Horizontal Line"));
        break;
    case(ShiftRight):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":shiftRight.png"));
        button->setToolTip(tr("Shift Right"));
        break;
    case(ShiftLeft):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":shiftLeft.png"));
        button->setToolTip(tr("Shift Left"));
        break;
    case(NumberList):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":numberList.png"));
        button->setToolTip(tr("Number List"));
        break;
    case(BulletList):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":bulletList.png"));
        button->setToolTip(tr("Button List"));
        break;
    case(SpellCheck):
        button = new QPushButton(parent);
        button->setIcon(QIcon(":spellCheck.png"));
        button->setToolTip(tr("Spell Check"));
        break;
    case (Todo):
        button = new QPushButton(parent);
        button->setToolTip(tr("To-do"));
        button->setIcon(QIcon(":todo.png"));
        break;
    }
    if (button != NULL) {
        button->setFlat(true);
        button->setStyleSheet("QPushButton{min-width:30px; min-height:15px;}");
        return button;
    }
    return 0;
}


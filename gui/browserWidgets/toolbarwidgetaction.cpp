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

QWidget *ToolbarWidgetAction::createWidget() {
    switch(buttonType) {
    case(Undo):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":undo.png"));
        button->setToolTip(tr("Undo"));
        break;
    case(Redo):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":redo.png"));
        button->setToolTip(tr("Redo"));
        break;
    case(Cut):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":cut.png"));
        button->setToolTip(tr("Copy"));
        break;
    case(Copy):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":copy.png"));
        button->setToolTip(tr("Copy"));
        break;
    case(Paste):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":paste.png"));
        button->setToolTip(tr("Paste"));
        break;
    case(Bold):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":bold.png"));
        button->setToolTip(tr("Bold"));
        break;
    case(Italics):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":italics.png"));
        button->setToolTip(tr("Italics"));
        break;
    case(Strikethrough):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":strikethrough.png"));
        button->setToolTip(tr("Strikethrough"));
        break;
    case(Underline):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":underline.png"));
        button->setToolTip(tr("Underline"));
        break;
    case(AlignLeft):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":leftAlign.png"));
        button->setToolTip(tr("Left Justify"));
        break;
    case(AlignRight):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":rightAlign.png"));
        button->setToolTip(tr("Right Justify"));
        break;
    case(AlignCenter):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":centerAlign.png"));
        button->setToolTip(tr("Center Justify"));
        break;
    case(HorizontalLine):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":hline.png"));
        button->setToolTip(tr("Horizontal Line"));
        break;
    case(ShiftRight):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":shiftRight.png"));
        button->setToolTip(tr("Shift Right"));
        break;
    case(ShiftLeft):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":shiftLeft.png"));
        button->setToolTip(tr("Shift Left"));
        break;
    case(NumberList):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":numberList.png"));
        button->setToolTip(tr("Number List"));
        break;
    case(BulletList):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":bulletList.png"));
        button->setToolTip(tr("Button List"));
        break;
    case(SpellCheck):
        button = new QPushButton(browser);
        button->setIcon(QIcon(":spellCheck.png"));
        button->setToolTip(tr("Spell Check"));
        break;
    case (Todo):
        button = new QPushButton(browser);
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


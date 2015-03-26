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

#include "treewidgeteditor.h"
#include <QTreeWidget>
#include <QEvent>
#include <QKeyEvent>
#include "global.h"


extern Global global;

TreeWidgetEditor::TreeWidgetEditor(QTreeWidget *parent) :
    QLineEdit(parent)
{
    this->parent = parent;
    this->setFont(global.getGuiFont(font()));
    lid = 0;
    stackName = "";
    connect(this, SIGNAL(returnPressed()), SLOT(textChanged()));
}


void TreeWidgetEditor::setTreeWidgetItem(QTreeWidgetItem *w, int col) {
   this->originalWidgetItem = w;
   this->column = col;
   this->originalWidget = parent->itemWidget(w, col);
}

void TreeWidgetEditor::focusOutEvent(QFocusEvent *e) {
    // suppress unused
    Q_UNUSED(e);

    originalWidgetItem->setData(0, Qt::DisplayRole, text().trimmed());
    parent->setItemWidget(originalWidgetItem, column, originalWidget);
    emit(editComplete());
}


// When a user presses "Enter", force a focus out to save the data
void TreeWidgetEditor::textChanged() {
//    originalWidgetItem->setData(0, Qt::DisplayRole, text().trimmed());
//    parent->setItemWidget(originalWidgetItem, column, originalWidget);
    emit(editComplete());
}


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

TreeWidgetEditor::TreeWidgetEditor(QTreeWidget *parent) :
    QLineEdit(parent)
{
    this->parent = parent;
    QFont f = font();
    f.setPointSize(8);
    setFont(f);
    lid = 0;
    stackName = "";
}


void TreeWidgetEditor::setTreeWidgetItem(QTreeWidgetItem *w, int col) {
   this->originalWidgetItem = w;
   this->column = col;
   this->originalWidget = parent->itemWidget(w, col);

}

void TreeWidgetEditor::focusOutEvent(QFocusEvent *e) {
    e=e; // suppress unused
    originalWidgetItem->setData(0, Qt::DisplayRole, text().trimmed());
    parent->setItemWidget(originalWidgetItem, column, originalWidget);
    emit(editComplete());
}

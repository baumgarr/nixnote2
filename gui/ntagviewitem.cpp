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

#include "ntagviewitem.h"

NTagViewItem::NTagViewItem(QTreeWidget* parent):QTreeWidgetItem(parent) {
    parentLid = 0;     // The Id of the parent to this tag
    account = 0;        // The notebook account that owns this tag (non zero if this tag is from a linked notebook).
    childrenLids.clear();  // Children of this tag.
    parentGuid = "";    // GUID of the parent of this tag.
    subTotal = 0;          // Count of notes matching this tag
    total = 0;
}


NTagViewItem::NTagViewItem():QTreeWidgetItem(){
    parentLid = 0;     // The Id of the parent to this tag
    account = 0;        // The notebook account that owns this tag (non zero if this tag is from a linked notebook).
    childrenLids.clear();  // Children of this tag.
    parentGuid = "";    // GUID of the parent of this tag.
    subTotal = 0;          // Count of notes matching this tag
    total = 0;          // Total of all notes with this tag
}


// Setup the root view of the tree
void NTagViewItem::setRootColor(bool val) {
    if (val) {
        QFont f;
        f.setBold(true);
        setFont(0, f);
    }
}



// Used when sorting tags.  We want to sort case insensitive
bool NTagViewItem::operator<(const QTreeWidgetItem &other)const {
    int column = treeWidget()->sortColumn();
    return text(column).toLower() < other.text(column).toLower();
}

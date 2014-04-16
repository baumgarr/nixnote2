/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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



#include "favoritesviewitem.h"

FavoritesViewItem::FavoritesViewItem(QTreeWidget* parent):QTreeWidgetItem(parent){
    total=0;
    subTotal=0;
}

FavoritesViewItem::FavoritesViewItem():QTreeWidgetItem(){}


bool FavoritesViewItem::operator<(const QTreeWidgetItem &other)const {
     int column = treeWidget()->sortColumn();
     FavoritesViewItem *i = (FavoritesViewItem*)&other;
     if (record.order != i->record.order)
             return record.order < i->record.order;
     return text(column).toLower() < other.text(column).toLower();
}


void FavoritesViewItem::setRootColor(bool val) {
    if (val) {
        QFont f;
        f.setBold(true);
        setFont(0, f);
    }
}

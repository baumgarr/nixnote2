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



#include "favoritesviewdelegate.h"
#include "gui/favoritesview.h"
#include "gui/favoritesviewitem.h"
#include "global.h"

extern Global global;

FavoritesViewDelegate::FavoritesViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}


QSize FavoritesViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize sz=QStyledItemDelegate::sizeHint(option, index);
    return sz;
}




void FavoritesViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
#if QT_VERSION < 0x050000
    QStyleOptionViewItemV4 options = option;
#else
    QStyleOptionViewItem options = option;
#endif
    initStyleOption(&options, index);

    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);
    if (global.countBehavior == Global::CountNone)
        return;
    qint32 lid = index.data(Qt::UserRole).toInt();
    QString test = index.data(Qt::UserRole).toString().toLower();
    if (test == "stack")
        return;

    FavoritesView *tree = NULL;
    FavoritesViewItem *item = NULL;

    if (lid > 0) {
        tree = (FavoritesView*)options.widget;
        item = tree->dataStore[lid];
    } else {
        return;
    }
    if (item->record.type == FavoritesRecord::NotebookStack
            || item->record.type == FavoritesRecord::LinkedStack
            || item->record.type == FavoritesRecord::Search)
        return;

    qint32 total = item->total;
    qint32 subTotal = item->subTotal;
    if (total < 0 || subTotal < 0) {
        return;
    }

    QString countString;
    if (total == subTotal){
        countString = QString("(")+QString::number(total) + QString(")");
    } else {
        countString = QString("(")+QString::number(subTotal) + QString("/") + QString::number(total) + QString(")");
    }
    painter->save();

    // shift text right to make icon visible
    QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left()+iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());

    painter->setClipRect(clip);
    QFont f = options.font;
    f.setBold(false);
    painter->setFont(f);
    painter->setPen(Qt::darkGray);
    //painter->drawText(6+fm.width(index.data().toString()+QString("   ")),fm.ascent(),countString);

    painter->restore();
}

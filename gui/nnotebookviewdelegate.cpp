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

#include "nnotebookviewdelegate.h"
#include <QPainter>
#include "nnotebookviewitem.h"
#include "nnotebookview.h"
#include "global.h"

extern Global global;

NNotebookViewDelegate::NNotebookViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}



void NNotebookViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    qint32 lid = index.data(Qt::UserRole).toInt();
    if (lid > 0) {

        NNotebookView  *tree = (NNotebookView*)options.widget;
        NNotebookViewItem *item = tree->dataStore[lid];
        qint32 total = item->total;
        qint32 subTotal = item->subTotal;
        QString countString;
        if (total == subTotal){
            countString = QString("(")+QString::number(total) + QString(")");
        } else {
            countString = QString("(")+QString::number(subTotal) + QString("/") + QString::number(total) + QString(")");
        }

        // shift text right to make icon visible
        QSize iconSize = options.icon.actualSize(options.rect.size());
        painter->translate(options.rect.left()+iconSize.width(), options.rect.top());
        QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());

        painter->setClipRect(clip);
        QFontMetrics fm = options.fontMetrics;
        painter->setPen(Qt::darkGray);
        painter->drawText(6+fm.width(index.data().toString()+QString("   ")),fm.height()-1,countString);
    }
    painter->restore();
}



QSize NNotebookViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
//    int theWidth=-1;

//    // handle other types of view here if needed
//    QTreeView* tree=qobject_cast<QTreeView*>(this->);
//    if(tree)
//    {
//        qDebug("WIDTH @ %d : %d",index.column(),tree->header()->sectionSize(index.column()));
//        theWidth=tree->header()->sectionSize(index.column());
//    }
    QSize sz=QStyledItemDelegate::sizeHint(option, index);
//    if(theWidth>=0)
//        sz.setWidth(theWidth);

    return sz;
}

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


#include "truefalsedelegate.h"
#include "global.h"
#include "sql/notetable.h"

extern Global global;

TrueFalseDelegate::TrueFalseDelegate()
{
}


void TrueFalseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool value = index.data().toBool();
    QModelIndex ix;  // Dummy model index so we don't put the actual value in the column
    QStyledItemDelegate::paint(painter,option, ix);
    if(value)
    {
        painter->save();
        QPixmap dot = global.getPixmapResource(":blackDotIcon");
        int centerDot = dot.width()/2;
        int len = (option.rect.right() - option.rect.left())/2;
        len = len-centerDot;
        painter->drawPixmap(option.rect.x()+len,option.rect.y()+2,dot);
        painter->restore();
    }

}

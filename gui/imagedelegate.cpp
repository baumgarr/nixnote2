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


#include "imagedelegate.h"
#include <QPixmap>
#include <QPainter>

ImageDelegate::ImageDelegate()
{
}



QString ImageDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    Q_UNUSED(locale); // suppress unused variable
    Q_UNUSED(value);
    return "";
}



void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option, index);
    QString filename = index.data().toString();

    if(filename != "")
    {
        QStyleOptionViewItemV4 options = option;
        initStyleOption(&options, index);

        QImage image(filename);
        if (image.isNull())
            return;

        painter->save();
        QSize imageSize = options.icon.actualSize(options.rect.size());
        imageSize.scale(imageSize.width(), imageSize.height(), Qt::KeepAspectRatioByExpanding);

        painter->translate(options.rect.left() + imageSize.width(), options.rect.top());

        QPixmap pix = QPixmap::fromImage(image);
        pix = pix.scaledToHeight(options.rect.height(), Qt::SmoothTransformation);
        if (pix.width() > options.rect.width())
            pix = pix.scaledToWidth(options.rect.width(), Qt::SmoothTransformation);

        painter->drawPixmap(0,0,pix);

        painter->restore();
    }

}


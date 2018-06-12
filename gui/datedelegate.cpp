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

#include "datedelegate.h"
#include <QDateTime>
#include "global.h"

extern Global global;

DateDelegate::DateDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}


QString DateDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    Q_UNUSED(locale); // suppress unused variable

    if (value.toLongLong() == 0)
        return "";
    QDateTime timestamp;
    timestamp.setTime_t(value.toLongLong()/1000);

    //QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));
    if (timestamp.date() == QDate::currentDate())
        return tr("Today") +" " + timestamp.time().toString(global.timeFormat);
    return timestamp.toString(global.dateFormat + QString(" ") +global.timeFormat);
//    return timestamp.toString(Qt::SystemLocaleShortDate);
}


void DateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
{
    QStyleOptionViewItem noption = QStyleOptionViewItem(option);
    noption.textElideMode =  Qt::ElideNone;

    QStyledItemDelegate::paint(painter, noption, index);
}


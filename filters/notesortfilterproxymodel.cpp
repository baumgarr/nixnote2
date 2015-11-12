/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#include "notesortfilterproxymodel.h"
#include "global.h"

NoteSortFilterProxyModel::NoteSortFilterProxyModel() :
    QSortFilterProxyModel()
{
    lidMap = new QMap<qint32,qint32>();
}


NoteSortFilterProxyModel::~NoteSortFilterProxyModel()
{
    delete lidMap;
}


bool NoteSortFilterProxyModel::filterAcceptsRow(qint32 source_row, const QModelIndex &source_parent) const {
    QModelIndex idx = sourceModel()->index(source_row,NOTE_TABLE_LID_POSITION, source_parent);
    qint32 rowLid = sourceModel()->data(idx).toInt();
    if (lidMap->contains(rowLid)) {
        lidMap->remove(rowLid);
        lidMap->insert(rowLid, source_row);
        return true;
    }
    return false;
}


bool NoteSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QVariant leftData = this->sourceModel()->data(left);
    QVariant rightData = this->sourceModel()->data(right);

    if (left.column() == NOTE_TABLE_IS_DIRTY_POSITION)
        return leftData.toBool() < rightData.toBool();

    if (leftData.type() == QVariant::Invalid || rightData.type() == QVariant::Invalid)
        return true;

    if (leftData.type() == QVariant::Icon || rightData.type() == QVariant::Icon)
        return true;

    if (leftData.type() == QVariant::Image || rightData.type() == QVariant::Image)
        return true;

    if (leftData.type() == QVariant::String && rightData.type() == QVariant::String) {
        QString left = leftData.toString();
        QString right = rightData.toString();
        return left.toLower() < right.toLower();
    }

    if (leftData.type() == QVariant::Int && rightData.type() == QVariant::Int)
        return leftData.toInt() < rightData.toInt();

    if (leftData.type() == QVariant::LongLong && rightData.type() == QVariant::LongLong)
        return leftData.toLongLong() < rightData.toLongLong();

    if (leftData.type() == QVariant::Double && rightData.type() == QVariant::Double)
        return leftData.toDouble() < rightData.toDouble();

    return true;
}


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

#ifndef NOTESORTFILTERPROXYMODEL_H
#define NOTESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QMap>
#include <stdint.h>

class NoteSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit NoteSortFilterProxyModel();
    ~NoteSortFilterProxyModel();
    bool filterAcceptsRow(qint32 source_row, const QModelIndex &source_parent) const;
    //void sort(int column, Qt::SortOrder order);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    QMap<qint32, qint32> *lidMap;

signals:

public slots:

};

#endif // NOTESORTFILTERPROXYMODEL_H

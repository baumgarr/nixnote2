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



#ifndef FAVORITESTABLE_H
#define FAVORITESTABLE_H

#include <QObject>
#include "sql/favoritesrecord.h"
#include <QSqlDatabase>
#include <QPair>

#define FAVORITES_TYPE      200
#define FAVORITES_TARGET    201
#define FAVORITES_ORDER     202
#define FAVORITES_PARENT    203



// Read/Write a favorite records to the database

class FavoritesTable : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase *db;

public:
    explicit FavoritesTable(QSqlDatabase *db, QObject *parent = 0);   // Generic constructor

    // DB Read Functions
    void getAll(QList<qint32> &lids);                                 // Get the lid of all favorites
    bool get(FavoritesRecord &record, qint32 lid);                    // Get a particular favorite record
    bool childrenFound(qint32 lid);                                   // Does this favorite have any children?

    // DB Write Functions
    void expunge(qint32 lid);                                         // Erase a favorite record
    void setOrder(QList< QPair< qint32, qint32 > > order);            // Set the order of display for a favorite
    qint32 add(const FavoritesRecord &record);                        // Add a new favorite
    qint32 insert(const FavoritesRecord &record);                     // Insert a new favorite
    qint32 getLidByTarget(const QVariant &target);                    // Get a favorite lid by an owner

signals:

public slots:

};

#endif // FAVORITESTABLE_H

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

#ifndef NTAGVIEWITEM_H
#define NTAGVIEWITEM_H

#include <QTreeWidgetItem>

class NTagViewItem : public QTreeWidgetItem
{
public:
    explicit NTagViewItem();
    explicit NTagViewItem(QTreeWidget* parent);
    qint32 parentLid;
    QList<qint32> childrenGuids;
    QString parentGuid;
    qint32 count;
    qint32 account;

    bool operator<(const QTreeWidgetItem &other)const;
    void setRootColor(bool val) ;


signals:
public slots:
    void setHidden(bool hide);

};

#endif // NTAGVIEWITEM_H

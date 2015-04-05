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

#ifndef NNOTEBOOKVIEWITEM_H
#define NNOTEBOOKVIEWITEM_H

#include <QTreeWidgetItem>

class NNotebookViewItem : public QTreeWidgetItem
{
public:
    enum NNotebookWidgetType {
        Local = 0,
        Synchronized = 1,
        Conflict = 2,
        Stack = 3,
        Shared = 4,
        Linked = 5,
        LinkedStack = 6
    };

    void setIconType();
    void setType(NNotebookWidgetType type);
    explicit NNotebookViewItem(qint32 lid);
    explicit NNotebookViewItem(qint32 lid, QTreeWidget* parent);
    QString stack;
    QList<qint32> childrenLids;
    qint32 subTotal;
    qint32 total;
    NNotebookWidgetType type;
    qint32 lid;

    bool operator<(const QTreeWidgetItem &other)const;
    void setRootColor(bool val) ;
    void reloadIcon();

signals:

public slots:

};

#endif // NNOTEBOOKVIEWITEM_H

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

#ifndef NTRASHTREE_H
#define NTRASHTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>

class NTrashTree : public QTreeWidget
{
    Q_OBJECT
private:
    int filterPosition;
    QTreeWidgetItem *root;
    QMenu contextMenu;
    QAction *restoreAction;
    QAction *expungeAction;
    virtual void mousePressEvent(QMouseEvent *event);

public:
    explicit NTrashTree(QWidget *parent = 0);
    void updateSelection();
    void resetSize();
    qint32 count;

signals:
    void updateSelectionRequested();

public slots:
    void contextMenuEvent(QContextMenuEvent *event);
    void updateTotals(qint32 total);

private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void buildSelection();
    void restoreAll();
    void expungeAll();
    
};

#endif // NTRASHTREE_H

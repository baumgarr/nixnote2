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

#ifndef NSEARCHVIEW_H
#define NSEARCHVIEW_H

#include <QTreeWidget>
#include <QHash>
#include <QMenu>
#include <QAction>
#include <QShortcut>

#include "global.h"
#include "nsearchviewitem.h"
#include "gui/treewidgeteditor.h"

//****************************************************
//* This is the tree for the list of saved searches.
//****************************************************



class NSearchView : public QTreeWidget
{
    Q_OBJECT
private:
    NSearchViewItem *root;
    virtual void mousePressEvent(QMouseEvent *event);
    int filterPosition;
    QMenu context;
    QAction *addAction;
    QAction *propertiesAction;
    QAction *deleteAction;
    QAction *renameAction;
    QShortcut *renameShortcut;
    QShortcut *addShortcut;
    QShortcut *deleteShortcut;
    TreeWidgetEditor *editor;


private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void editComplete();

public:
    explicit NSearchView(QWidget *parent = 0);
    ~NSearchView();
    QHash<qint32, NSearchViewItem*> dataStore;
    void resetSize();
    void loadData();
    void contextMenuEvent(QContextMenuEvent *event);
    QSize sizeHint();
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

signals:
    void updateSelectionRequested();

public slots:
    void searchUpdated(qint32 lid, QString name);
    void searchExpunged(qint32 lid);
    void buildSelection();
    void updateSelection();
    void addRequested();
    void propertiesRequested();
    void deleteRequested();
    void renameRequested();


};

#endif // NSEARCHVIEW_H

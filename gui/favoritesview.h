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



#ifndef FAVORITESVIEW_H
#define FAVORITESVIEW_H

#include <QTreeWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QImage>
#include <QPainter>
#include <QHeaderView>
#include <QShortcut>

#include "gui/favoritesviewitem.h"

class FavoritesView : public QTreeWidget
{
    Q_OBJECT
private:
    QImage *collapsedImage;
    QImage *expandedImage;
    QAction *deleteAction;
    QShortcut *deleteShortcut;
    int maxCount;
    void dropRecord(qint32 lid, FavoritesRecord::FavoritesRecordType type, int row);
    void buildTreeEntry(FavoritesViewItem *parent, const FavoritesRecord *record);

private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();

public:
    explicit FavoritesView(QWidget *parent = 0);
    virtual void mousePressEvent(QMouseEvent *event);
    int filterPosition;
    FavoritesViewItem *root;
    QMenu context;
    void resetSize();
    void updateSelection();
    void loadData();
    bool rebuildFavoritesTreeNeeded;
    void contextMenuEvent(QContextMenuEvent *event);
    QHash<qint32, FavoritesViewItem*> dataStore;
    QHash<qint32, FavoritesViewItem*> targetStore;
    QSize sizeHint();
    void dropEvent(QDropEvent *event);
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void reloadIcons();

signals:
    void updateSelectionRequested();
    void updateCounts();

public slots:
    void deleteRequested();
    void itemRenamed(qint32 lid, QString oldName, QString newName);
    void buildSelection();
    void updateTotals(qint32 lid, qint32 subTotal, qint32 total);
    void itemExpunged(qint32 lid);
    void itemExpunged(qint32 lid, QString name);
    void stackExpunged(QString stackname);
    void stackRenamed(QString oldName, QString newName);

};

#endif // FAVORITESVIEW_H

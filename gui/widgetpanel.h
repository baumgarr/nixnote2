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

#ifndef WIDGETPANEL_H
#define WIDGETPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QTimer>
#include "gui/favoritesview.h"
#include "gui/nnotebookview.h"
#include "gui/ntagview.h"
#include "gui/nsearchview.h"
#include "gui/nattributetree.h"
#include "gui/ntrashtree.h"


class WidgetPanel : public QWidget
{
    Q_OBJECT
private:
    void addWidget(QWidget *widget);
    FavoritesView *favoritesView;
    NTagView *tagView;
    NSearchView *searchView;
    NNotebookView *notebookView;
    NTrashTree *trashTree;
    NAttributeTree *attributeTree;
    QPoint priorMousePosition;
    QTimer timer;
    bool scrollUp;

public:
     QVBoxLayout *vboxLayout;

    explicit WidgetPanel(QWidget *parent = 0);
    ~WidgetPanel();
    void paintEvent(QPaintEvent *);
    QSize sizeHint();
    QScrollArea *scroll;
    void dragMoveHandler(QDragMoveEvent *event);
    void dragEnterHandler(QDragEnterEvent *event);
    void addFavoritesView(FavoritesView *view);
    void addNotebookView(NNotebookView *view);
    void addTagView(NTagView *view);
    void addSearchView(NSearchView *view);
    void addAttributeTree(NAttributeTree *view);
    void addTrashTree(NTrashTree *view);
    void addSeparator(QLabel *separator);

signals:

public slots:
    //void dragMoveEvent(QDragMoveEvent *event);
    void scrollTimer();

};

#endif // WIDGETPANEL_H

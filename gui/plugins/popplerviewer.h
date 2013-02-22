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

#ifndef POPPLERVIEWER_H
#define POPPLERVIEWER_H

#include <QImage>
#include <QStandardItemModel>
#include <QList>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <poppler-qt4.h>

#include "gui/plugins/popplergraphicsview.h"

class PopplerViewer : public QWidget
{
    Q_OBJECT

public:
    PopplerViewer(const QString &mimeType, const QString &lid, QWidget *parent = 0);

private:
    QGraphicsScene *scene;
    PopplerGraphicsView *view;
    QGraphicsPixmapItem *item;
    QImage *image;
    QString mimeType;
    QLabel *pageLabel;
    QLabel *imageLabel;
    int currentPage;
    int totalPages;
    QPushButton *pageLeft;
    QPushButton *pageRight;
    qint32 lid;
    QString printImageFile;

public:
    Poppler::Document *doc;

public slots:
    void pageRightPressed();
    void pageLeftPressed();
};

#endif // POPPLERVIEWER_H

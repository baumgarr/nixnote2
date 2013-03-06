/***************************************************************************
 *   Copyright (C) 2009 - 2011 by Artem 'DOOMer' Galichkin                        *
 *   doomer3d@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/



#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <QDialog>
#include "config.h"

#include <QtGui/QDialog>

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtCore/QSize>
#include <QtCore/QPoint>

class ScreenCapture : public QDialog
{
    Q_OBJECT
public:
    explicit ScreenCapture(QWidget *parent = 0);
    ~ScreenCapture();
    QPixmap getSelection();

signals:

private:
    QRect selectRect;
    QSize sizeDesktop;

    QPoint selStartPoint;
    QPoint selEndPoint;

    bool palBackground;

    QPixmap desktopPixmapBkg;
    QPixmap desktopPixmapClr;

    void drawBackGround();
    void drawRectSelection(QPainter &painter);

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
public slots:
    
};

#endif // SCREENCAPTURE_H

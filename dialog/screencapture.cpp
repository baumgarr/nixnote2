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



#include "screencapture.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>


ScreenCapture::ScreenCapture(QWidget *parent) :
    QDialog(parent)
{

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    setWindowState(Qt::WindowFullScreen);
    setCursor(Qt::CrossCursor);

    sizeDesktop = QApplication::desktop()->size();
    resize(sizeDesktop);

    desktopPixmapBkg = QPixmap::grabWindow(QApplication::desktop()->winId());
    desktopPixmapClr = desktopPixmapBkg;

    move(0, 0);
    drawBackGround();
}





ScreenCapture::~ScreenCapture()
{
}

bool ScreenCapture::event(QEvent *event)
{
  if (event->type() == QEvent::MouseButtonRelease
   || event->type() == QEvent::KeyPress)
  {
    accept();
  }
  if (event->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);

    if (mouseEvent->button() != Qt::LeftButton)
      reject();

    selStartPoint = mouseEvent->pos();
    selectRect = QRect(selStartPoint, QSize());
  }

  return QDialog::event(event);
}

void ScreenCapture::paintEvent(QPaintEvent *event)
{
    event=event;  // suppress unused variable warning
    QPainter painter(this);

    if (!palBackground)
      painter.drawPixmap(QPoint(0, 0), desktopPixmapBkg);

    drawRectSelection(painter);
}

void ScreenCapture::mouseMoveEvent(QMouseEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
    selectRect = QRect(selStartPoint, mouseEvent->pos()).normalized();
    selEndPoint  = mouseEvent->pos();
    update();
}



void ScreenCapture::drawBackGround()
{
    // create painter on  pixelmap of desktop
    QPainter painter(&desktopPixmapBkg);

    // set painter brush on 85% transparency
    painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));

    // draw rect of desktop size in poainter
    painter.drawRect(QApplication::desktop()->rect());

    QRect txtRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    QString txtTip = QApplication::tr("Use your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");

    txtRect.setHeight(qRound(txtRect.height() / 10)); // rounded val of text rect height

    painter.setPen(QPen(Qt::red)); // ste message rect border color
    painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
    QRect txtBgRect = painter.boundingRect(txtRect, Qt::AlignCenter, txtTip);

    // set height & width of bkg rect
    txtBgRect.setX(txtBgRect.x() - 6);
    txtBgRect.setY(txtBgRect.y() - 4);
    txtBgRect.setWidth(txtBgRect.width() + 12);
    txtBgRect.setHeight(txtBgRect.height() + 8);

    painter.drawRect(txtBgRect);

      // Draw the text
    painter.setPen(QPen(Qt::black)); // black color pen
    painter.drawText(txtBgRect, Qt::AlignCenter, txtTip);

    palBackground = (qApp->desktop()->numScreens() > 1);

    // set bkg to pallette widget
    if (palBackground)
    {
        QPalette newPalette = palette();
        newPalette.setBrush(QPalette::Window, QBrush(desktopPixmapBkg));
        setPalette(newPalette);
    }
}

void ScreenCapture::drawRectSelection(QPainter &painter)
{
    painter.drawPixmap(selectRect, desktopPixmapClr, selectRect);
    painter.setPen(QPen(QBrush(QColor(0, 0, 0, 255)), 2));
    painter.drawRect(selectRect);

    QString txtSize = QApplication::tr("%1 x %2 pixels ").arg(selectRect.width()).arg(selectRect.height());
    painter.drawText(selectRect, Qt::AlignBottom | Qt::AlignRight, txtSize);

    /* This does a zoom box around the selection.
    if (!selEndPoint.isNull() ) // && conf->getZoomAroundMouse() == true)
    {
        const quint8 zoomSide = 200;

        // create magnifer coords
        QPoint zoomStart = selEndPoint;
        zoomStart -= QPoint(zoomSide/5, zoomSide/5); // 40, 40

        QPoint zoomEnd = selEndPoint;
        zoomEnd += QPoint(zoomSide/5, zoomSide/5);

        // creating rect area for magnifer
        QRect zoomRect = QRect(zoomStart, zoomEnd);

        QPixmap zoomPixmap = desktopPixmapClr.copy(zoomRect).scaled(QSize(zoomSide, zoomSide), Qt::KeepAspectRatio);

        QPainter zoomPainer(&zoomPixmap); // create painter from pixmap maignifer
        zoomPainer.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2));
        zoomPainer.drawRect(zoomPixmap.rect()); // draw
        zoomPainer.drawText(zoomPixmap.rect().center() - QPoint(4, -4), "+");

        // position for drawing preview
        QPoint zoomCenter = selectRect.bottomRight();

        if (zoomCenter.x() + zoomSide > desktopPixmapClr.rect().width() || zoomCenter.y() + zoomSide > desktopPixmapClr.rect().height())
        {
            zoomCenter -= QPoint(zoomSide, zoomSide);
        }
        painter.drawPixmap(zoomCenter, zoomPixmap);
    }
    */
}

QPixmap ScreenCapture::getSelection()
{
    QPixmap sel;
    sel = desktopPixmapClr.copy(selectRect);
    return sel;
}


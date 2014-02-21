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


// Disclaimer!! A lot of this code is based on samples at:
// http://dournac.org/info/canny_edge
// http://opencv-srf.blogspot.com/2011/09/capturing-images-videos.html
// http://develnoter.blogspot.com/
// http://umanga.wordpress.com/2010/04/19/how-to-covert-qt-qimage-into-opencv-iplimage-and-wise-versa/



#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>

class CVImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CVImageWidget(QWidget *parent = 0);

    QSize sizeHint();
    QSize minimumSizeHint();
    bool getImage(QImage &image);

public slots:

    void showImage(const cv::Mat& image);

protected:
    void paintEvent(QPaintEvent* /*event*/);

    QImage _qimage;
    cv::Mat _tmp;
};



#endif // CVIMAGEWIDGET_H

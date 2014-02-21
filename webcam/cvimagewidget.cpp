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

#include "cvimagewidget.h"
#include <opencv2/opencv.hpp>



CVImageWidget::CVImageWidget(QWidget *parent) :
    QWidget(parent)
{
}


QSize CVImageWidget::sizeHint() { return _qimage.size(); }
QSize CVImageWidget::minimumSizeHint() { return _qimage.size(); }


void CVImageWidget::showImage(const cv::Mat& image) {
    // Convert the image to the RGB888 format
    switch (image.type()) {
    case CV_8UC1:
        cvtColor(image, _tmp, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(image, _tmp, CV_BGR2RGB);
        break;
    }

    // QImage needs the data to be stored continuously in memory
    assert(_tmp.isContinuous());
    // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
    // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
    // has three bytes.
    _qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols*3, QImage::Format_RGB888);

    this->setFixedSize(image.cols, image.rows);

    repaint();
}


void CVImageWidget::paintEvent(QPaintEvent* /*event*/) {
    // Display the image
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), _qimage);
    painter.end();
}



bool CVImageWidget::getImage(QImage &image) {
    if (_qimage.width() == 0 || _qimage.height() == 0)
        return false;
    image = _qimage;
    return true;
}

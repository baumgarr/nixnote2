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



#ifndef WEBCAMCAPTUREDIALOG_H
#define WEBCAMCAPTUREDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QTimer>

#include "../webcam/cvimagewidget.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"


class WebcamCaptureDialog : public QDialog
{
    Q_OBJECT
private:
    QVBoxLayout *mainLayout;       // main layout on the dialog
    cv::VideoCapture *cap;        // video capture device
    cv::Mat frame;                // matrix for image from webcam
    QPushButton *okButton;       // Ok push button
    QPushButton *cancelButton;   // cancel push button
    QTimer picTimer;  // timer to refresh the picture


public:
    explicit WebcamCaptureDialog(QWidget *parent = 0);
    ~WebcamCaptureDialog();
    CVImageWidget *webcamImage;       // Main display of image
    bool okPressed;                   // was the Ok button pressed?
    bool webcamReady;                 // Is the webcam working?

signals:
    
public slots:
    void pictureRefresh();             // refresh the picture from camera
    void okButtonPressed();            // ok button was pushed
    void cancelButtonPressed();        // cancel button was pushed
    
};

#endif // WEBCAMCAPTUREDIALOG_H

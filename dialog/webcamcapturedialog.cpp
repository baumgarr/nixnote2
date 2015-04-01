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



#include "webcamcapturedialog.h"
#include "dialog/webcamcapturedialog.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include "global.h"

#include <QHBoxLayout>
#include <QSpacerItem>

extern Global global;

// Default constructor
WebcamCaptureDialog::WebcamCaptureDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setFont(global.getGuiFont(font()));
    webcamReady = true;
    this->setWindowTitle(tr("Webcam Capture"));
    this->setWindowIcon(global.getIconResource(":webcam.png"));
    mainLayout = new QVBoxLayout();
    webcamImage = new CVImageWidget();
    mainLayout->addWidget(webcamImage);
    this->setLayout(mainLayout);
    cap = new cv::VideoCapture(0); // open the video camera no. 0
    if (!cap->isOpened())
        webcamReady = false;
    else {
        cap->read(frame);
        if (webcamImage->width() == 0 || webcamImage->height() == 0)
            webcamReady = false;
        else
            webcamImage->showImage(frame);
    }
    okPressed = false;

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    cancelButton = new QPushButton(this);
    okButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    okButton->setText(tr("OK"));
    buttonLayout->addSpacerItem(new QSpacerItem(20,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacerItem(new QSpacerItem(20,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(new QSpacerItem(20,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));

    connect(&picTimer, SIGNAL(timeout()), this, SLOT(pictureRefresh()));
    picTimer.setInterval(10);
    picTimer.start();
}


// Destructor
WebcamCaptureDialog::~WebcamCaptureDialog() {
    delete cap;
}



// Ok button was pressed
void WebcamCaptureDialog::okButtonPressed() {
    picTimer.stop();
    okPressed = true;
    this->close();
}


// Cancel button was pressed
void WebcamCaptureDialog::cancelButtonPressed() {
    okPressed = false;
    this->close();
}


// Refresh the QImage from the camera
void WebcamCaptureDialog::pictureRefresh() {
    cap->read(frame);
    if (webcamImage->width() == 0 || webcamImage->height() == 0)
        webcamReady = false;
    else
        webcamImage->showImage(frame);
}

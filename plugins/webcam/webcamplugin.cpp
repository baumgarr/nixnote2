/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2016 Randy Baumgarte

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


#include "webcamplugin.h"
#include <QDebug>



// Default constructor
WebCamPlugin::WebCamPlugin()
{
    initialized=false;
}



// Update the picture displayed.
void WebCamPlugin::pictureRefresh() {
    if (isWebcamReady())
        dialog->pictureRefresh();
}



// Check if the webcam is ready for use
bool WebCamPlugin::isWebcamReady() {
    return dialog->webcamReady;
}



// Initialize for use. I don't do it in the constructor because I don't 
// want to take the time unless the user REALLY wants to use the webcam.
// It also solves the problem of users 
void WebCamPlugin::initialize()  {
    if (!initialized)
        dialog = new WebcamCaptureDialog();
    initialized=true;
    return;
}


bool WebCamPlugin::okPressed()  {
    return dialog->okPressed;
}

void WebCamPlugin::exec() {
    dialog->exec();
}


bool WebCamPlugin::getImage(QImage &image) {
    return dialog->webcamImage->getImage(image);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(webcamplugin, WebCamPlugin)
#endif

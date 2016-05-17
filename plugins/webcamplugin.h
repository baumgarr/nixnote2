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


// This overrides the virtual plugin WebCamInterface.

#ifndef WEBCAMPLUGIN_H
#define WEBCAMPLUGIN_H
#include "webcaminterface.h"
#include "../dialog/webcamcapturedialog.h"

class WebCamPlugin : public QObject, WebCamInterface
{
    Q_OBJECT
    Q_INTERFACES(WebCamInterface)

private:
    WebcamCaptureDialog *dialog;
    bool initialized;

public:
    WebCamPlugin();
    void initialize();
    bool getImage(QImage &image);
    bool isWebcamReady();
    void exec();
    bool okPressed();
    void pictureRefresh();
};

#endif // WEBCAMPLUGIN_H

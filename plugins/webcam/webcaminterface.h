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


// This class is used as a virtual .


#ifndef WEBCAMINTERFACE_H
#define WEBCAMINTERFACE_H

#include <QtPlugin>
#include <QImage>

class WebCamInterface
{
public:
    virtual ~WebCamInterface() {}

    virtual void initialize() = 0;
    virtual bool getImage(QImage &image) = 0;
    virtual bool isWebcamReady() = 0;
    virtual void exec() = 0;
    virtual bool okPressed() = 0;
    virtual void pictureRefresh() = 0;
};

Q_DECLARE_INTERFACE(WebCamInterface, "org.nixnote.NixNote2.WebCamInterface/1.0")

#endif // WEBCAMINTERFACE_H

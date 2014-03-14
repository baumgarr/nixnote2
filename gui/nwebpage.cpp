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

#include "nwebpage.h"
#include <QDebug>
#include <QWebFrame>
#include "global.h"

//*******************************************************
//* This class inherits everything from QWebPage.  It
//* handles some of the special things we need in
//* QWebPage to be able to edit notes.
//*******************************************************

NWebPage::NWebPage(QWidget *parent) :
    QWebPage(parent)
{
    connect(this,SIGNAL(contentsChanged()), this, SLOT(editAlert()));
    mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
}


// This class overrides the one provided by QWebPage.  This shows us
// any javascript console messages that are produced.
void NWebPage::javaScriptConsoleMessage(QString message, int lineNumber, QString sourceID) {
   QLOG_DEBUG() << "Javascript message: " << "Line: " << lineNumber << " Source: " << sourceID <<
           " message: " << message;
}


void NWebPage::editAlert() {
    isDirty = true;
}

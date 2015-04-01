/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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


#include "externalbrowse.h"
#include <QGridLayout>
#include <QLayout>
#include "global.h"

extern Global global;

ExternalBrowse::ExternalBrowse(qint32 lid, QWidget *parent) :
    QMdiSubWindow(parent)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    this->setWindowTitle(tr("NixNote"));
    setWindowIcon(global.getIconResource(":windowIcon.png"));

    browser = new NBrowserWindow(this);
    setWidget(browser);

    browser->setContent(lid);
}


// We don't really close the window, we just hide it.  This
// solves problems later on if the user wants to re-open the same
// note in an external window.
void ExternalBrowse::closeEvent(QCloseEvent *closeEvent) {
    if (this->browser->editor->isDirty)
        this->browser->saveNoteContent();
    this->setVisible(false);
    closeEvent->ignore();
}



void ExternalBrowse::setTitle(QString text) {
    this->setWindowTitle(tr("NixNote - ") +text);
}

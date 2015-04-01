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



#include "reminderbutton.h"
#include <QMenu>
#include <QAction>
#include <QFont>

#include "global.h"

extern Global global;

ReminderButton::ReminderButton(QWidget *parent) :
    QPushButton(parent)
{
    setIcon(global.getIconResource(":alarmclock.png"));
    doneAction = menu.addAction(tr("Mark as Done"));
    setAction = menu.addAction(tr("Change Date"));
    clearAction = menu.addAction(tr("Clear Reminder"));
    QFont f = font();
    this->setFont(f);
    menu.setFont(f);
    this->setMenu(&menu);
}

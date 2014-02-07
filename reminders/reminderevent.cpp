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


#include "reminderevent.h"
#include "global.h"

#include <QDateTime>

ReminderEvent::ReminderEvent(QObject *parent) :
    QObject(parent)
{
    timer.setSingleShot(true);
}


bool ReminderEvent::setTimer(qint32 lid, QDateTime time) {
    this->lid = lid;
    this->time = time.currentMSecsSinceEpoch();
    QDateTime now = QDateTime::currentDateTime();
    int interval = (now.secsTo(time)+1)*1000;
    if (interval>0) {
        timer.stop();
        timer.start(interval);
        return true;
    }
    return false;
}



bool ReminderEvent::setTimer(qint32 lid, qlonglong time) {
    this->lid = lid;
    this->time = time;
    QDateTime now = QDateTime::currentDateTime();
    qlonglong interval = time - now.currentMSecsSinceEpoch();
    if (interval>0) {
        timer.start(interval);
        return true;
    }
    return false;
}

void ReminderEvent::stopTimer() {
    timer.stop();
}

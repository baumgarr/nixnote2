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

#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QList>
#include <QSystemTrayIcon>

#include "reminders/reminderevent.h"

class ReminderManager : public QObject
{
    Q_OBJECT
private:
    QList<ReminderEvent*> reminders;

public:
    explicit ReminderManager(QObject *parent = 0);
    QSystemTrayIcon *trayIcon;
    void reloadTimers();
    void checkReminders();
    void updateReminder(qint32 lid, QDateTime time);
    void remove(qint32 lid);
    
signals:
    
public slots:
    void timerPop();
    
};

#endif // REMINDERMANAGER_H

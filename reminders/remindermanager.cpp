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


#include "remindermanager.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

ReminderManager::ReminderManager(QObject *parent) :
    QObject(parent)
{
}


void ReminderManager::reloadTimers() {
    for (int i=reminders.size()-1; i>=0; i--) {
        delete reminders[i];
    }

    ReminderEvent *event = new ReminderEvent();
    connect(&event->timer, SIGNAL(timeout()), this, SLOT(timerPop()));

    NoteTable ntable(global.db);
    QList< QPair<qint32, qlonglong>* > notes;
    ntable.getAllReminders(&notes);

    for (int i=notes.size() -1; i>=0; i--) {
        ReminderEvent *event = new ReminderEvent();
        event->lid = notes[i]->first;
        connect(&event->timer, SIGNAL(timeout()), this, SLOT(timerPop()));
        event->setTimer(notes[i]->first, notes[i]->second);
        reminders.append(event);
    }
}


void ReminderManager::timerPop() {
    if (trayIcon == NULL)
        return;
    checkReminders();
}


void ReminderManager::checkReminders() {
    QString msg;
    NoteTable ntable(global.db);
    QDateTime now = QDateTime::currentDateTime();
    for (int i=reminders.size()-1; i>=0; i--) {
        Note note;
        ReminderEvent *event;
        event = reminders[i];
        if (event->time > global.getLastReminderTime() ||
                global.getLastReminderTime() == 0) {
            if (event->time <= now.currentMSecsSinceEpoch()) {
              ntable.get(note, event->lid, false);
              QString title = QString::fromStdString(note.title);
              msg = msg+title+"\n";
              delete reminders[i];
              reminders.removeAt(i);
            }
        } else {
            if (event->time < global.getLastReminderTime()) {
                delete reminders[i];
                reminders.removeAt(i);
            }
        }
    }
    if (msg.trimmed() != "")
        trayIcon->showMessage(tr("Reminders Due"), msg);
    global.setLastReminderTime(now.currentMSecsSinceEpoch());
}



void ReminderManager::updateReminder(qint32 lid, QDateTime time) {
    for (int i=0; i<reminders.size(); i++) {
        if (reminders[i]->lid == lid) {
            reminders[i]->setTimer(lid, time);
            return;
        }
    }
    ReminderEvent *event = new ReminderEvent();
    event->lid = lid;
    connect(&event->timer, SIGNAL(timeout()), this, SLOT(timerPop()));
    event->setTimer(lid, time);
    reminders.append(event);
}


void ReminderManager::remove(qint32 lid) {
    for (int i=reminders.size()-1; i>=0; i--) {
        if (reminders[i]->lid == lid) {
            delete reminders[i];
            reminders.removeAt(i);
        }
    }
}

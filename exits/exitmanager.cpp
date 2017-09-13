/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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


#include "exitmanager.h"
#include "global.h"
#include "exits/exitpoint.h"

#include <QSettings>

extern Global global;

ExitManager::ExitManager(QObject *parent) : QObject(parent)
{
    exitPoints = new QHash<QString, ExitPoint*>();
    exitPoints->clear();
}



void ExitManager::loadExits() {
    exitPoints->clear();
    loadExitPoint("ExitPoint_LoadNote");
    loadExitPoint("ExitPoint_SaveNote");
    loadExitPoint("ExitPoint_ImportKeep");
    loadExitPoint("ExitPoint_ImportDelete");
}


void ExitManager::loadExitPoint(QString name, int goodVersion) {
    exitPoints->remove(name);
    global.settings->beginGroup(name);
    bool enabled = global.settings->value("enabled", false).toBool();
    QString fileName = global.settings->value("script", "").toString();
    int version = global.settings->value("version", 1).toInt();
    global.settings->endGroup();

    if (fileName =="")  {
        QLOG_ERROR() << tr("Script filename is blank. Disabling exit ") << name;
        enabled = false;
    }

    if (version != goodVersion) {
        enabled = false;
        QLOG_ERROR() << tr("Unknown script version. Disabling exit.") << name;
    }

    QString script;
    QFile f(fileName);
    if (!f.exists()) {
        QLOG_ERROR() << tr("Script file doesn't exist or cannot be read. Disabling exit ") << name;
        enabled = false;
    } else {
        f.open(QIODevice::ReadOnly);
        script = f.readAll();
    }

    ExitPoint *point = new ExitPoint();
    point->setEnabled(enabled);
    point->setFileName(fileName);
    point->setVersion(version);
    point->setScript(script);
    point->setExitName(name);

    exitPoints->insert(name, point);

}

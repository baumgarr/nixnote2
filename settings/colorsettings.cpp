/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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



#include "colorsettings.h"
#include <QSettings>
#include <QPair>
#include <QStringList>
#include <QHash>

#include "global.h"

extern Global global;

ColorSettings::ColorSettings(QObject *parent) :
    QObject(parent)
{
    colors.clear();

    colors.insert(tr("White"), "white");
    colors.insert(tr("Red"), "Red");
    colors.insert(tr("Blue"), "blue");
    colors.insert(tr("Green"), "green");
    colors.insert(tr("Yellow"), "yellow");
    colors.insert(tr("Black"), "black");
    colors.insert(tr("Purple"), "purple");
    colors.insert(tr("Brown"), "brown");
    colors.insert(tr("Orange"), "orange");
    colors.insert(tr("Grey"), "grey");
    colors.insert(tr("Powder Blue"), "powderblue");

    QString userFile = global.fileManager.getHomeDirPath("") + QString("colors.txt");
    QString globalFile = global.fileManager.getProgramDirPath("") +QString("colors.txt");
    QSettings globalSettings(globalFile, QSettings::IniFormat);
    QSettings userSettings(userFile, QSettings::IniFormat);

    loadFile(globalSettings);
    loadFile(userSettings);

}


void ColorSettings::loadFile(QSettings &settings) {
    QStringList keyList = settings.allKeys();
    for (int i=0; i<keyList.size(); i++) {
        if (!keyList[i].startsWith("#")) {
            QString value = settings.value(keyList[i], "").toString();
            if (value != "") {
                if (colors.contains(keyList[i]))
                    colors[keyList[i]] = value;
                else
                    colors.insert(keyList[i], value);
            }
        }
    }
}



QList< QPair <QString, QString> > ColorSettings::colorList() {
    QStringList keys = colors.keys();
    keys.sort();
    QList< QPair <QString, QString> > list;

    for (int i=0; i<keys.size(); i++) {
        QPair<QString,QString> pair;
        pair.first = keys[i];
        pair.second = colors[keys[i]];
        list.append(pair);
    }
    return list;
}

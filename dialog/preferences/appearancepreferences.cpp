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

#include "appearancepreferences.h"
#include "global.h"

extern Global global;

AppearancePreferences::AppearancePreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);

    showTrayIcon = new QCheckBox(tr("Show tray icon"), this);

    defaultNotebookOnStartupLabel = new QLabel(tr("Notebook to use on startup"),this);
    defaultNotebookOnStartup = new QComboBox();
    defaultNotebookOnStartup->addItem(tr("Default notebook"), UseDefaultNotebook);
    defaultNotebookOnStartup->addItem(tr("All notebooks"), UseAllNotebooks);
    defaultNotebookOnStartup->addItem(tr("Last viewed"), UseLastViewedNotebook);
    defaultNotebookOnStartup->setEnabled(false);

    mainLayout->addWidget(showTrayIcon,0,0);
    mainLayout->addWidget(defaultNotebookOnStartupLabel,1,0);
    mainLayout->addWidget(defaultNotebookOnStartup, 1,1);

    global.settings->beginGroup("Appearance");
    showTrayIcon->setChecked(global.settings->value("showTrayIcon", false).toBool());
    global.settings->endGroup();
}


void AppearancePreferences::saveValues() {
    global.settings->beginGroup("Appearance");
    global.settings->setValue("showTrayIcon", showTrayIcon->isChecked());
    global.settings->endGroup();
}

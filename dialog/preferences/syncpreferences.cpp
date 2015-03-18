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

#include "syncpreferences.h"
#include "global.h"

extern Global global;

SyncPreferences::SyncPreferences(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    syncAutomatically = new QCheckBox(tr("Sync automatically"), this);
    syncAutomatically->setChecked(true);

    syncInterval = new QComboBox(this);
    syncInterval->addItem(tr("Every 15 minutes"), 15);
    syncInterval->addItem(tr("Every 30 minutes"), 30);
    syncInterval->addItem(tr("Every hour"), 60);
    syncInterval->addItem(tr("Every day"), 1440);

    syncOnStartup = new QCheckBox(tr("Sync on startup"), this);
    //syncOnStartup->setEnabled(false);
    syncOnShutdown = new QCheckBox(tr("Sync on shutdown"),this);
    //syncOnShutdown->setEnabled(false);
    enableSyncNotifications = new QCheckBox(tr("Enable sync notifications"), this);
    showGoodSyncMessagesInTray = new QCheckBox(tr("Show successful syncs"), this);

    mainLayout->addWidget(enableSyncNotifications,0,0);
    mainLayout->addWidget(showGoodSyncMessagesInTray, 0,1);
    mainLayout->addWidget(syncOnStartup,1,0);
    mainLayout->addWidget(syncOnShutdown,2,0);
    mainLayout->addWidget(syncAutomatically,3,0);
    mainLayout->addWidget(syncInterval, 3,1);

    global.settings->beginGroup("Sync");
    int interval = global.settings->value("syncInterval", 15).toInt();
    int index = syncInterval->findData(interval);
    syncInterval->setCurrentIndex(index);
    syncAutomatically->setChecked(global.settings->value("syncAutomatically", false).toBool());
    syncOnShutdown->setChecked(global.settings->value("syncOnShutdown", false).toBool());
    syncOnStartup->setChecked(global.settings->value("syncOnStartup", false).toBool());
    enableSyncNotifications->setChecked(global.settings->value("enableNotification", true).toBool());
    showGoodSyncMessagesInTray->setChecked(global.showGoodSyncMessagesInTray);
    global.settings->endGroup();
    global.showGoodSyncMessagesInTray = showGoodSyncMessagesInTray->isChecked();

    if (enableSyncNotifications->isChecked())
        showGoodSyncMessagesInTray->setEnabled(true);
    else
        showGoodSyncMessagesInTray->setEnabled(false);

    connect(syncAutomatically, SIGNAL(stateChanged(int)), this, SLOT(enableSyncStateChange()));
    connect(enableSyncNotifications, SIGNAL(toggled(bool)), this, SLOT(enableSuccessfulSyncMessagesInTray()));
}


SyncPreferences::~SyncPreferences() {
    delete syncInterval;
    delete syncOnShutdown;
    delete syncOnStartup;
    delete syncAutomatically;
}


void SyncPreferences::enableSyncStateChange() {
    if (syncAutomatically->isChecked())
        syncInterval->setEnabled(true);
    else
        syncInterval->setEnabled(false);
}

void SyncPreferences::enableSuccessfulSyncMessagesInTray() {
    if (this->enableSyncNotifications->isChecked())
        this->showGoodSyncMessagesInTray->setEnabled(true);
    else
        this->showGoodSyncMessagesInTray->setEnabled(false);
}

int SyncPreferences::getSyncInterval() {
    int index = syncInterval->currentIndex();
    return syncInterval->itemData(index).toInt();
}


void SyncPreferences::saveValues() {
    global.settings->beginGroup("Sync");
    global.settings->setValue("syncAutomatically", syncAutomatically->isChecked());
    global.settings->setValue("syncOnShutdown", syncOnShutdown->isChecked());
    global.settings->setValue("syncOnStartup", syncOnStartup->isChecked());
    global.settings->setValue("enableNotification", enableSyncNotifications->isChecked());
    global.settings->setValue("syncInterval", getSyncInterval());
    global.settings->setValue("showGoodSyncMessagesInTray", showGoodSyncMessagesInTray ->isChecked());
    global.settings->endGroup();
    global.showGoodSyncMessagesInTray = showGoodSyncMessagesInTray->isChecked();
}

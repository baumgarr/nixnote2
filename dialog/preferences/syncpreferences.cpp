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
    connect(syncAutomatically, SIGNAL(stateChanged(int)), this, SLOT(enableSyncStateChange()));

    syncInterval = new QComboBox(this);
    syncInterval->addItem(tr("Every 15 minutes"), 15);
    syncInterval->addItem(tr("Every 30 minutes"), 30);
    syncInterval->addItem(tr("Every hour"), 60);
    syncInterval->addItem(tr("Every day"), 1440);

    syncOnStartup = new QCheckBox(tr("Sync on startup"), this);
    syncOnStartup->setEnabled(false);
    syncOnShutdown = new QCheckBox(tr("Sync on shutdown"),this);
    syncOnShutdown->setEnabled(false);
    enableSyncNotifications = new QCheckBox(tr("Enable sync notifications"), this);

    mainLayout->addWidget(enableSyncNotifications,0,0);
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
    global.settings->endGroup();
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
    global.settings->endGroup();
}

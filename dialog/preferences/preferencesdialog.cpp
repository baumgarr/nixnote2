#include "preferencesdialog.h"
#include <QSpacerItem>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    okButtonPressed = false;
    setWindowTitle(tr("User Settings"));
    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    tabs = new QTabWidget(this);
    mainLayout->addWidget(tabs);

    this->setupAppearancePanel();
    this->setupSyncPanel();
    this->setupDebugPanel();

    cancelButton = new QPushButton(tr("Cancel"), this);
    okButton = new QPushButton(tr("Ok"), this);

    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));

    QSpacerItem *spacer1 = new QSpacerItem(0,0,QSizePolicy::MinimumExpanding);
    QSpacerItem *spacer2 = new QSpacerItem(0,0,QSizePolicy::MinimumExpanding);


    buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(spacer2);
    buttonLayout->setStretch(0,100);
    buttonLayout->setStretch(3,100);

    mainLayout->addLayout(buttonLayout);

    connect(appearancePanel->showTrayIcon, SIGNAL(clicked(bool)),
            syncPanel->enableSyncNotifications, SLOT(setEnabled(bool)));
    syncPanel->enableSyncNotifications->setEnabled(appearancePanel->showTrayIcon->isChecked());
}




PreferencesDialog::~PreferencesDialog() {
    delete okButton;
    delete cancelButton;
    delete buttonLayout;
    delete syncPanel;
    delete debugPanel;
    delete appearancePanel;
    delete tabs;
    delete mainLayout;
}

void PreferencesDialog::setupAppearancePanel() {
    appearancePanel = new AppearancePreferences();
    tabs->addTab(appearancePanel, tr("Appearance"));
}

void PreferencesDialog::setupSyncPanel() {
    syncPanel = new SyncPreferences(this);
    tabs->addTab(syncPanel, tr("Sync"));
}

void PreferencesDialog::setupDebugPanel() {
    debugPanel = new DebugPreferences(this);
    tabs->addTab(debugPanel, tr("Debugging"));
}


void PreferencesDialog::cancelButtonClicked() {
    this->close();
}


void PreferencesDialog::okButtonClicked() {
    okButtonPressed = true;
    appearancePanel->saveValues();
    debugPanel->saveValues();
    syncPanel->saveValues();
    this->close();
}

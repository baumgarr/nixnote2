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

#include "global.h"
#include "preferencesdialog.h"
#include <QSpacerItem>

extern Global global;

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    okButtonPressed = false;
    setWindowTitle(tr("User Settings"));
    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    tabs = new QTabWidget(this);
    scrollArea->setWidget(tabs);
    mainLayout->addWidget(scrollArea);
    this->setFont(global.getGuiFont(font()));

    this->setupAppearancePanel();
    this->setupLocalePanel();
    this->setupSyncPanel();
    this->setupSearchPanel();
    this->setupEmailPanel();
    this->setupThumbnailPanel();
    this->setupDebugPanel();

    cancelButton = new QPushButton(tr("Cancel"), this);
    okButton = new QPushButton(tr("OK"), this);

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
    this->resize(this->size().width()*1.2, this->size().height()*1.2);

    connect(appearancePanel->showTrayIcon, SIGNAL(clicked(bool)),
            syncPanel->enableSyncNotifications, SLOT(setEnabled(bool)));
    syncPanel->enableSyncNotifications->setEnabled(appearancePanel->showTrayIcon->isChecked());
}




PreferencesDialog::~PreferencesDialog() {
    delete okButton;
    delete cancelButton;
    delete buttonLayout;
    delete syncPanel;
    delete emailPanel;
    delete debugPanel;
    delete appearancePanel;
    delete tabs;
    delete mainLayout;
}

void PreferencesDialog::setupAppearancePanel() {
    appearancePanel = new AppearancePreferences();
    tabs->addTab(appearancePanel, tr("Appearance"));
}


void PreferencesDialog::setupLocalePanel() {
    localePanel = new LocalePreferences(this);
    tabs->addTab(localePanel, tr("Locale"));
}


void PreferencesDialog::setupSearchPanel() {
    searchPanel = new SearchPreferences();
    tabs->addTab(searchPanel, tr("Search"));
}


void PreferencesDialog::setupSyncPanel() {
    syncPanel = new SyncPreferences(this);
    tabs->addTab(syncPanel, tr("Sync"));
}


void PreferencesDialog::setupEmailPanel() {
    emailPanel = new EmailPreferences(this);
    tabs->addTab(emailPanel, tr("Email"));
}

void PreferencesDialog::setupThumbnailPanel() {
    thumbnailPanel = new ThumbnailPreferences(this);
    tabs->addTab(thumbnailPanel, tr("Thumbnail"));
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
    localePanel->saveValues();
    debugPanel->saveValues();
    syncPanel->saveValues();
    emailPanel->saveValues();
    searchPanel->saveValues();
    thumbnailPanel->saveValues();
    this->close();
}


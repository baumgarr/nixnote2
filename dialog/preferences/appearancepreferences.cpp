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
    showPDFs = new QCheckBox(tr("Display PDFs inline"), this);
    showSplashScreen = new QCheckBox(tr("Show splash screen on startup"), this);
    showMissedReminders = new QCheckBox(tr("Show missed reminders on startup"), this);
    startMinimized = new QCheckBox(tr("Always Start minimized"), this);
    windowIconChooser = new QComboBox();
    windowIconChooser->addItem(QIcon(":windowIcon0.png"), "", ":windowIcon0.png");
    windowIconChooser->addItem(QIcon(":windowIcon1.png"), "", ":windowIcon1.png");
    windowIconChooser->addItem(QIcon(":windowIcon2.png"), "", ":windowIcon2.png");
    windowIconChooser->addItem(QIcon(":windowIcon3.png"), "", ":windowIcon3.png");
    windowIconChooser->addItem(QIcon(":windowIcon4.png"), "", ":windowIcon4.png");
    windowIconChooser->addItem(QIcon(":windowIcon5.png"), "", ":windowIcon5.png");
    windowIconChooser->addItem(QIcon(":windowIcon6.png"), "", ":windowIcon6.png");
   // windowIconChooser->setSizePolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    windowIconChooser->adjustSize();
    windowIconChooser->setIconSize(QSize(80,80));

    defaultNotebookOnStartupLabel = new QLabel(tr("Startup Behavior"),this);
    defaultNotebookOnStartup = new QComboBox();
    defaultNotebookOnStartup->addItem(tr("Restore Selection Criteria"), UseLastViewedNotebook);
    defaultNotebookOnStartup->addItem(tr("Select Default Notebook"), UseDefaultNotebook);
    defaultNotebookOnStartup->addItem(tr("View All Notebooks"), UseAllNotebooks);

    mainLayout->addWidget(showTrayIcon,0,0);
    mainLayout->addWidget(showSplashScreen, 1,0);
    mainLayout->addWidget(showPDFs, 2,0);
    mainLayout->addWidget(showMissedReminders, 3, 0);
    mainLayout->addWidget(startMinimized, 4, 0);
    mainLayout->addWidget(defaultNotebookOnStartupLabel,5,0);
    mainLayout->addWidget(defaultNotebookOnStartup, 5,1);
    mainLayout->addWidget(new QLabel(tr("Window Icon")), 6,0);
    mainLayout->addWidget(windowIconChooser, 6,1);

    global.settings->beginGroup("Appearance");

    showTrayIcon->setChecked(global.settings->value("showTrayIcon", false).toBool());
    showPDFs->setChecked(global.settings->value("showPDFs", true).toBool());
    showSplashScreen->setChecked(global.settings->value("showSplashScreen", false).toBool());
    showMissedReminders->setChecked(global.settings->value("showMissedReminders", false).toBool());
    startMinimized->setChecked(global.settings->value("startMinimized", false).toBool());
    int defaultNotebook = global.settings->value("startupNotebook", UseLastViewedNotebook).toInt();
    defaultNotebookOnStartup->setCurrentIndex(defaultNotebook);
    global.settings->endGroup();
    QString windowIcon = global.getWindowIcon();
    int index = windowIconChooser->findData(windowIcon);
    windowIconChooser->setCurrentIndex(index);
}


void AppearancePreferences::saveValues() {
    global.settings->beginGroup("Appearance");
    global.settings->setValue("showTrayIcon", showTrayIcon->isChecked());
    global.settings->setValue("showPDFs", showPDFs->isChecked());
    global.pdfPreview = showPDFs->isChecked();
    global.settings->setValue("showSplashScreen", showSplashScreen->isChecked());
    global.settings->setValue("startMinimized", startMinimized->isChecked());
    global.settings->setValue("showMissedReminders", showMissedReminders->isChecked());
    int index = defaultNotebookOnStartup->currentIndex();
    int value = defaultNotebookOnStartup->itemData(index).toInt();
    global.settings->setValue("startupNotebook", value);
    index = windowIconChooser->currentIndex();
    global.settings->setValue("windowIcon", windowIconChooser->itemData(index).toString());
    global.settings->endGroup();
}

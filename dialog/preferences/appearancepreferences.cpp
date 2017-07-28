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

#include <QFontDatabase>
#include <QWebSettings>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#ifdef _WIN32
#include <QDesktopServices>
#endif

extern Global global;

AppearancePreferences::AppearancePreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);
    int middleClickIndex = global.getMiddleClickAction();

    showTrayIcon = new QCheckBox(tr("Show tray icon"), this);
    showPDFs = new QCheckBox(tr("Display PDFs inline**"), this);
    showSplashScreen = new QCheckBox(tr("Show splash screen on startup"), this);
    autoStart = new QCheckBox(tr("Start automatically at login"), this);
    confirmDeletes = new QCheckBox(tr("Confirm Deletes"), this);
    showMissedReminders = new QCheckBox(tr("Show missed reminders on startup"), this);
    startMinimized = new QCheckBox(tr("Always Start minimized"), this);
    dynamicTotals = new QCheckBox(tr("Show notebook and tag totals"), this);
    autoHideEditorButtonbar = new QCheckBox(tr("Auto-Hide editor toolbar"), this);
    autoHideEditorButtonbar->setChecked(global.autoHideEditorToolbar);
    disableEditingOnStartup = new QCheckBox(tr("Disable note editing on startup"), this);
    newNoteFocusOnTitle = new QCheckBox(tr("Focus on Note Title on New Note"), this);
    forceWebFonts = new QCheckBox(tr("Limit Editor to Web Fonts*"), this);
    forceWebFonts->setChecked(global.forceWebFonts);
    showNoteListGrid = new QCheckBox(tr("Show note list grid*"), this);
    alternateNoteListColors = new QCheckBox(tr("Alternate note list colors*"), this);
    autosetUserid = new QCheckBox(tr("Set author on new notes."),this);
    autosetUserid->setChecked(global.autosetUsername());
    fontPreviewInDialog  = new QCheckBox(tr("Preview fonts in editor dialag*"));
    fontPreviewInDialog->setChecked(global.previewFontsInDialog());

    traySingleClickAction = new QComboBox();
    traySingleClickAction->addItem(tr("Do Nothing"), -1);
    traySingleClickAction->addItem(tr("Show/Hide NixNote"), 0);
    traySingleClickAction->addItem(tr("New Text Note"), 1);
    traySingleClickAction->addItem(tr("New Quick Note"), 2);
    traySingleClickAction->addItem(tr("Screen Capture"), 3);

    trayMiddleClickAction = new QComboBox();
    trayMiddleClickAction->addItem(tr("Do Nothing"), -1);
    trayMiddleClickAction->addItem(tr("Show/Hide NixNote"), 0);
    trayMiddleClickAction->addItem(tr("New Text Note"), 1);
    trayMiddleClickAction->addItem(tr("New Quick Note"), 2);
    trayMiddleClickAction->addItem(tr("Screen Capture"), 3);

    trayDoubleClickAction = new QComboBox();
    trayDoubleClickAction->addItem(tr("Do Nothing"), -1);
    trayDoubleClickAction->addItem(tr("Show/Hide NixNote"), 0);
    trayDoubleClickAction->addItem(tr("New Text Note"), 1);
    trayDoubleClickAction->addItem(tr("New Quick Note"), 2);
    trayDoubleClickAction->addItem(tr("Screen Capture"), 3);

    mouseMiddleClickAction = new QComboBox();
    mouseMiddleClickAction->addItem(tr("Open New Tab"), MOUSE_MIDDLE_CLICK_NEW_TAB);
    mouseMiddleClickAction->addItem(tr("Open New Window"), MOUSE_MIDDLE_CLICK_NEW_WINDOW);
    mouseMiddleClickAction->setCurrentIndex(middleClickIndex);


    defaultGuiFontSizeChooser = new QComboBox();
    defaultFontChooser = new QComboBox();
    defaultFontSizeChooser = new QComboBox();
    connect(defaultFontChooser, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadFontSizes(QString)));
    loadFontNames(defaultFontChooser, global.defaultFont);

    defaultGuiFontChooser = new QComboBox();
    connect(defaultGuiFontChooser, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadGuiFontSizes(QString)));
    loadFontNames(defaultGuiFontChooser, global.defaultGuiFont);

    systemNotifier = new QComboBox();
    systemNotifier->addItem(tr("Qt Default"), "qt");
    systemNotifier->addItem(tr("notify-send"), "notify-send");

    windowThemeChooser = new QComboBox();
    windowThemeChooser->addItem(tr("System Default"));
    windowThemeChooser->addItems(global.getThemeNames());


    defaultNotebookOnStartupLabel = new QLabel(tr("Startup Behavior"),this);
    defaultNotebookOnStartup = new QComboBox();
    defaultNotebookOnStartup->addItem(tr("Restore Selection Criteria"), UseLastViewedNotebook);
    defaultNotebookOnStartup->addItem(tr("Select Default Notebook"), UseDefaultNotebook);
    defaultNotebookOnStartup->addItem(tr("View All Notebooks"), UseAllNotebooks);

    confirmDeletes->setChecked(global.confirmDeletes());

    int row=0;
    minimizeToTray = NULL;
    closeToTray = NULL;
    mainLayout->addWidget(showTrayIcon,row,0);
    mainLayout->addWidget(showSplashScreen, row++,1);
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        minimizeToTray = new QCheckBox(tr("Minimize to tray"));
        closeToTray = new QCheckBox(tr("Close to tray"));
        mainLayout->addWidget(minimizeToTray, row, 0);
        mainLayout->addWidget(closeToTray, row++, 1);
    }
    mainLayout->addWidget(autoHideEditorButtonbar, row, 0);
    mainLayout->addWidget(showPDFs, row++, 1);
    mainLayout->addWidget(showMissedReminders, row, 0);
    mainLayout->addWidget(dynamicTotals, row++, 1);
    mainLayout->addWidget(startMinimized, row, 0);
    mainLayout->addWidget(autoStart, row++, 1);
    mainLayout->addWidget(disableEditingOnStartup, row, 0);
    mainLayout->addWidget(newNoteFocusOnTitle, row++, 1);
    mainLayout->addWidget(confirmDeletes, row, 0);
    mainLayout->addWidget(forceWebFonts, row++, 1);
    mainLayout->addWidget(showNoteListGrid,row,0);
    mainLayout->addWidget(alternateNoteListColors,row++,1);
    mainLayout->addWidget(autosetUserid, row,0);
    mainLayout->addWidget(fontPreviewInDialog, row++, 1);

    mainLayout->addWidget(defaultNotebookOnStartupLabel,row,0);
    mainLayout->addWidget(defaultNotebookOnStartup, row++,1);

    mainLayout->addWidget(new QLabel(tr("Notification Service")), row, 0);
    mainLayout->addWidget(systemNotifier, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Middle Click Open Behavior")), row,0);
    mainLayout->addWidget(mouseMiddleClickAction, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Tray Icon Click Action")), row, 0);
    mainLayout->addWidget(traySingleClickAction, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Tray Icon Middle Click Action")), row, 0);
    mainLayout->addWidget(trayMiddleClickAction, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Tray Icon Double Click Action")), row, 0);
    mainLayout->addWidget(trayDoubleClickAction, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Default GUI Font*")), row, 0);
    mainLayout->addWidget(defaultGuiFontChooser, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Default GUI Font Size*")), row, 0);
    mainLayout->addWidget(defaultGuiFontSizeChooser, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Default Editor Font*")), row, 0);
    mainLayout->addWidget(defaultFontChooser, row++, 1);

    mainLayout->addWidget(new QLabel(tr("Default Editor Font Size*")), row, 0);
    mainLayout->addWidget(defaultFontSizeChooser, row++, 1);

    mainLayout->addWidget(new QLabel(""), row++, 0);
    mainLayout->addWidget(new QLabel(tr("* May require restart on some systems.")), row++, 0);
    mainLayout->addWidget(new QLabel(tr("** Can crash on Gnome systems.")), row++, 0);

    global.settings->beginGroup("Appearance");

    disableEditingOnStartup->setChecked(global.settings->value("disableEditingOnStartup",false).toBool());

    int idx  = global.settings->value("traySingleClickAction", 0).toInt();
    idx = traySingleClickAction->findData(idx, Qt::UserRole);
    traySingleClickAction->setCurrentIndex(idx);

    idx  = global.settings->value("trayMiddleClickAction", 0).toInt();
    idx = trayMiddleClickAction->findData(idx, Qt::UserRole);
    trayMiddleClickAction->setCurrentIndex(idx);

    idx  = global.settings->value("trayDoubleClickAction", 0).toInt();
    idx = trayDoubleClickAction->findData(idx, Qt::UserRole);
    trayDoubleClickAction->setCurrentIndex(idx);

    showTrayIcon->setChecked(global.settings->value("showTrayIcon", false).toBool());
    showPDFs->setChecked(global.settings->value("showPDFs", true).toBool());
    showSplashScreen->setChecked(global.settings->value("showSplashScreen", false).toBool());
    showMissedReminders->setChecked(global.settings->value("showMissedReminders", false).toBool());
    startMinimized->setChecked(global.settings->value("startMinimized", false).toBool());
    if (global.countBehavior == Global::CountAll)
        dynamicTotals->setChecked(true);
    else
        dynamicTotals->setChecked(false);
    autoStart->setChecked(global.settings->value("autoStart", false).toBool());
    int defaultNotebook = global.settings->value("startupNotebook", UseLastViewedNotebook).toInt();
    defaultNotebookOnStartup->setCurrentIndex(defaultNotebook);
    showNoteListGrid->setChecked(global.settings->value("showNoteListGrid", false).toBool());
    alternateNoteListColors->setChecked(global.settings->value("alternateNoteListColors", true).toBool());
    global.settings->endGroup();

    connect(showTrayIcon, SIGNAL(clicked(bool)), this, SLOT(showTrayIconChanged(bool)));

    if (minimizeToTray != NULL) {
        minimizeToTray->setChecked(global.minimizeToTray());
        if (!showTrayIcon->isChecked())
            minimizeToTray->setEnabled(false);
    }
    if (closeToTray != NULL) {
        closeToTray->setChecked(global.closeToTray());
        if (!showTrayIcon->isChecked())
            closeToTray->setEnabled(false);
    }

    newNoteFocusOnTitle->setChecked(global.newNoteFocusToTitle());

    this->setFont(global.getGuiFont(font()));

    // Check if notify-send is installed
    QProcess notifyProcess;
    notifyProcess.start("notify-send -?");
    notifyProcess.waitForFinished();
    if (notifyProcess.exitCode()) {
        systemNotifier->setEnabled(false);
    } else {
        QString notifier = global.systemNotifier();
        int idx = systemNotifier->findData(notifier, Qt::UserRole);
        systemNotifier->setCurrentIndex(idx);
    }
}






void AppearancePreferences::saveValues() {
    int index = systemNotifier->currentIndex();
    QString sysnotifier = systemNotifier->itemData(index, Qt::UserRole).toString();

    global.setNewNoteFocusToTitle(newNoteFocusOnTitle->isChecked());
    global.setDeleteConfirmation(this->confirmDeletes->isChecked());
    global.setAutosetUsername(autosetUserid->isChecked());
    if (!autosetUserid->isChecked())
        global.full_username="";
    global.settings->beginGroup("Appearance");
    global.settings->setValue("disableEditingOnStartup", disableEditingOnStartup->isChecked());
    global.settings->setValue("forceWebFonts", forceWebFonts->isChecked());
    global.settings->setValue("showTrayIcon", showTrayIcon->isChecked());
    global.settings->setValue("showPDFs", showPDFs->isChecked());
    global.autoHideEditorToolbar = this->autoHideEditorButtonbar->isChecked();
    global.settings->setValue("autoHideEditorToolbar", global.autoHideEditorToolbar);
    global.settings->setValue("mouseMiddleClickOpen", mouseMiddleClickAction->currentIndex());
    global.settings->setValue("trayDoubleClickAction", trayDoubleClickAction->currentIndex());
    global.settings->setValue("traySingleClickAction", traySingleClickAction->currentIndex());
    global.settings->setValue("trayMiddleClickAction", trayMiddleClickAction->currentIndex());
    global.settings->setValue("systemNotifier", sysnotifier);
//    global.settings->remove("trayDoubleClickAction");
    global.settings->setValue("showNoteListGrid", showNoteListGrid->isChecked());
    global.settings->setValue("alternateNoteListColors", alternateNoteListColors->isChecked());
    global.pdfPreview = showPDFs->isChecked();
    if (minimizeToTray!= NULL)
        global.settings->setValue("minimizeToTray", minimizeToTray->isChecked());
    else
        global.settings->remove("minimizeToTray");
    if (closeToTray != NULL)
        global.settings->setValue("closeToTray", closeToTray->isChecked());
    else
        global.settings->remove("closeToTray");
    global.settings->setValue("showSplashScreen", showSplashScreen->isChecked());
    global.settings->setValue("startMinimized", startMinimized->isChecked());
    global.settings->setValue("showMissedReminders", showMissedReminders->isChecked());
    if (dynamicTotals->isChecked()) {
        global.settings->setValue("countBehavior", 1);
        global.countBehavior = Global::CountAll;
    } else {
        global.settings->setValue("countBehavior", 2);
        global.countBehavior = Global::CountNone;
    }
    index = defaultNotebookOnStartup->currentIndex();
    int value = defaultNotebookOnStartup->itemData(index).toInt();
    global.settings->setValue("startupNotebook", value);

    //  Save default font & size
    if (webSettingsChanged) {
        int idx = defaultFontChooser->currentIndex();
        global.defaultFont = defaultFontChooser->itemData(idx, Qt::UserRole).toString();
        idx = defaultFontSizeChooser->currentIndex();
        global.defaultFontSize = defaultFontSizeChooser->itemData(idx, Qt::UserRole).toInt();
        idx = defaultGuiFontSizeChooser->currentIndex();
        global.defaultGuiFontSize = defaultGuiFontSizeChooser->itemData(idx, Qt::UserRole).toInt();
        idx = defaultGuiFontChooser->currentIndex();
        global.defaultGuiFont = defaultGuiFontChooser->itemData(idx, Qt::UserRole).toString();
        if (global.defaultGuiFont == "System Default")
            global.defaultGuiFont = "";
        if (global.defaultFont == "System Default")
            global.defaultFont = "";
        global.settings->setValue("defaultFont", global.defaultFont);
        global.settings->setValue("defaultFontSize", global.defaultFontSize);
        global.settings->setValue("defaultGuiFont", global.defaultGuiFont);
        global.settings->setValue("defaultGuiFontSize", global.defaultGuiFontSize);

        QWebSettings *settings = QWebSettings::globalSettings();
        settings->setFontFamily(QWebSettings::StandardFont, global.defaultFont);
        // QWebkit DPI is hard coded to 96. Hence, we calculate the correct
        // font size based on desktop logical DPI.
        if (global.defaultFontSize > 0) {
            settings->setFontSize(QWebSettings::DefaultFontSize, global.defaultFontSize * (QApplication::desktop()->logicalDpiX() / 96.0));
        }
    }


    // See if the user has overridden the window icon

        //Copy the nixnote2.desktop so we can override the app icon
        // Ideally, we could use QSettings since it is ini format, but
        // it puts [Desktop Entry] as [Desktop%20Enry], which screws
        // things up.
        QString systemFile = "/usr/share/applications/nixnote2.desktop";
        QFile systemIni(systemFile);
        QStringList desktopData;

        if (systemIni.open(QIODevice::ReadOnly)) {
            QTextStream data(&systemIni);
            QString line = data.readLine();
            while (!line.isNull()) {
                if (line.startsWith("Icon=")) {
                    line = "Icon=" +global.getResourceFileName(global.resourceList,":windowIcon.png");
                }
                desktopData.append(line);
                line = data.readLine();
            }
        }
        systemIni.close();

        // Now, write it back out
        QString userFile =  QDir::homePath()+"/.local/share/applications/nixnote2.desktop";
        QFile userIni(userFile);
        if (userIni.open(QIODevice::WriteOnly)) {
            QTextStream data(&userIni);
            for (int i=0; i<desktopData.size(); i++) {
                data << desktopData[i] << "\n";
            }
        }
        userIni.close();

//    }

    // Setup if the user wants to start NixNote the next time they login.
    global.settings->setValue("autoStart", autoStart->isChecked());
#ifdef _WIN32
    QFileInfo fileInfo(QCoreApplication::applicationFilePath());
    QFile::remove(QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation) + QDir::separator() + "Startup" + QDir::separator() + fileInfo.completeBaseName() + ".lnk");
#else
    QDir dir;
    QString startFile =  QDir::homePath()+"/.config/autostart/nixnote2.desktop";
    dir.remove(startFile);
#endif
    if (autoStart->isChecked()) {
#ifdef _WIN32
        QFile::link(QCoreApplication::applicationFilePath(), QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation) + QDir::separator() + "Startup" + QDir::separator() + fileInfo.completeBaseName() + ".lnk");
#else
        //Copy the nixnote2.desktop to the ~/.config/autostart directory
        QString systemFile = "/usr/share/applications/nixnote2.desktop";
        QFile systemIni(systemFile);
        QStringList desktopData;

        if (systemIni.open(QIODevice::ReadOnly)) {
            QTextStream data(&systemIni);
            QString line = data.readLine();
            while (!line.isNull()) {
                if (line.startsWith("Icon=")) {
                    line = "Icon=" +global.getResourceFileName(global.resourceList,":windowIcon.png");
                }
                desktopData.append(line);
                line = data.readLine();
            }
        }
        systemIni.close();

        // Now, write it back out
        QString userFile =  QDir::homePath()+"/.config/autostart/nixnote2.desktop";
        QFile userIni(userFile);
        if (userIni.open(QIODevice::WriteOnly)) {
            QTextStream data(&userIni);
            for (int i=0; i<desktopData.size(); i++) {
                data << desktopData[i] << "\n";
            }
            data << "X-GNOME-Autostart-enabled=true";
        }
        userIni.close();
#endif
    }

    global.settings->endGroup();

    global.setPreviewFontsInDialog(fontPreviewInDialog->isChecked());

}


// Load the list of font names
void AppearancePreferences::loadFontNames(QComboBox *combo, QString defaultFont) {
    QFontDatabase fonts;
    QStringList fontFamilies = fonts.families();
    combo->addItem(tr("System Default"), "System Default");
    for (int i = 0; i < fontFamilies.size(); i++) {
        combo->addItem(fontFamilies[i], fontFamilies[i]);
    }
    int idx = combo->findData(defaultFont, Qt::UserRole);
    if (idx >=0) {
        combo->setCurrentIndex(idx);
    } else
        combo->setCurrentIndex(0);
}



// Load the list of font sizes
void AppearancePreferences::loadGuiFontSizes(QString name) {
    webSettingsChanged =true;
    QFontDatabase fdb;
    defaultGuiFontSizeChooser->clear();
    defaultGuiFontSizeChooser->addItem(tr("System Default"), 0);
    QList<int> sizes = fdb.pointSizes(name);
    for (int i=0; i<sizes.size(); i++) {
        defaultGuiFontSizeChooser->addItem(QString::number(sizes[i]), sizes[i]);
    }
    int idx = defaultGuiFontSizeChooser->findData(global.defaultGuiFontSize, Qt::UserRole);
    if (idx >=0) {
        defaultGuiFontSizeChooser->setCurrentIndex(idx);
    } else
        defaultGuiFontSizeChooser->setCurrentIndex(0);
}



// Load the list of font sizes
void AppearancePreferences::loadFontSizes(QString name) {
    webSettingsChanged =true;
    QFontDatabase fdb;
    defaultFontSizeChooser->clear();
    defaultFontSizeChooser->addItem(tr("System Default"), 0);
    QList<int> sizes = fdb.pointSizes(name);
    for (int i=0; i<sizes.size(); i++) {
        defaultFontSizeChooser->addItem(QString::number(sizes[i]), sizes[i]);
    }
    int idx = defaultFontSizeChooser->findData(global.defaultFontSize, Qt::UserRole);
    if (idx >=0) {
        defaultFontSizeChooser->setCurrentIndex(idx);
    } else
        defaultFontSizeChooser->setCurrentIndex(0);
}


void AppearancePreferences::fontSizeChanged(QString name) {
    // suppress unused
    Q_UNUSED(name);

    webSettingsChanged = true;
}


void AppearancePreferences::showTrayIconChanged(bool value) {
    if (value) {
        minimizeToTray->setEnabled(true);
        closeToTray->setEnabled(true);
    } else {
        minimizeToTray->setChecked(false);
        closeToTray->setChecked(false);
        minimizeToTray->setEnabled(false);
        closeToTray->setEnabled(false);
    }
}

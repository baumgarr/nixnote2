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

#include "debugpreferences.h"
#include "logger/qslog.h"
#include "global.h"

extern Global global;

using namespace QsLogging;

DebugPreferences::DebugPreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(mainLayout);

    strictDTD = new QCheckBox(tr("Bypass strict note checking. *"),this);
    strictDTD->setChecked(!global.strictDTD);
    bypassTidy = new QCheckBox(tr("Bypass HTML Tidy. *"), this);
    bypassTidy->setChecked(global.bypassTidy);
    disableUploads = new QCheckBox(tr("Disable uploads to server."),this);
    disableImageHighlight = new QCheckBox(tr("Disable image search highlighting."), this);
    showLidColumn = new QCheckBox(tr("Show LID column (requires restart)."));
    nonAsciiSortBug = new QCheckBox(tr("Disable Tag Sorting (useful for non-ASCII sort bug)."));
    forceUTF8 = new QCheckBox(tr("Force UTF8 Encoding."));
    nonAsciiSortBug->setChecked(global.nonAsciiSortBug);
    forceUTF8->setChecked(global.getForceUTF8());
    global.settings->beginGroup("Debugging");
    disableUploads->setChecked(global.disableUploads);
    showLidColumn->setChecked(global.settings->value("showLids", false).toBool());
    global.settings->endGroup();
    disableImageHighlight->setChecked(global.disableImageHighlight());

    int row=0;
    mainLayout->addWidget(disableUploads,row++,1);
    mainLayout->addWidget(showLidColumn, row++,1);
    mainLayout->addWidget(nonAsciiSortBug,row++,1);
    mainLayout->addWidget(disableImageHighlight,row++,1);
    mainLayout->addWidget(strictDTD,row++,1);
    mainLayout->addWidget(bypassTidy, row++, 1);
    mainLayout->addWidget(forceUTF8, row++, 1);

#ifndef _WIN32
    interceptSigHup = new QCheckBox(tr("Intercept Unix SIGHUP (requires restart)."));
    interceptSigHup->setChecked(global.getInterceptSigHup());
    mainLayout->addWidget(interceptSigHup,row++,1);
#endif

    multiThreadSave = new QCheckBox(tr("Use multipe theads to save note contents (experimental)."));
    multiThreadSave->setChecked(global.getMultiThreadSave());
    mainLayout->addWidget(multiThreadSave,row++,1);

//    useLibTidy = new QCheckBox(tr("Use libtidy directly (experimental)."));
//    useLibTidy->setChecked(global.getUseLibTidy());
//    mainLayout->addWidget(useLibTidy,row++,1);

    mainLayout->addWidget(new QLabel(tr("Auto-Save Interval (in seconds).")), row,0);
    autoSaveInterval = new QSpinBox();
    autoSaveInterval->setMinimum(5);
    autoSaveInterval->setMaximum(300);
    autoSaveInterval->setValue(global.getAutoSaveInterval());
    mainLayout->addWidget(autoSaveInterval, row++,1);

    debugLevelLabel = new QLabel(tr("Message Level"), this);
    debugLevelLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    debugLevel = new QComboBox(this);
    debugLevel->addItem(tr("Trace"), TraceLevel);
    debugLevel->addItem(tr("Debug"), DebugLevel);
    debugLevel->addItem(tr("Info"), InfoLevel);
    debugLevel->addItem(tr("Warnings"), WarnLevel);
    debugLevel->addItem(tr("Errors"), ErrorLevel);
    debugLevel->addItem(tr("Fatal"), FatalLevel);

    global.settings->beginGroup("Debugging");
    int value = global.settings->value("messageLevel", InfoLevel).toInt();
    global.settings->endGroup();
    int index = debugLevel->findData(value);
    debugLevel->setCurrentIndex(index);
    mainLayout->addWidget(debugLevelLabel,row,0);
    mainLayout->addWidget(debugLevel,row++,1);
    mainLayout->addWidget(new QLabel(" "), row++, 0);
    mainLayout->addWidget(new QLabel(tr("* Note: Enabling can cause sync issues.")), row++, 0);
    this->setFont(global.getGuiFont(font()));

}


DebugPreferences::~DebugPreferences() {
    delete debugLevel;
    delete debugLevelLabel;
    delete mainLayout;
}


int DebugPreferences::getMessageLevel() {
    int index = debugLevel->currentIndex();
    return debugLevel->itemData(index).toInt();
}


void DebugPreferences::saveValues() {
    int value = getMessageLevel();

    global.setForceUTF8(forceUTF8->isChecked());
    global.settings->beginGroup("Debugging");
    global.settings->setValue("messageLevel", value);
    global.settings->setValue("showLids", showLidColumn->isChecked());
    global.settings->setValue("nonAsciiSortBug", nonAsciiSortBug->isChecked());
    global.settings->setValue("disableImageHighlight", disableImageHighlight->isChecked());
    global.nonAsciiSortBug = nonAsciiSortBug->isChecked();

    // If the disable uploads is different than the defaults or if it has changed, we save it.
    if (disableUploads->isChecked() || disableUploads->isChecked() != global.disableUploads)
        global.settings->setValue("disableUploads", disableUploads->isChecked());

    global.settings->endGroup();
    global.setAutoSaveInterval(autoSaveInterval->value());
    global.disableUploads = disableUploads->isChecked();
    global.setStrictDTD(!strictDTD->isChecked());
    global.setBypassTidy(bypassTidy->isChecked());

#ifndef _WIN32
    global.setInterceptSigHup(interceptSigHup->isChecked());
#endif

//    global.setMultiThreadSave(multiThreadSave->isChecked());
//    global.setUseLibTidy(useLibTidy->isChecked());
}

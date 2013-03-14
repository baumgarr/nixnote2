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

    disableUploads = new QCheckBox(tr("Disable uploads to server"),this);
    global.settings->beginGroup("Debugging");
    disableUploads->setChecked(global.disableUploads);
    global.settings->endGroup();

    mainLayout->addWidget(disableUploads,0,1);

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
    mainLayout->addWidget(debugLevelLabel,1,0);
    mainLayout->addWidget(debugLevel,1,1);

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

    global.settings->beginGroup("Debugging");
    global.settings->setValue("messageLevel", value);

    // If the disable uploads is different than the defaults or if it has changed, we save it.
    if (!disableUploads->isChecked() || disableUploads->isChecked() != global.disableUploads)
        global.settings->setValue("disableUploads", disableUploads->isChecked());

    global.settings->endGroup();
    global.disableUploads = disableUploads->isChecked();
}

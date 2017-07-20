/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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

#include <QFrame>
#include <QSpacerItem>
#include "exitpreferences.h"
#include "global.h"

extern Global global;

ExitPreferences::ExitPreferences(QWidget *parent) : QWidget(parent)
{
    this->setFont(global.getGuiFont(font()));

    loadExitFile = new QLineEdit();
    loadExitButton = new QPushButton();
    loadExitButton->setText("...");
    loadExitFileLabel = new QLabel();
    loadExitFileLabel->setText(tr("File Name"));
    connect(loadExitButton, SIGNAL(clicked(bool)), this, SLOT(loadExitButtonPressed(bool)));

    loadExitFileLabel = new QLabel();
    loadExitFileLabel->setText(tr("File Name"));

    loadExitEnabledCombo = new QComboBox();
    loadExitEnabledCombo->addItem(tr("Enabled"), true);
    loadExitEnabledCombo->addItem(tr("Disabled"), false);

    loadExitEnabledLabel = new QLabel();
    loadExitEnabledLabel->setText(tr("Status"));


    saveExitFile = new QLineEdit();
    saveExitButton = new QPushButton();
    saveExitButton->setText("...");
    saveExitFileLabel = new QLabel();
    saveExitFileLabel->setText(tr("File Name"));
    connect(saveExitButton, SIGNAL(clicked(bool)), this, SLOT(saveExitButtonPressed(bool)));


    saveExitFileLabel = new QLabel();
    saveExitFileLabel->setText(tr("File Name"));

    saveExitEnabledCombo = new QComboBox();
    saveExitEnabledCombo->addItem(tr("Enabled"), true);
    saveExitEnabledCombo->addItem(tr("Disabled"), false);

    saveExitEnabledLabel = new QLabel();
    saveExitEnabledLabel->setText(tr("Status"));


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QGridLayout *loadLayout = new QGridLayout();

    int row=0;
    mainLayout->addLayout(loadLayout);
    loadLayout->addWidget(new QLabel(tr("<b>Note Load Exit</b>")),row++,0);
    loadLayout->addWidget(loadExitFileLabel, row,0);
    loadLayout->addWidget(loadExitFile, row,1);
    loadLayout->addWidget(loadExitButton, row++,2);

    loadLayout->addWidget(loadExitEnabledLabel, row,0);
    loadLayout->addWidget(loadExitEnabledCombo, row++,1);

    QFrame *hFrame = new QFrame();
    hFrame->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(hFrame);

    mainLayout->addSpacerItem(new QSpacerItem(30,30));

    QGridLayout *saveLayout = new QGridLayout();
    mainLayout->addLayout(saveLayout);
    saveLayout->addWidget(new QLabel(tr("<b>Note Save Exit</b>")),row++,0);
    saveLayout->addWidget(saveExitFileLabel, row,0);
    saveLayout->addWidget(saveExitFile, row,1);
    saveLayout->addWidget(saveExitButton, row++,2);

    saveLayout->addWidget(saveExitEnabledLabel, row,0);
    saveLayout->addWidget(saveExitEnabledCombo, row++,1);

    loadValues();
}



void ExitPreferences::loadExitButtonPressed(bool value) {
    Q_UNUSED(value);
    QFileDialog f;
    loadExitFile->setText(f.getOpenFileName(this, tr("Select Exit File")));

}



void ExitPreferences::saveExitButtonPressed(bool value) {\
    Q_UNUSED(value);
    QFileDialog f;
    saveExitFile->setText(f.getOpenFileName(this, tr("Select Exit File")));

}



void ExitPreferences::saveValues() {

    global.settings->beginGroup("ExitPoint_LoadNote");
    global.settings->setValue("enabled", loadExitEnabledCombo->currentData().toBool());;
    global.settings->setValue("script", loadExitFile->text());
    global.settings->setValue("version", "1");
    global.settings->endGroup();

    global.settings->beginGroup("ExitPoint_SaveNote");
    global.settings->setValue("enabled", saveExitEnabledCombo->currentData().toBool());;
    global.settings->setValue("script", saveExitFile->text());
    global.settings->setValue("version", "1");
    global.settings->endGroup();
}




void ExitPreferences::loadValues() {

    QString file="";
    bool enabled = false;

    global.settings->beginGroup("ExitPoint_LoadNote");
    enabled = global.settings->value("enabled", false).toBool();
    file = global.settings->value("script", "").toString();
    if (enabled)
        loadExitEnabledCombo->setCurrentIndex(0);
    else
        loadExitEnabledCombo->setCurrentIndex(1);
    loadExitFile->setText(file);
    global.settings->endGroup();


    global.settings->beginGroup("ExitPoint_SaveNote");
    enabled = global.settings->value("enabled", false).toBool();
    file = global.settings->value("script", "").toString();
    if (enabled)
        saveExitEnabledCombo->setCurrentIndex(0);
    else
        saveExitEnabledCombo->setCurrentIndex(1);
    saveExitFile->setText(file);
    global.settings->endGroup();

}

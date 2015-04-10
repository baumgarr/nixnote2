/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#include "searchpreferences.h"
#include "global.h"
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>

extern Global global;

SearchPreferences::SearchPreferences(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    syncAttachments = new QCheckBox(tr("Index Attachments"));
    mainLayout->addWidget(syncAttachments,0,0);
    syncAttachments->setChecked(global.synchronizeAttachments());

    weight = new QSpinBox(this);
    mainLayout->addWidget(new QLabel(tr("Minimum Image Recognition Weight")), 1,0);
    mainLayout->addWidget(weight,1,1);
    weight->setMinimum(1);
    weight->setMaximum(100);
    weight->setValue(global.getMinimumRecognitionWeight());
    this->setFont(global.getGuiFont(font()));

}




void SearchPreferences::saveValues() {
    global.setMinimumRecognitionWeight(weight->value());
    global.setSynchronizeAttachments(syncAttachments->isChecked());
}

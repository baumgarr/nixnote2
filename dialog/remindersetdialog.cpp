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


#include "remindersetdialog.h"
#include "global.h"

extern Global global;


ReminderSetDialog::ReminderSetDialog(QWidget *parent) :
    QDialog(parent)
{


    time = new QTimeEdit(this);
    calendar = new QCalendarWidget(this);
    ok = new QPushButton(this);
    cancel = new QPushButton(this);

    this->setWindowIcon(QIcon(":alarmclock.png"));
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    setLayout(vboxLayout);
    vboxLayout->addWidget(time);
    vboxLayout->addWidget(calendar);

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->addWidget(ok);
    hboxLayout->addWidget(cancel);
    vboxLayout->addLayout(hboxLayout);
    ok->setText(tr("OK"));
    cancel->setText(tr("Cancel"));

    connect(ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    this->setFont(global.getGuiFont(font()));
}


void ReminderSetDialog::okButtonPressed() {
    okPressed = true;
    close();
}

void ReminderSetDialog::cancelButtonPressed() {
    okPressed = false;
    close();
}

/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2016 Randy Baumgarte

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


#include "noteproperties.h"
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>


NoteProperties::NoteProperties()
{
    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(20);
    tableWidget->setColumnCount(2);
    tableWidget->verticalHeader()->hide();
    tableWidget->horizontalHeader()->hide();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    okButton = new QPushButton(this);
    this->setWindowTitle(tr("Note Properties"));
    okButton->setText(tr("Ok"));
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tableWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QSpacerItem *spacer1 = new QSpacerItem(50,1, QSizePolicy::Maximum);
    QSpacerItem *spacer2 = new QSpacerItem(50,1, QSizePolicy::Maximum);
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacerItem(spacer2);

    layout->addLayout(buttonLayout);
    setLayout(layout);
    connect(okButton, SIGNAL(pressed()), this, SLOT(close()));
}

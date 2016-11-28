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

#include "shortcutdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QHash>
#include <QHashIterator>

#include "global.h"

extern Global global;

ShortcutDialog::ShortcutDialog()
{
    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(global.shortcutKeys->actionMap->size());
    tableWidget->setColumnCount(2);
    tableWidget->verticalHeader()->hide();
    tableWidget->horizontalHeader()->hide();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    okButton = new QPushButton(this);
    this->setWindowTitle(tr("Defined Shortcuts"));
    okButton->setText(tr("Ok"));
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tableWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QSpacerItem *spacer1 = new QSpacerItem(150,1, QSizePolicy::Maximum);
    QSpacerItem *spacer2 = new QSpacerItem(150,1, QSizePolicy::Maximum);
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacerItem(spacer2);

    layout->addLayout(buttonLayout);
    setLayout(layout);
    connect(okButton, SIGNAL(pressed()), this, SLOT(close()));

    ShortcutKeys *keys = global.shortcutKeys;
    QHashIterator<QString, QString> i(*keys->actionMap);
    int row = 0;
    while (i.hasNext()) {
        i.next();
        tableWidget->setItem(row, 0, new QTableWidgetItem(i.key().toUpper()));
        QString sc = i.value().toUpper();
        sc = sc.replace("SPACE", "Space");
        sc = sc.replace("CTRL", "Ctrl");
        sc = sc.replace("ALT", "Alt");
        sc = sc.replace("SHIFT", "Shift");
        sc = sc.replace("PGUP", "PgUp");
        sc = sc.replace("PGDOWN", "PgDown");
        tableWidget->setItem(row++, 1, new QTableWidgetItem(sc));
    }
    tableWidget->sortByColumn(0, Qt::AscendingOrder);

    tableWidget->resizeColumnsToContents();
    //this->resize(size().width(), size().height()+30);


}

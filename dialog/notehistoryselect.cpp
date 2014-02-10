/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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


#include "notehistoryselect.h"
#include <QDateTime>
#include "global.h"


extern Global global;

NoteHistorySelect::NoteHistorySelect(QWidget *parent) :
    QDialog(parent)
{
    importPressed = false;
    this->setWindowTitle(tr("Note History"));
    cancelButton.setText(tr("Cancel"));
    importButton.setText(tr("Import"));
    importButton.setEnabled(false);

    connect(&importButton, SIGNAL(clicked()), this, SLOT(importButtonPressed()));
    connect(&cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    connect(&list, SIGNAL(itemSelectionChanged()), this, SLOT(enableImport()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *entryLayout = new QHBoxLayout(this);
    entryLayout->addWidget(&list);
    entryLayout->addWidget(&importButton);

    mainLayout->addLayout(entryLayout);
    this->setLayout(mainLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(&importButton);
    buttonLayout->addWidget(&cancelButton);
    mainLayout->addLayout(buttonLayout);

}


void NoteHistorySelect::importButtonPressed() {
    this->importPressed = true;
    this->close();
}


void NoteHistorySelect::cancelButtonPressed() {
    this->importPressed = false;
    this->close();
}


void NoteHistorySelect::loadData(vector<NoteVersionId> &versions) {

    this->versions = &versions;
    for (unsigned int i=0; i<versions.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(&list);
        item->setData(Qt::UserRole, versions.at(i).updateSequenceNum);
        QString text;

        QDateTime timestamp;
        timestamp.setTime_t(versions.at(i).saved/1000);
        if (timestamp.date() == QDate::currentDate())
            text = tr("Today") +" " + timestamp.time().toString(Qt::SystemLocaleShortDate);
        if (timestamp.date() == QDate::currentDate().addDays(-1))
            text = tr("Yesterday") +" " + timestamp.time().toString(Qt::SystemLocaleShortDate);
        text = timestamp.toString(global.dateFormat + " " +global.timeFormat);

        text = text + " : "+ QString::fromStdString(versions.at(i).title);
        item->setText(text);
        list.addItem(item);
    }
}


void NoteHistorySelect::enableImport() {
    importButton.setEnabled(true);
    usn = this->list.selectedItems().at(0)->data(Qt::UserRole).toInt();
    QLOG_DEBUG() << usn;
}

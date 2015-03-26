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

#include "databasestatus.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "sql/notetable.h"
#include "sql/resourcetable.h"
#include "global.h"

extern Global global;

DatabaseStatus::DatabaseStatus(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    // Setup window layout, title, & icon
    this->setWindowTitle(tr("Database Status"));
    QGridLayout *textGrid = new QGridLayout();
    setLayout(vBoxLayout);

    NoteTable ntable(global.db);
    ResourceTable rtable(global.db);
    qint32 totalNotes = ntable.getCount();
    qint32 totalResources = rtable.getCount();
    qint32 unindexedNotes = ntable.getUnindexedCount();
    qint32 unindexedResources = rtable.getUnindexedCount();
    qint32 thumbnailsNeeded = ntable.getThumbnailsNeededCount();

    textGrid->addWidget(new QLabel(tr("Total Notes:")),1,1);
    textGrid->addWidget(new QLabel(QString::number(totalNotes)), 1,2);
    textGrid->addWidget(new QLabel(tr("Total Resources:")), 2,1);
    textGrid->addWidget(new QLabel(QString::number(totalResources)), 2, 2);
    textGrid->addWidget(new QLabel(tr("Unindexed Notes:")), 3,1);
    textGrid->addWidget(new QLabel(QString::number(unindexedNotes)),3,2);
    textGrid->addWidget(new QLabel(tr("Unindexed Resources:")), 4,1);
    textGrid->addWidget(new QLabel(QString::number(unindexedResources)),4,2);
    textGrid->addWidget(new QLabel(tr("Thumbnails Needed:")), 5,1);
    textGrid->addWidget(new QLabel(QString::number(thumbnailsNeeded)),5,2);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    ok = new QPushButton(tr("OK"),this);
    connect(ok, SIGNAL(clicked()), this, SLOT(okPushed()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(ok);
    buttonLayout->addStretch();

    vBoxLayout->addLayout(textGrid);
    vBoxLayout->addLayout(buttonLayout);
    this->setFont(global.getGuiFont(font()));
}



// OK button pushed, close the window
void DatabaseStatus::okPushed() {
    this->close();
}

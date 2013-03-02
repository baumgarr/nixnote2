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

#include "tagproperties.h"

#include "global.h"
#include "sql/tagtable.h"

#include <QString>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>

extern Global global;

TagProperties::TagProperties(QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("Tag"));
    setWindowIcon(QIcon(":tag.png"));
    setLayout(&grid);

    connect(&name, SIGNAL(textChanged(const QString&)), this, SLOT(validateInput()));

    nameLabel.setText(tr("Name"));
    queryGrid.addWidget(&nameLabel, 1,1);
    queryGrid.addWidget(&name, 1, 2);
//    queryGrid.setContentsMargins(10, 10,  -10, -10);
    grid.addLayout(&queryGrid,1,1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    cancel.setText(tr("Cancel"));
    connect(&cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    buttonGrid.addWidget(&ok, 1, 1);
    buttonGrid.addWidget(&cancel, 1,2);
    grid.addLayout(&buttonGrid,2,1);
}



void TagProperties::okButtonPressed() {
    okPressed = true;

    if (this->lid > 0) {
        Tag tag;
        TagTable table;
        table.get(tag, lid);
        tag.name = this->name.text().trimmed().toStdString();
        tag.__isset.name = true;
        table.update(tag, true);
        close();
        return;
    }

    // We have a new tag to add
    Tag tag;
    tag.name = name.text().trimmed().toStdString();
    QUuid uuid;
    QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
    tag.guid = g.toStdString();
    tag.__isset.name = true;
    tag.__isset.guid = true;
    TagTable t;
    t.add(0,tag,true, account);
    close();
}

void TagProperties::cancelButtonPressed() {
    okPressed = false;
    close();
}

void TagProperties::setLid(qint32 lid) {
    account = 0;
    if (lid > 0) {
        this->lid = lid;
        Tag tag;
        TagTable table;
        table.get(tag, lid);
        originalName = QString::fromStdString(tag.name).trimmed();
        name.setText(originalName);
        account = table.owningAccount(lid);
        return;
    }
    this->lid = 0;
    this->setWindowTitle(tr("Add Tag"));
    originalName = "";
}

void TagProperties::validateInput() {
    if (account > 0)  {
        ok.setEnabled(false);
        return;
    }
    ok.setEnabled(true);
    if (name.text()=="") {
        ok.setEnabled(false);
        return;
    }
    TagTable t;
    QString tag = name.text().trimmed();
    qint32 x = t.findByName(tag, account);
    if (t.findByName(tag, account)>0 || name.text().trimmed() == originalName) {
        ok.setEnabled(false);
        return;
    }
}

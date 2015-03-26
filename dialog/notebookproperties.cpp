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

#include "notebookproperties.h"

#include "global.h"
#include "sql/notebooktable.h"

#include <QString>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>

extern Global global;

NotebookProperties::NotebookProperties(QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("Notebook"));
    setWindowIcon(QIcon(":notebook_small.png"));
    setLayout(&grid);

    syncBox.setText(tr("Synchronized"));
    syncBox.setChecked(true);
    syncBox.setEnabled(false);

    connect(&name, SIGNAL(textChanged(const QString&)), this, SLOT(validateInput()));

    nameLabel.setText(tr("Name"));
    queryGrid.addWidget(&nameLabel, 1,1);
    queryGrid.addWidget(&name, 1, 2);
    queryGrid.addWidget(&syncBox, 2,2);
//    queryGrid.setContentsMargins(10, 10,  -10, -10);
    grid.addLayout(&queryGrid,1,1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    cancel.setText(tr("Cancel"));
    connect(&cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    buttonGrid.addWidget(&ok, 1, 1);
    buttonGrid.addWidget(&cancel, 1,2);
    grid.addLayout(&buttonGrid,2,1);
    this->setFont(global.getGuiFont(font()));
}



void NotebookProperties::okButtonPressed() {
    okPressed = true;

    if (this->lid > 0) {
        Notebook book;
        NotebookTable table(global.db);
        table.get(book, lid);
        book.name = name.text().trimmed();
        table.update(book, true);
        close();
        return;
    }

    // We have a new notebook to add
    Notebook book;
    book.name = name.text().trimmed();
    bool isSynchronized = syncBox.isChecked();
    QUuid uuid;
    QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
    book.guid = g;
    NotebookTable t(global.db);
    t.add(0,book,true, !isSynchronized);
    close();
}

void NotebookProperties::cancelButtonPressed() {
    okPressed = false;
    close();
}

void NotebookProperties::setLid(qint32 lid) {
    if (lid > 0) {
        this->lid = lid;
        Notebook book;
        NotebookTable table(global.db);
        table.get(book, lid);
        originalName = book.name;
        name.setText(originalName.trimmed());
        syncBox.setEnabled(false);
        bool local = table.isLocal(lid);
        syncBox.setChecked(!local);
        return;
    }
    this->lid = 0;
    syncBox.setEnabled(true);
    this->setWindowTitle(tr("Add Notebook"));
    originalName = "";
}

void NotebookProperties::validateInput() {
    ok.setEnabled(true);
    if (name.text()=="") {
        ok.setEnabled(false);
        return;
    }
    NotebookTable t(global.db);
    QString notebook = name.text().trimmed();
    if (t.findByName(notebook)>0 && name.text().trimmed() != originalName) {
        ok.setEnabled(false);
        return;
    }
}

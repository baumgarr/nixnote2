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


// Constructor. Thys happens when the class is declared.
NotebookProperties::NotebookProperties(QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("Notebook"));
    setWindowIcon(global.getIconResource(":notebookSmallIcon"));
    setLayout(&grid);

    syncBox.setText(tr("Synchronized"));
    syncBox.setChecked(true);
    syncBox.setEnabled(false);

    defaultNotebook.setText(tr("Default"));
    defaultNotebook.setChecked(false);
    defaultNotebook.setEnabled(true);

    connect(&name, SIGNAL(textChanged(const QString&)), this, SLOT(validateInput()));

    nameLabel.setText(tr("Name"));
    queryGrid.addWidget(&nameLabel, 1,1);
    queryGrid.addWidget(&name, 1, 2);
    queryGrid.addWidget(&syncBox, 2,2);
    queryGrid.addWidget(&defaultNotebook, 3,2);
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


// This happens when the Ok button is pressed.
void NotebookProperties::okButtonPressed() {
    okPressed = true;
    bool isDefault = defaultNotebook.isChecked();

    if (this->lid > 0) {
        Notebook book;
        NotebookTable table(global.db);
        table.get(book, lid);
        book.name = name.text().trimmed();
        book.defaultNotebook = isDefault;
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
    book.defaultNotebook = isDefault;
    t.add(0,book,true, !isSynchronized);
    close();
}




// This happens when the cancel button is pressed.
void NotebookProperties::cancelButtonPressed() {
    okPressed = false;
    close();
}


// This is usually called after the constructor and will set up
// the notebook name (if it already exists). If the lid > 0 then
// we are editing an existing notebook. If it is 0 then this is a
// new notebook.
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
        qint32 defaultLid = table.getDefaultNotebookLid();
        if (defaultLid == lid) {
            defaultNotebook.setEnabled(false);
            defaultNotebook.setChecked(true);
        }
        return;
    }
    this->lid = 0;
    syncBox.setEnabled(true);
    this->setWindowTitle(tr("Add Notebook"));
    originalName = "";
}





// This is called when the notebook name is changed. We check that
// the name isn't blank and there isn't already a notebook by that
// name.
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

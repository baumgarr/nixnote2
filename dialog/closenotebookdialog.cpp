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


#include "closenotebookdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QList>

#include "sql/notebooktable.h"
#include "global.h"

extern Global global;


// Constructor
CloseNotebookDialog::CloseNotebookDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("Open/Close Notebooks"));
    this->setWindowIcon(QIcon(":notebook_small.png"));
    okPressed = false;
    openButton.setEnabled(false);
    closeButton.setEnabled(false);
    okButton.setText(tr("OK"));
    cancelButton.setText(tr("Cancel"));
    connect(&openNotebooks, SIGNAL(itemSelectionChanged()), this, SLOT(openListSelection()));
    connect(&closeNotebooks, SIGNAL(itemSelectionChanged()), this, SLOT(closeListSelection()));
    connect(&okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    connect(&cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    connect(&closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    connect(&openButton, SIGNAL(clicked()), this, SLOT(openButtonClicked()));

    openNotebooks.setSelectionMode(QAbstractItemView::MultiSelection);
    closeNotebooks.setSelectionMode(QAbstractItemView::MultiSelection);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *topLayout = new QGridLayout(this);

    openLabel.setText(tr("Open Notebooks"));
    closeLabel.setText(tr("Closed Notebooks"));

    openButton.setText(tr("Open"));
    closeButton.setText(tr("Close"));

    topLayout->addWidget(&openLabel, 0,0);
    topLayout->addWidget(&openNotebooks, 1,0);

    QVBoxLayout *middleButtonLayout =new QVBoxLayout(this);
    middleButtonLayout->addWidget(&openButton);
    middleButtonLayout->addWidget(&closeButton);
    topLayout->addLayout(middleButtonLayout, 1,1);

    topLayout->addWidget(&closeLabel, 0,2);
    topLayout->addWidget(&closeNotebooks,1,2);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addSpacerItem(new QSpacerItem(100,1));
    buttonLayout->addWidget(&okButton);
    buttonLayout->addSpacerItem(new QSpacerItem(100,1));
    buttonLayout->addWidget(&cancelButton);
    buttonLayout->addSpacerItem(new QSpacerItem(100,1));

    this->setLayout(mainLayout);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);

    NotebookTable bookTable(global.db);
    QList<qint32> openLids;
    QList<qint32> closedLids;
    bookTable.getOpenNotebooks(openLids);
    bookTable.getClosedNotebooks(closedLids);

    for (int i=0; i<openLids.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(&openNotebooks);
        Notebook notebook;
        bookTable.get(notebook, openLids[i]);
        item->setData(Qt::UserRole, openLids[i]);
        item->setText(notebook.name);
        openNotebooks.addItem(item);
    }
    openNotebooks.sortItems();

    for (int i=0; i<closedLids.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(&closeNotebooks);
        Notebook notebook;
        bookTable.get(notebook, closedLids[i]);
        item->setData(Qt::UserRole, closedLids[i]);
        item->setText(notebook.name);
        closeNotebooks.addItem(item);
    }
    closeNotebooks.sortItems();
}



// OK button was pressed.  Mark specified notebooks
// as closed.
void CloseNotebookDialog::okButtonPressed() {
    NotebookTable bookTable(global.db);
    bookTable.openAllNotebooks();
    for (int i=0; i<closeNotebooks.count(); i++) {
        QListWidgetItem *item = closeNotebooks.item(i);
        qint32 lid = item->data(Qt::UserRole).toInt();
        bookTable.closeNotebook(lid);
    }
    okPressed = true;
    this->close();
}



// Cancel was pressed.  Close the dialog & go home.
void CloseNotebookDialog::cancelButtonPressed() {
    okPressed = false;
    this->close();
}


// The "open" button was pressed.  Move the notebook
// to the open list.
void CloseNotebookDialog::openButtonClicked() {
    QList< QListWidgetItem *> items = closeNotebooks.selectedItems();
    for (int i=items.size()-1; i>=0; i--) {
        QListWidgetItem *newItem = items[i]->clone();
        openNotebooks.addItem(newItem);
        delete items[i];
    }
    openNotebooks.sortItems();
    if (closeNotebooks.count() == 0)
        openButton.setEnabled(false);
}


// The "closed" button was pressed.  Move the notebook
// to the closed list.
void CloseNotebookDialog::closeButtonClicked() {
    QList< QListWidgetItem *> items = openNotebooks.selectedItems();
    for (int i=items.size()-1; i>=0; i--) {
        QListWidgetItem *newItem = items[i]->clone();
        closeNotebooks.addItem(newItem);
        delete items[i];
    }
    closeNotebooks.sortItems();

    if (openNotebooks.count() <= 1)
        closeButton.setEnabled(false);
}



// An item in the open list was selected.  We enable
// the close button if an item was selected AND if
// there is more than one item in the open list that is not
// selected.  You  must always have at least one notebook open.
void CloseNotebookDialog::openListSelection() {
    closeButton.setEnabled(true);
    if (openNotebooks.count() <= 1)
        closeButton.setEnabled(false);
    if (openNotebooks.selectedItems().size() == 0)
        closeButton.setEnabled(false);
    if (openNotebooks.selectedItems().size() == openNotebooks.count())
        closeButton.setEnabled(false);
}



// An item in the closed list was either selected
// or deselected.  If an item is selected we enable
// the open button.  If nothing is selected we disable
// the open button.
void CloseNotebookDialog::closeListSelection() {
    openButton.setEnabled(true);
    if (closeNotebooks.count() <= 0)
        openButton.setEnabled(false);
    if (closeNotebooks.selectedItems().size() == 0)
        openButton.setEnabled(false);
}

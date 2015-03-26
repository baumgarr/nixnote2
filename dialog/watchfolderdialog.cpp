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

#include "watchfolderdialog.h"
#include "sql/filewatchertable.h"
#include "sql/notebooktable.h"
#include "dialog/watchfolderadd.h"

#include <QFontMetrics>
#include <QApplication>
#include <QTableWidgetItem>
#include <QHeaderView>

WatchFolderDialog::WatchFolderDialog(QWidget *parent) :
    QDialog(parent)
{
    okClicked = false;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    okButton = new QPushButton(this);
    okButton->setText(tr("OK"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onClicked()));

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    setWindowTitle(tr("Auto Import Folders"));


    QList<qint32> lids;
    FileWatcherTable ft(global.db);
    ft.getAll(lids);
    table = new QTableWidget(lids.size(),4);
    connect(table, SIGNAL(itemSelectionChanged()), this, SLOT(tableSelection()));
    horizontalLayout->addWidget(table);


    addButton = new QPushButton(this);
    addButton->setText(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addPressed()));

    editButton = new QPushButton(this);
    editButton->setText(tr("Edit"));
    editButton->setEnabled(false);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editPressed()));

    deleteButton = new QPushButton(this);
    deleteButton->setText(tr("Delete"));
    deleteButton->setEnabled(false);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deletePressed()));

    QVBoxLayout *editLayout = new QVBoxLayout();
    editLayout->addWidget(addButton);
    editLayout->addWidget(editButton);
    editLayout->addWidget(deleteButton);

    QHBoxLayout *listLayout = new QHBoxLayout();
    listLayout->addLayout(horizontalLayout);
    listLayout->addLayout(editLayout);

    mainLayout->addLayout(listLayout);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayout);

    //table->setColumnWidth(0, 160);
    //resize(500, 200);
    //table->resizeColumnsToContents();
    load();
    this->setFont(global.getGuiFont(font()));
}




void WatchFolderDialog::onClicked() {
    okClicked = true;
    close();
}


void WatchFolderDialog::onCancel() {
    okClicked = false;
    close();
}


void WatchFolderDialog::load() {
    QList<QString> headers;
    headers.append(tr("Directory"));
    headers.append(tr("Target Notebook"));
    headers.append(tr("Keep"));
    headers.append(tr("Include Subdirectories"));
    table->setHorizontalHeaderLabels(headers);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->verticalHeader()->hide();

    QList<qint32> lids;
    FileWatcherTable ft(global.db);
    ft.getAll(lids);
    QString dir;
    qint32 notebookLid;
    bool includeSubdirs;
    FileWatcher::ScanType type;

    for (int i=0; i<lids.size(); i++) {
        ft.get(lids[i], dir, type, notebookLid, includeSubdirs);
        addRow(lids[i], i, dir, notebookLid, type, includeSubdirs);
    }
    table->resizeColumnsToContents();
}

void WatchFolderDialog::addRow(qint32 lid, int row, QString folder, qint32 notebookLid, FileWatcher::ScanType type, bool includeSubdirs) {
    QFontMetrics f = QApplication::fontMetrics();
    int fontHeight = f.height();

    QTableWidgetItem *dir = new QTableWidgetItem();
    dir->setText(folder);
    dir->setData(Qt::UserRole, lid);
    table->setItem(row, 0, dir);
    table->setRowHeight(row, fontHeight);
    dir->setToolTip(folder);

    NotebookTable bookTable(global.db);
    Notebook n;
    bookTable.get(n, notebookLid);

    QTableWidgetItem *book = new QTableWidgetItem();
    book->setText(n.name);
    book->setData(Qt::UserRole, notebookLid);
    table->setItem(row, 1, book);

    QTableWidgetItem *keep = new QTableWidgetItem();
    if (type == FileWatcher::ImportKeep) {
        keep->setText(tr("Keep"));
        keep->setData(Qt::UserRole, FileWatcher::ImportKeep);
    } else {
        keep->setText(tr("Delete"));
        keep->setData(Qt::UserRole, FileWatcher::ImportDelete);
    }
    table->setItem(row, 2, keep);

    QTableWidgetItem *subdirs = new QTableWidgetItem();
    if (includeSubdirs) {
        subdirs->setText(tr("Yes"));
        subdirs->setData(Qt::UserRole, true);
    } else {
        subdirs->setText(tr("No"));
        subdirs->setData(Qt::UserRole, false);
    }
    table->setItem(row, 3, subdirs);

}


void WatchFolderDialog::tableSelection() {
    editButton->setEnabled(true);
    deleteButton->setEnabled(true);
}


void WatchFolderDialog::addPressed() {
    WatchFolderAdd dialog(0, this);
    dialog.exec();
    if (dialog.okClicked) {
        QString dir = dialog.directory->text();
        qint32 notebook = dialog.books->itemData(dialog.books->currentIndex()).toInt();
        qint32 lid = dialog.lid;

        FileWatcher::ScanType keep;
        int index = dialog.keep->currentIndex();
        QString keepString  = dialog.keep->itemData(index).toString();
        if (keepString == "keep")
            keep = FileWatcher::ImportKeep;
        else
            keep = FileWatcher::ImportDelete;
        bool includeSubdirs = dialog.subdirs->isChecked();
        table->insertRow(table->rowCount());
        addRow(lid, table->rowCount()-1, dir, notebook, keep, includeSubdirs);
    }
}



void WatchFolderDialog::editPressed() {
    QModelIndex index = table->currentIndex();
    int row = index.row();
    QTableWidgetItem *item = table->item(row, 0);
    qint32 lid = item->data(Qt::UserRole).toInt();

    WatchFolderAdd dialog(lid, this);
    dialog.exec();
    if (dialog.okClicked) {
       table->reset();
       load();
       table->resizeColumnsToContents();
    }
}



void WatchFolderDialog::deletePressed() {
    QModelIndex index = table->currentIndex();
    int row = index.row();

    QTableWidgetItem *dirWidget = table->item(row, 0);
    qint32 value = dirWidget->data(Qt::UserRole).toInt();
    FileWatcherTable ft(global.db);
    ft.expunge(value);
    table->removeRow(row);

    if (table->rowCount() == 0) {
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
    }
}

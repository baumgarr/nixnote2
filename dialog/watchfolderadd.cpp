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

#include "watchfolderadd.h"
#include "sql/filewatchertable.h"
#include "sql/notebooktable.h"
#include "watcher/filewatcher.h"
#include "sql/configstore.h"

#include <QFileDialog>
#include <QDir>

WatchFolderAdd::WatchFolderAdd(qint32 lid, QWidget *parent) :
    QDialog(parent)
{
    mainLayout = new QVBoxLayout();
    okClicked = false;
    fileDialog = new QFileDialog(this);
    QString dir = QDir::home().absolutePath();
    bool includeSubdirs = false;
    qint32 notebookLid = 0;
    FileWatcher::ScanType type = FileWatcher::ImportKeep;
    this->lid = lid;
    if (lid > 0) {
        FileWatcherTable ft;
        ft.get(lid, dir, type, notebookLid, includeSubdirs);
    }


    okButton = new QPushButton(this);
    okButton->setText(tr("OK"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(onClicked()));

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

    folderButton = new QPushButton(this);
    folderButton->setText(tr("Directory"));
    connect(folderButton, SIGNAL(clicked()), this, SLOT(folderButtonClicked()));

    directory = new QLabel(this);
    directory->setText(dir);


    keep = new QComboBox(this);
    keep->addItem(tr("Keep"),"keep");
    keep->addItem(tr("Delete"),"delete");
    if (type == FileWatcher::ImportKeep)
        keep->setCurrentIndex(0);
    else
        keep->setCurrentIndex(1);

    subdirs = new QCheckBox(this);
    subdirs->setChecked(includeSubdirs);

    books = new QComboBox(this);
    NotebookTable ntable;
    QList<qint32> lids;
    ntable.getAll(lids);
    for (int i=0; i<lids.size(); i++) {
        Notebook n;
        ntable.get(n, lids[i]);
        books->addItem(QString::fromStdString(n.name), lids[i]);
    }
    books->model()->sort(0);


    grid = new QGridLayout();
    grid->addWidget(directory,0,1);
    grid->addWidget(folderButton,0,0);
    grid->addWidget(new QLabel(tr("Notebook")),1,0);
    grid->addWidget(books,1,1);
    grid->addWidget(new QLabel(tr("After import")), 2,0);
    grid->addWidget(keep,2,1);
    grid->addWidget(new QLabel(tr("Include subdirectories")),3,0);
    grid->addWidget(subdirs, 3, 1);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    setWindowTitle(tr("Add Import Folder"));


    mainLayout->addLayout(grid);
    mainLayout->addSpacing(1);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}



void WatchFolderAdd::onClicked() {
    okClicked = true;
    int index = books->currentIndex();
    qint32 notebookLid = books->itemData(index).toInt();
    QString folder = this->directory->text();
    FileWatcher::ScanType type = FileWatcher::ImportDelete;
    QString after = keep->itemData(keep->currentIndex()).toString();
    if (after == "keep")
        type = FileWatcher::ImportKeep;
    FileWatcherTable ft;
    if (lid <= 0) {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    } else {
        ft.expunge(lid);
    }
    ft.addEntry(lid, folder, type, notebookLid, subdirs->isChecked());
    close();
}



void WatchFolderAdd::onCancel() {
    okClicked = false;
    close();
}


void WatchFolderAdd::itemSelected() {
    okButton->setEnabled(true);
}


void WatchFolderAdd::folderButtonClicked() {
    QDir dir;
    fileDialog->setDirectory(dir.homePath()+"/");
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    connect(fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(folderSelected(QString)));
    fileDialog->exec();
}


void WatchFolderAdd::folderSelected(QString f) {
    directory->setText(f);
}


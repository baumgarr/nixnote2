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

#ifndef WATCHFOLDERDIALOG_H
#define WATCHFOLDERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "watcher/filewatcher.h"
#include <QTableWidget>

class WatchFolderDialog : public QDialog
{
    Q_OBJECT
private:
    void load();
    void addRow(qint32 lid, int row, QString folder, qint32 notebookLid, FileWatcher::ScanType, bool includeSubdirs);

public:
    explicit WatchFolderDialog(QWidget *parent = 0);
    bool okClicked;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QTableWidget *table;
    
signals:
    
public slots:
    
private slots:
    void onClicked();
    void onCancel();
    void tableSelection();
    void addPressed();
    void editPressed();
    void deletePressed();
};

#endif // WATCHFOLDERDIALOG_H

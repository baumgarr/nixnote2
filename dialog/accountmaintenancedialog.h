/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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


#ifndef ACCOUNTMAINTENANCEDIALOG_H
#define ACCOUNTMAINTENANCEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "gui/nmainmenubar.h"

class AccountMaintenanceDialog : public QDialog
{
    Q_OBJECT
private:
    NMainMenuBar *menuBar;
    QPushButton *okButton;
    QPushButton *closeButton;
    QPushButton *addButton;
    QPushButton *renameButton;
    QPushButton *removeAuthButton;
    QPushButton *deleteButton;
    QListWidget *nameList;
    QVBoxLayout *buttonLayout1;
    QHBoxLayout *buttonLayout2;
    QHBoxLayout *displayLayout;
    QVBoxLayout *mainLayout;
    void loadData();
    QList<QString> names;
    QList<int> ids;
    bool removeDir(const QString & dirName);
    QWidget *parent;

public:
    explicit AccountMaintenanceDialog(NMainMenuBar *menuBar, QWidget *parent = 0);
    
signals:
    
public slots:
    void addAccount();
    void deleteAccount();
    void renameAccount();
    void removeOAuth();
    
};

#endif // ACCOUNTMAINTENANCEDIALOG_H

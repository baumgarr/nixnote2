#ifndef WATCHFOLDERADD_H
#define WATCHFOLDERADD_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QCheckBox>

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

class WatchFolderAdd : public QDialog
{
    Q_OBJECT
private:
    QPushButton *okButton;
    QPushButton *cancelButton;

    QPushButton *folderButton;

    QGridLayout *grid;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
    QFileDialog *fileDialog;

public:
    explicit WatchFolderAdd(qint32 lid=0, QWidget *parent = 0);
    bool okClicked;
    QComboBox   *books;
    QLabel      *directory;
    QComboBox   *keep;
    QCheckBox   *subdirs;
    qint32 lid;
    
signals:
    
public slots:

private slots:
    void onClicked();
    void onCancel();
    void itemSelected();
    void folderButtonClicked();
    void folderSelected(QString f);
    
};

#endif // WATCHFOLDERADD_H


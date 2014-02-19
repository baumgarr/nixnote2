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


#ifndef CLOSENOTEBOOKDIALOG_H
#define CLOSENOTEBOOKDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QGridLayout>

class CloseNotebookDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CloseNotebookDialog(QWidget *parent = 0);
    bool okPressed;
    QPushButton openButton;
    QPushButton closeButton;
    QPushButton cancelButton;
    QPushButton okButton;
    QListWidget openNotebooks;
    QListWidget closeNotebooks;
    QLabel openLabel;
    QLabel closeLabel;
    
signals:
    
public slots:
    void openButtonClicked();
    void closeButtonClicked();
    void okButtonPressed();
    void cancelButtonPressed();
    void closeListSelection();
    void openListSelection();
};

#endif // CLOSENOTEBOOKDIALOG_H

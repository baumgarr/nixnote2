#ifndef ADDUSERACCOUNTDIALOG_H
#define ADDUSERACCOUNTDIALOG_H

#include <QDialog>
#include <QList>
#include <QHBoxLayout>
#include <QPushButton>
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

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

class AddUserAccountDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddUserAccountDialog(QWidget *parent = 0);
    ~AddUserAccountDialog();
    QPushButton *okButton;
    QPushButton *cancelButton;
    bool okPushed;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QGridLayout *labelLayout;
    QLabel *newAccountLabel;
    QLineEdit *newAccountName;
    QLabel *newAccountServerLabel;
    QComboBox *newAccountServer;
    QStringList accountNames;
    
signals:
    
public slots:
    void okClicked();
    void cancelClicked();
    void textChanged(QString text);
    
};

#endif // ADDUSERACCOUNTDIALOG_H

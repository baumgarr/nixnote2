#ifndef ENCRYPTDIALOG_H
#define ENCRYPTDIALOG_H

#include <QDialog>
#include <QLineEdit>
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

#include <QLabel>

class EnCryptDialog : public QDialog
{
    Q_OBJECT
private:
    bool wasOkPressed;
    QLineEdit	password;
    QLineEdit password2;
    QLineEdit hint;
    QPushButton ok;
    QLabel error;

public:
    explicit EnCryptDialog(QWidget *parent = 0);
    QString getPasswordVerify();
    QString getPassword();
    QString getHint();
    bool okPressed();
    
signals:
    
public slots:

private slots:
    void validateInput();
    void okButtonPressed();
    void cancelButtonPressed();
    
};

#endif // ENCRYPTDIALOG_H

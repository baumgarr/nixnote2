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

#ifndef ENDECRYPTDIALOG_H
#define ENDECRYPTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class EnDecryptDialog : public QDialog
{
    Q_OBJECT
private:

    QPushButton ok;
    QLabel passwordLabel;
    QLabel hintLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout passwordGrid;
    QGridLayout buttonGrid;




public:
    explicit EnDecryptDialog(QWidget *parent = 0);
    QLineEdit	hint;
    QLineEdit	password;
    bool okPressed;


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

};


#endif // ENDECRYPTDIALOG_H

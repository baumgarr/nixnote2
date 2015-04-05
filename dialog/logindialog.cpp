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

#include "logindialog.h"
#include "global.h"

#include <QString>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>

extern Global global;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("NixNote Login"));
    setWindowIcon(global.getIconResource(":passwordIcon"));
    setLayout(&grid);

    password.setEchoMode(QLineEdit::Password);

    connect(&userid, SIGNAL(textChanged(const QString&)), this, SLOT(validateInput()));
    connect(&password, SIGNAL(textChanged(const QString&)), this, SLOT(validateInput()));

    useridLabel.setText(tr("Userid"));
    passwordLabel.setText(tr("Password"));
    passwordGrid.addWidget(&useridLabel, 1,1);
    passwordGrid.addWidget(&userid, 1, 2);
    passwordGrid.addWidget(&passwordLabel, 2,1);
    passwordGrid.addWidget(&password, 2, 2);
    passwordGrid.setContentsMargins(10, 10,  -10, -10);
    grid.addLayout(&passwordGrid,1,1);

    ok.setText(tr("OK"));
    if (global.password == "" and global.username == "")
        ok.setEnabled(false);
    connect(&ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    cancel.setText(tr("Cancel"));
    connect(&cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    buttonGrid.addWidget(&ok, 1, 1);
    buttonGrid.addWidget(&cancel, 1,2);
    grid.addLayout(&buttonGrid,2,1);
    grid.setSizeConstraint( QLayout::SetFixedSize );
    this->setFont(global.getGuiFont(font()));
}



void LoginDialog::okButtonPressed() {
    okPressed = true;
    close();
}

void LoginDialog::cancelButtonPressed() {
    okPressed = false;
    close();
}



void LoginDialog::validateInput() {
    ok.setEnabled(true);
    if (userid.text()=="") {
            ok.setEnabled(false);
            return;
    }		if (password.text() == "") {
            ok.setEnabled(false);
            return;
    }
}

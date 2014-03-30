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

#include "adduseraccountdialog.h"
#include "global.h"
#include <QSpacerItem>

extern Global global;

AddUserAccountDialog::AddUserAccountDialog(QWidget *parent) :
    QDialog(parent)
{
    okPushed = false;
    okButton = new QPushButton(this);
    cancelButton = new QPushButton(this);
    buttonLayout = new QHBoxLayout();
    mainLayout = new QVBoxLayout();
    labelLayout = new QGridLayout();
    newAccountLabel = new QLabel();
    newAccountServer = new QComboBox();
    newAccountServerLabel = new QLabel();
    newAccountServerLabel->setText(tr("Server"));
    okButton->setText(tr("OK"));
    okButton->setEnabled(false);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    setLayout(mainLayout);

    newAccountLabel = new QLabel(tr("Account Name"));
    newAccountName = new QLineEdit();
    connect(newAccountName, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    labelLayout->addWidget(newAccountLabel,0,0);
    labelLayout->addWidget(newAccountName, 0, 1);
    labelLayout->addWidget(newAccountServerLabel, 1,0);;
    labelLayout->addWidget(newAccountServer,1,1);

    QSpacerItem *spacer1 = new QSpacerItem(10,10,QSizePolicy::Maximum, QSizePolicy::Minimum);
    QSpacerItem *spacer2= new QSpacerItem(10,10,QSizePolicy::Maximum, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->setStretch(0,100);
    buttonLayout->setStretch(3,100);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(spacer2);

    mainLayout->addLayout(labelLayout);
    mainLayout->addLayout(buttonLayout);

    accountNames = global.accountsManager->nameList();

    newAccountServer->addItem(tr("Evernote"), "www.evernote.com");
    newAccountServer->addItem(tr("Yinxiang Biji"), "app.yinxiang.com");
    newAccountServer->addItem(tr("Evernote Sandbox"), "sandbox.evernote.com");
}



AddUserAccountDialog::~AddUserAccountDialog() {
    delete okButton;
    delete cancelButton;
    delete buttonLayout;
    delete mainLayout;
}


void AddUserAccountDialog::cancelClicked() {
    okPushed = false;
    close();
}


void AddUserAccountDialog::okClicked() {
    okPushed = true;
    close();
}


void AddUserAccountDialog::textChanged(QString text) {
    if (text.trimmed() == "")  {
        okButton->setEnabled(false);
        return;
    }
    if (accountNames.contains(text, Qt::CaseInsensitive)) {
        okButton->setEnabled(false);
        return;
    }
    okButton->setEnabled(true);
}

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


#include "emaildialog.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QRegExp>
#include "global.h"

extern Global global;

EmailDialog::EmailDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowIcon(global.getIconResource(":emailIcon"));
    sendPressed = false;
    cancelPressed = true;
    setWindowTitle(tr("Send Email"));

    sendButton = new QPushButton(tr("Send"));
    cancelButton = new QPushButton(tr("Cancel"));

    QGridLayout *grid = new QGridLayout(this);
    setLayout(grid);
    QGridLayout *addressGrid = new QGridLayout();
    QGridLayout *noteGrid = new QGridLayout();
    QGridLayout *buttonGrid = new QGridLayout();
    grid->addLayout(addressGrid, 0,0);
    grid->addLayout(noteGrid, 1,0);
    grid->addLayout(buttonGrid,2,0);

    toLabel = new QLabel(this);
    toLabel->setText(tr("To:"));
    bccLabel = new QLabel(this);
    bccLabel->setText(tr("BCC:"));
    ccLabel = new QLabel(this);
    ccLabel->setText(tr("CC:"));
    subjectLabel = new QLabel(this);
    subjectLabel->setText(tr("Subject:"));
    noteLabel = new QLabel(this);
    noteLabel->setText(tr("Note:"));

    ccSelf = new QCheckBox(this);

    toAddress = new QLineEdit(this);
    ccAddress = new QLineEdit(this);
    bccAddress = new QLineEdit(this);
    subject = new QLineEdit(this);
    note = new QPlainTextEdit(this);

    int row = 0;
    addressGrid->addWidget(toLabel, row, 0);
    addressGrid->addWidget(toAddress, row++, 1);
    addressGrid->addWidget(ccLabel, row, 0);
    addressGrid->addWidget(ccAddress, row++, 1);
    addressGrid->addWidget(bccLabel, row, 0);
    addressGrid->addWidget(bccAddress, row++, 1);

    ccSelf->setText(tr("CC me on this email"));
    noteGrid->addWidget(ccSelf, 0,0);

    addressGrid->addWidget(subjectLabel, row, 0);
    addressGrid->addWidget(subject, row++,1);

    noteGrid->addWidget(noteLabel, 1, 0);
    noteGrid->addWidget(note, 2,0);

    buttonGrid->addWidget(cancelButton, 0,0);
    buttonGrid->addWidget(sendButton, 0,1);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendButtonPressed()));

    connect(toAddress, SIGNAL(textChanged(QString)), this, SLOT(toAddressChanged()));
    connect(ccAddress, SIGNAL(textChanged(QString)), this, SLOT(toAddressChanged()));
    connect(bccAddress, SIGNAL(textChanged(QString)), this, SLOT(toAddressChanged()));
    connect(ccSelf, SIGNAL(toggled(bool)), this, SLOT(toAddressChanged()));
    cancelButton->setAutoDefault(false);
    sendButton->setEnabled(false);
    sendButton->setAutoDefault(true);
    sendButton->setAutoExclusive(true);
    toAddress->setFocus();
}




void EmailDialog::sendButtonPressed() {
    sendPressed = true;
    cancelPressed = false;
    close();
}



void EmailDialog::cancelButtonPressed() {
    cancelPressed = true;
    sendPressed = false;
    close();
}


void EmailDialog::toAddressChanged() {
    QString to, bcc, cc;
    to = toAddress->text().trimmed();
    bcc = bccAddress->text().trimmed();
    cc = ccAddress->text().trimmed();

    if (to.contains("@") || cc.contains("@") || bcc.contains("@") ||
	ccSelf->isChecked())
        sendButton->setEnabled(true);
    else
        sendButton->setEnabled(false);
}



QStringList EmailDialog::tokenizeString(QString value) {
    QStringList values =  value.split(QRegExp(",|;|\\s+"), QString::SkipEmptyParts);

    // There is probably an easier way to do this with regular expressions, but
    // I am horrible at regular expressions.
    for (int i=0; i<values.size(); i++) {
        values[i] = values[i].trimmed();
    }
    return values;
}


QStringList EmailDialog::getToAddresses() {
    return tokenizeString(toAddress->text().trimmed());
}


QStringList EmailDialog::getCcAddresses() {
    return tokenizeString(ccAddress->text().trimmed());
}


QStringList EmailDialog::getBccAddresses() {
    return tokenizeString(bccAddress->text().trimmed());
}

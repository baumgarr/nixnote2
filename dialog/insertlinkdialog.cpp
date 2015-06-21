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

#include "insertlinkdialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

#include "global.h"

extern Global global;

InsertLinkDialog::InsertLinkDialog(bool insert, QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("Insert Link"));
    //setWindowIcon(new QIcon(iconPath+"link.png"));
    QGridLayout *grid = new QGridLayout();
    QGridLayout *input = new QGridLayout();
    QGridLayout *button = new QGridLayout();
    setLayout(grid);
    insertHyperlink = insert;


    url = new QLineEdit("");

    input->addWidget(new QLabel(tr("Url")), 1,1);
    input->addWidget(url, 1, 2);
    input->setContentsMargins(10, 10,  -10, -10);
    grid->addLayout(input, 1,1);

    ok = new QPushButton(tr("OK"));
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    ok->setEnabled(false);

    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    button->addWidget(ok, 1, 1);
    button->addWidget(cancel, 1,2);
    grid->addLayout(button, 3, 1);
    connect(url,SIGNAL(textChanged(QString)), this, SLOT(validateInput()));

   // setAttribute(Qt::WA_DeleteOnClose);
    this->setFont(global.getGuiFont(font()));

}

// Get the password
QString InsertLinkDialog::getUrl() {
    if (urlText.indexOf("://") > 0)
        return urlText;
    if (urlText.trimmed().startsWith("mailto:", Qt::CaseInsensitive))
        return urlText;
    if (urlText.trimmed() != "")
        return "http://"+urlText;
    return urlText;
}


// Set the url
void InsertLinkDialog::setUrl(QString u) {
    url->setText(u);
}



// Check if the OK button was pressed
bool InsertLinkDialog::okButtonPressed() {
    return okPressed;
}



// Check that we have a valid URL
void InsertLinkDialog::validateInput() {
    ok->setEnabled(true);
    if (url->text().trimmed() == "" && insertHyperlink)
        ok->setEnabled(false);
}

void InsertLinkDialog::accept() {
    if (ok->isEnabled()) {
        okPressed = true;
        urlText = url->text();
        QDialog::accept();
    }
}

void InsertLinkDialog::reject() {
    okPressed=false;
    QDialog::reject();
}

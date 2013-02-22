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

#include "insertlatexdialog.h"
#include <QGridLayout>
#include <QLabel>

InsertLatexDialog::InsertLatexDialog(QWidget *parent) :
    QDialog(parent)
{
    wasOkPressed = false;
    setWindowTitle(tr("Insert LaTeX Formula"));
    //setWindowIcon(new QIcon(iconPath+"link.png"));
    QGridLayout *grid = new QGridLayout(this);
    QGridLayout *input = new QGridLayout(this);
    QGridLayout *button = new QGridLayout(this);
    setLayout(grid);


    url.setText("");

    input->addWidget(new QLabel(tr("Formula")), 1,1);
    input->addWidget(&url, 2, 1);
    input->setContentsMargins(10, 10,  -10, -10);
    grid->addLayout(input, 1,1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(accept()));
    ok.setEnabled(false);

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    button->addWidget(&ok, 1, 1);
    button->addWidget(cancel, 1,2);
    grid->addLayout(button, 3, 1);
    connect(&url, SIGNAL(textChanged()), this, SLOT(validateInput()));

    //setAttribute(Qt.WidgetAttribute.WA_DeleteOnClose);
}


// Set the formula
void InsertLatexDialog::setFormula(QString x) {
    url.setText(x);
}


// Get the formula
QString InsertLatexDialog::getFormula() {
    return latexText;
}



// Set the url
void InsertLatexDialog::setUrl(QString u) {
    url.setText(u);
}



// Check if the OK button was pressed
bool InsertLatexDialog::okPressed() {
    return wasOkPressed;
}



// Check that we have a valid URL
void InsertLatexDialog::validateInput() {
    ok.setEnabled(true);
    if (url.toPlainText().trimmed() == "")
        ok.setEnabled(false);
}


void InsertLatexDialog::accept() {
    if (ok.isEnabled()) {
        wasOkPressed = true;
        latexText = url.toPlainText();
        QDialog::accept();
    }
}


void InsertLatexDialog::reject() {
    wasOkPressed=false;
    QDialog::reject();
}

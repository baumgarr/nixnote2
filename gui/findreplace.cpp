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

#include "findreplace.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>

#include "global.h"

extern Global global;

FindReplace::FindReplace(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);

    findLine = new QLineEdit(this);
    closeButton  = new QPushButton(this);

    closeButton->setIcon(global.getIconResource(":filecloseIcon"));
    closeButton->setStyleSheet("border:none;");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));

    findLine->setMaximumWidth(300);

    nextButton = new QPushButton(tr("Next"));
    prevButton = new QPushButton(tr("Previous"));
    caseSensitive = new QCheckBox(tr("Match case"));


    QSpacerItem *spacer = new QSpacerItem(this->width()/3,findLine->height(), QSizePolicy::MinimumExpanding);

    replaceLine = new QLineEdit();

    replaceLine = new QLineEdit();
    replaceLine->setMaximumWidth(300);
    replaceButton = new QPushButton(tr("Replace"), this);
    replaceAllButton = new QPushButton(tr("Replace all"),this);

    QVBoxLayout *closeLayout = new QVBoxLayout();
    closeLayout->addWidget(closeButton);
    QLabel *closeSpacer = new QLabel(this);
    closeLayout->addWidget(closeSpacer);

    QVBoxLayout *lineLayout = new QVBoxLayout();
    lineLayout->addWidget(findLine);
    lineLayout->addWidget(replaceLine);

    QVBoxLayout *button1Layout = new QVBoxLayout();
    button1Layout->addWidget(nextButton);
    button1Layout->addWidget(replaceButton);

    QVBoxLayout *button2Layout = new QVBoxLayout();
    button2Layout->addWidget(prevButton);
    button2Layout->addWidget(replaceAllButton);

    QVBoxLayout *checkLayout = new QVBoxLayout();
    checkLayout->addWidget(caseSensitive);
    checkLayout->addSpacerItem(spacer);

    layout->addLayout(closeLayout);
    layout->addLayout(lineLayout);
    layout->addLayout(button1Layout);
    layout->addLayout(button2Layout);
    layout->addLayout(checkLayout);

    nextButton->setDisabled(true);
    prevButton->setDisabled(true);
    replaceButton->setDisabled(true);
    replaceAllButton->setDisabled(true);
    caseSensitive->setDisabled(true);

    connect(findLine, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(replaceLine, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));

    this->hide();

}


void FindReplace::showFind() {
    replaceLine->hide();
    replaceButton->hide();
    replaceAllButton->hide();
    caseSensitive->hide();

    if (isHidden()) {
        show();
        this->findLine->setFocus();
    }
}


void FindReplace::showFindReplace() {
    replaceLine->show();
    replaceButton->show();
    replaceAllButton->show();
    caseSensitive->show();

    if (isHidden())
        show();
}


void FindReplace::textChanged() {
    nextButton->setDisabled(true);
    prevButton->setDisabled(true);
    replaceButton->setDisabled(true);
    replaceAllButton->setDisabled(true);
    caseSensitive->setDisabled(true);

    if (findLine->text().trimmed() != "") {
        nextButton->setDisabled(false);
        prevButton->setDisabled(false);
        replaceButton->setDisabled(false);
        replaceAllButton->setDisabled(false);
        caseSensitive->setDisabled(false);
    }
}



QWebPage::FindFlags FindReplace::getCaseSensitive() {
    QWebPage::FindFlags ff;
    ff = QWebPage::HighlightAllOccurrences;
    ff = 0;
    if (caseSensitive->isChecked())
        ff = ff | QWebPage::FindCaseSensitively;

    return ff;
}

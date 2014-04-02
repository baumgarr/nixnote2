/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte
Copyright (C) 2014 Milos.Kozina@postino.net

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


#include "htmlentitiesdialog.h"
#include <QTextDocument>
#include "global.h"

extern Global global;

HtmlEntitiesDialog::HtmlEntitiesDialog(QWidget *parent) :
    QDialog(parent)
{

    editMode = false;
    cancelString = tr("Cancel");
    closeString = tr("Close");
    editString = tr("Edit entries list");
    saveString = tr("Save enities list");
    defaultEntities.append("&larr;");
    defaultEntities.append("&rarr;");
    defaultEntities.append("&lArr;");
    defaultEntities.append("&rArr;");
    defaultEntities.append("&uarr;");
    defaultEntities.append("&darr;");
    defaultEntities.append("&uArr;");
    defaultEntities.append("&dArr;");
    defaultEntities.append("&harr;");
    defaultEntities.append("&#8597;");
    defaultEntities.append("&#8661;");
    defaultEntities.append("&hArr;");
    defaultEntities.append("&spades;");
    defaultEntities.append("&clubs;");
    defaultEntities.append("&diams;");
    defaultEntities.append("&hearts;");
    defaultEntities.append("&#9785;");
    defaultEntities.append("&#9786;");
    defaultEntities.append("&#9787;");
    defaultEntities.append("&#9775;");
    defaultEntities.append("&copy;");
    defaultEntities.append("&reg;");
    defaultEntities.append("&euro;");
    defaultEntities.append("&deg;");
    defaultEntities.append("&frac14;");
    defaultEntities.append("&frac12;");
    defaultEntities.append("&frac34;");
    defaultEntities.append("&plusmn;");
    defaultEntities.append("&#10003;");
    defaultEntities.append("&#10004;");
    defaultEntities.append("&#10007;");
    defaultEntities.append("&#10008;");
    this->listSeparator = ",";
    loadCustomEntities();
    setWindowTitle(tr("HTML Entities Selection"));
    this->setWindowIcon(QIcon(":htmlentities.png"));
    setModal(false);

    initGrid();
    initEntitiesGrid();
    signalmapper = new QSignalMapper();
    initEntitiesButtons();
    connect(signalmapper, SIGNAL(mapped(QString)), this, SLOT(htmlEntityClicked(QString)));

    scrollArea = new QScrollArea();
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setMinimumWidth(ENTITY_BUTTON_COLUMNS*(ENTITY_BUTTON_WIDTH+2*ENTITY_MARGIN));
    scrollArea->setMinimumHeight(ENTITY_BUTTON_ROWS*(ENTITY_BUTTON_HEIGHT+2*ENTITY_MARGIN));

    initEntitiesWidget();
    scrollArea->setWidget(entitiesWidget);
    scrollArea->setWidgetResizable(false);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    grid->addWidget(scrollArea,2,1);

    buttonLayout = new QHBoxLayout();
    editButton = new QPushButton(editString);
    closeButton = new QPushButton(closeString);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeClicked()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    grid->addLayout(buttonLayout,3,1);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    textEdit = NULL;
    scrollArea->setWidgetResizable(false);
    //layout()->setSizeConstraint( QLayout::SetFixedSize);
    //saveSize = this->size();
}


void HtmlEntitiesDialog::clearEntitiesButtons() {
    for (int i =buttonList.size()-1; i>=0; i--) {
        signalmapper->removeMappings(buttonList[i]);
        entitiesGrid->removeWidget(buttonList[i]);
        buttonList[i]->setVisible(false);
        //delete buttonList[i];
    }
    buttonList.clear();
}


void HtmlEntitiesDialog::initEntitiesButtons() {
    int row = 0;
    int column = 0;
    QStringList *ptr;
    if (entities.size() > 0)
        ptr = &entities;
    else
        ptr = &defaultEntities;
    for (int i=0; i<ptr->size(); i++) {
        column = i % ENTITY_BUTTON_COLUMNS;
        if (column == 0)
            row++;
        QTextDocument doc;
        doc.setHtml(ptr->at(i));
        QString htmlText = doc.toPlainText();
        QPushButton *button = new QPushButton(htmlText);
        signalmapper->setMapping(button, ptr->at(i));
        connect(button, SIGNAL(clicked()), signalmapper, SLOT(map()));
        button->setMinimumHeight(ENTITY_BUTTON_HEIGHT);
        button->setMinimumWidth(ENTITY_BUTTON_WIDTH);
        entitiesGrid->addWidget(button,row, column);
        buttonList.append(button);
    }
}

void HtmlEntitiesDialog::initEntitiesWidget() {
    entitiesWidget = new QWidget();
    entitiesWidget->setLayout(entitiesGrid);
}


void HtmlEntitiesDialog::htmlEntityClicked(QString htmlEntity) {
    emit entityClicked(htmlEntity);
}



void HtmlEntitiesDialog::initEntitiesGrid() {
    entitiesGrid = new QGridLayout();
    entitiesGrid->setContentsMargins(QMargins(ENTITY_MARGIN, ENTITY_MARGIN, ENTITY_MARGIN, ENTITY_MARGIN));
}


void HtmlEntitiesDialog::initGrid() {
    grid = new QGridLayout();
    setLayout(grid);
}

void HtmlEntitiesDialog::closeClicked() {
    if (editMode) {
        editMode = false;
        scrollArea->takeWidget();
        scrollArea->setWidget(entitiesWidget);
        editButton->setText(editString);
        closeButton->setText(closeString);
        return;
    }

    this->close();
}


void HtmlEntitiesDialog::editClicked() {
    if (editMode) {
        if (textEdit->document()->isModified()) {
            QString value = textEdit->toPlainText();
            QString fileName = global.fileManager.getHomeDirPath("") + QString("entities.txt");
            QDir dir;
            dir.remove(fileName);
            QFile file(fileName);
            file.open(QFile::WriteOnly);
            if (file.isOpen()) {
                QTextStream out(&file);
                out << value;
            }
            file.close();
        }
        clearEntitiesButtons();
        loadCustomEntities();
        initEntitiesButtons();

    } else {
        if (textEdit == NULL) {
            textEdit = new QTextEdit(tr("Comma separated list of HTML entities.\nSee http://www.w3schools.com/charsets/ for list."));
        }
        textEdit->setText(getEntities(true));
    }
    editMode = !editMode;
    scrollArea->takeWidget();
    scrollArea->setWidget(editMode ? textEdit : entitiesWidget);
    scrollArea->setWidgetResizable(editMode);
    editButton->setText(editMode ? saveString : editString);
    closeButton->setText(editMode ? cancelString : closeString);
}


bool HtmlEntitiesDialog::close() {
    this->hide();
    return false;
}


QString HtmlEntitiesDialog::getEntities(bool withBreakLines) {
    QString result = "";
    QStringList *ptr;
    if (entities.size() > 0)
        ptr = &entities;
    else
        ptr = &defaultEntities;
    for (int i=0; i<ptr->size(); i++) {
        result = result+ptr->at(i) + " ";
        if ((i+1) % ENTITY_BUTTON_COLUMNS == 0 && withBreakLines) {
            result = result + ",\n";
        } else {
            result = result + ", ";
        }
    }
    return result;
}


void HtmlEntitiesDialog::getEntitiesList(QStringList *list, QString entitiesAsText) {
    list->clear();
    if (entitiesAsText != "") {
        QStringList separated = entitiesAsText.split(listSeparator);
        for (int i = 0; i < separated.length(); i++) {
            QString s = separated.at(i).trimmed();
            if (s.startsWith("&") && s.endsWith(";")) {
                if (!list->contains(s))
                    list->append(s);
            }
        }
    }
}



void HtmlEntitiesDialog::loadCustomEntities() {
    entities.clear();
    QString fileName = global.fileManager.getHomeDirPath("") + QString("entities.txt");
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\t", " ");
            line = line.replace("\n", " ");
            line = line.replace("\r", " " );
            line = line.trimmed();
            QStringList list = line.split(this->listSeparator);
            for(int i=0;i<list.size(); i++) {
                QString s = list.at(i).trimmed();
                if (s.startsWith("&") && s.endsWith(";")) {
                    if (!entities.contains(s))
                        entities.append(s);
                }
            }
        }
        file.close();
    }
}

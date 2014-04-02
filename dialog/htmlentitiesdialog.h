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



#ifndef HTMLENTITIESDIALOG_H
#define HTMLENTITIESDIALOG_H

#include <QDialog>
#include <QScrollBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QIcon>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QWidget>
#include <QSignalMapper>

#define ENTITY_BUTTON_WIDTH 45
#define ENTITY_BUTTON_COLUMNS 4
#define ENTITY_BUTTON_HEIGHT 25
#define ENTITY_BUTTON_ROWS 4
#define ENTITY_MARGIN 1


class HtmlEntitiesDialog : public QDialog
{
    Q_OBJECT
private:
    QPushButton *closeButton;
    QStringList defaultEntities;
    QPushButton *editButton;
    QScrollArea *scrollArea;
    QWidget *entitiesWidget;
    bool editMode;
    QGridLayout *entitiesGrid;
    //QSize saveSize;
    QGridLayout *grid;
    QSignalMapper *signalmapper;
    QHBoxLayout *buttonLayout;
    QTextEdit *textEdit;
    QString listSeparator;
    QStringList entities;
    QString cancelString;
    QString closeString;
    QString editString;
    QString saveString;
    void initEntitiesWidget();
    void initGrid();
    void clearEntitiesButtons();
    void initEntitiesButtons();
    void initEntitiesGrid();
    QList<QPushButton*> buttonList;
    QString getEntities(bool withBreakLines);
    void getEntitiesList(QStringList *list, QString entitiesAsText);
    void loadCustomEntities();

public:
    explicit HtmlEntitiesDialog(QWidget *parent = 0);

signals:
    void entityClicked(QString entity);

public slots:
    void htmlEntityClicked(QString htmlEntity);
    void editClicked();
    void closeClicked();
    bool close();

};

#endif // HTMLENTITIESDIALOG_H

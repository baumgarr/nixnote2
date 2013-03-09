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

#ifndef NTABLEVIEWHEADER_H
#define NTABLEVIEWHEADER_H

#include <QTableView>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

class NTableViewHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit NTableViewHeader(Qt::Orientation orientation, QWidget *parent = 0);
    QAction *createdDateAction;
    QAction *changedDateAction;
    QAction *subjectDateAction;
    QAction *tagsAction;
    QAction *titleAction;
    QAction *notebookAction;
    QAction *synchronizedAction;
    QAction *sourceAction;
    QAction *authorAction;
    QAction *urlAction;
    QAction *altitudeAction;
    QAction *longitudeAction;
    QAction *latitudeAction;
    QAction *hasTodoAction;
    QAction *hasEncryptionAction;
    QAction *sizeAction;
    QAction *thumbnailAction;
    void checkActions();
    bool isThumbnailVisible();


signals:
    void setColumnVisible(int position, bool visible);

public slots:
    void saveSort(int pos, Qt::SortOrder order);
    void saveHeader();

    void createdDateChecked(bool);
    void changedDateChecked(bool);
    void subjectDateChecked(bool);
    void tagsChecked(bool);
    void titleChecked(bool);
    void notebookChecked(bool);
    void synchronizedChecked(bool);
    void sourceChecked(bool);
    void authorChecked(bool);
    void urlChecked(bool);
    void altitudeChecked(bool);
    void longitudeChecked(bool);
    void latitudeChecked(bool);
    void hasTodoChecked(bool);
    void hasEncryptionChecked(bool);
    void sizeChecked(bool);
    void thumbnailChecked(bool);
};

#endif // NTABLEVIEWHEADER_H

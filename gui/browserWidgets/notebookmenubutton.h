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

#ifndef NOTEBOOKMENUBUTTON_H
#define NOTEBOOKMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include <QString>
#include <QList>
#include <QAction>
#include <QActionGroup>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>


using namespace evernote::edam  ;
using namespace std;


class NotebookMenuButton : public QPushButton
{
    Q_OBJECT
private:
    QString currentNotebookName;
    QMenu rootMenu;
    QList<QMenu*> stackMenus;
    QList<QAction*> actions;
    int currentAction;
    qint32 currentNoteLid;
    void addNotebookMenuItem(QMenu *menu, QAction *action);
    void addStackMenuItem(QMenu *newMenu);
    QMenu *findStack(Notebook n);

public:
    explicit NotebookMenuButton(QWidget *parent = 0);
    void setCurrentNotebook(qint32 lid, Note n);
    void addNotebook(qint32 lid);
    qint32 notebookLid;
    QString notebookName;

    
signals:
    void notebookChanged();

public slots:
    void loadData();
    void reloadData();
    
private slots:
    void notebookSelected();
    
};

#endif // NOTEBOOKMENUBUTTON_H

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

#ifndef NMAINMENUBAR_H
#define NMAINMENUBAR_H

#include <QMenuBar>
#include <QFont>

#include "nixnote.h"
class NixNote;

class NMainMenuBar : public QMenuBar
{
    Q_OBJECT
private:
    NixNote *parent;
    QFont font;


    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupToolsMenu();
    void setupHelpMenu();
    void setupShortcut(QAction *action, QString text);

public:
    QList<QAction*> userAccountActions;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QMenu *findReplaceMenu;

    QAction *exitAction;
    QAction *printAction;
    QAction *synchronizeAction;
    QAction *importFoldersDialogAction;
    QAction *addUserAction;
    QAction *disconnectAction;
    QAction *databaseStatusDialogAction;
    QAction *restoreDatabaseAction;
    QAction *accountDialogAction;
    QAction *userMaintenanceAction;
    QAction *aboutAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteAsTextAction;
    QAction *deleteNoteAction;
    QAction *selectAllAction;
    QAction *preferencesAction;

    QAction *searchNotesAction;
    QAction *searchFindAction;
    QAction *searchFindNextAction;
    QAction *searchFindPrevAction;
    QAction *searchFindReplaceAction;
    QAction *resetSearchAction;

    QAction *viewSourceAction;
    QAction *viewExtendedInformation;
    QAction *viewNoteListWide;
    QAction *viewNoteListNarrow;
    QAction *viewLeftPanel;
    QAction *viewNoteList;
    QAction *viewNotePanel;
    QAction *viewToolbar;
    QAction *viewStatusbar;

    explicit NMainMenuBar(QWidget *parent = 0);

    void addUserAccount(QAction *action);

signals:

public slots:

};

#endif // NMAINMENUBAR_H

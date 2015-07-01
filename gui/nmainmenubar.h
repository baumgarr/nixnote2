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

    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupNoteMenu();
    void setupToolsMenu();
    void setupHelpMenu();
    void setupShortcut(QAction *action, QString text);

public:
    QList<QAction*> userAccountActions;
    QList<QAction*> themeActions;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *noteMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QMenu *findReplaceMenu;
    QMenu *themeMenu;

    QAction *exitAction;
    QAction *openCloseAction;
    QAction *printAction;
    QAction *synchronizeAction;
    QAction *importFoldersDialogAction;
    QAction *addUserAction;
    QAction *disconnectAction;
    QAction *databaseStatusDialogAction;
    QAction *reindexDatabaseAction;
    QAction *restoreDatabaseAction;
    QAction *backupDatabaseAction;
    QAction *exportNoteAction;
    QAction *importNoteAction;
    QAction *accountDialogAction;
    QAction *pauseIndexingAction;
    QAction *userMaintenanceAction;
    QAction *aboutAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteAsTextAction;
    QAction *removeFormattingAction;
    QAction *selectAllAction;
    QAction *preferencesAction;

    QAction *searchNotesAction;
    QAction *searchFindAction;
    QAction *searchFindNextAction;
    QAction *searchFindPrevAction;
    QAction *searchFindReplaceAction;
    QAction *resetSearchAction;

    QAction *viewSourceAction;
    QAction *viewHistoryAction;
    QAction *viewExtendedInformation;
    QAction *viewNoteListWide;
    QAction *viewNoteListNarrow;
    QAction *viewLeftPanel;
    QAction *viewFavoritesTree;
    QAction *viewNotebookTree;
    QAction *viewTagTree;
    QAction *viewSearchTree;
    QAction *viewAttributesTree;
    QAction *viewTrashTree;
    QAction *viewNoteList;
    QAction *viewNotePanel;
    QAction *viewToolbar;
    QAction *viewStatusbar;

    QAction *deleteNoteAction;
    QAction *newNoteAction;
    QAction *newWebcamNoteAction;
    QAction *duplicateNoteAction;
    QAction *pinNoteAction;
    QAction *unpinNoteAction;
    QAction *spellCheckAction;
    QAction *openTrunkAction;
    QAction *openManualAction;
    QAction *openEvernoteSupportAction;
    QAction *openMessageLogAction;
    QAction *openUserForumAction;
    QAction *openEvernoteAccountPageAction;
    QAction *themeInformationAction;

    explicit NMainMenuBar(QWidget *parent = 0);

    void addUserAccount(QAction *action);
    void setupThemeMenu();

signals:

public slots:
    void openManual();
    void openUserForum();
    void openEvernoteAccountPage();
    void openThemeInformation();
};

#endif // NMAINMENUBAR_H

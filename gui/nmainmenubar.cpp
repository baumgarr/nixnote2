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

#include "nmainmenubar.h"
#include "global.h"
#include <QAbstractAnimation>
#include <QFileIconProvider>
#include <QDesktopServices>

extern Global global;

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first < b.first;
    }
};

NMainMenuBar::NMainMenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    this->parent = (NixNote*)parent;
    font.setPointSize(8);

    setupFileMenu();
    setupEditMenu();
    setupViewMenu();
    setupNoteMenu();
    setupToolsMenu();
    setupHelpMenu();
}


void NMainMenuBar::setupFileMenu() {
    fileMenu = this->addMenu(tr("&File"));

    QFont f;
    f.setPointSize(8);

    printAction = new QAction(tr("Print Note"), this);
    printAction->setToolTip(tr("Print this note"));
    printAction->setFont(f);
    connect(printAction, SIGNAL(triggered()), parent, SLOT(printNote()));
    setupShortcut(printAction, QString("File_Print"));
    fileMenu->addAction(printAction);

    fileMenu->addSeparator();

    backupDatabaseAction = new QAction(tr("Backup Database"), this);
    backupDatabaseAction->setToolTip(tr("Backup database to a file"));
    backupDatabaseAction->setFont(f);
    connect(backupDatabaseAction, SIGNAL(triggered()), parent, SLOT(databaseBackup()));
    setupShortcut(backupDatabaseAction, QString("File_Backup_Database"));
    fileMenu->addAction(backupDatabaseAction);

    restoreDatabaseAction = new QAction(tr("Restore Database"), this);
    restoreDatabaseAction->setToolTip(tr("Restore from a backup"));
    restoreDatabaseAction->setFont(f);
    connect(restoreDatabaseAction, SIGNAL(triggered()), parent, SLOT(databaseRestore()));
    setupShortcut(restoreDatabaseAction, QString("File_Restore_Database"));
    fileMenu->addAction(restoreDatabaseAction);

    fileMenu->addSeparator();

    exportNoteAction = new QAction(tr("Export Notes"), this);
    exportNoteAction->setToolTip(tr("Export selected notes to a file"));
    exportNoteAction->setFont(f);
    connect(exportNoteAction, SIGNAL(triggered()), parent, SLOT(noteExport()));
    setupShortcut(exportNoteAction, QString("File_Note_Export"));
    fileMenu->addAction(exportNoteAction);

    importNoteAction = new QAction(tr("Import Notes"), this);
    importNoteAction->setToolTip(tr("Import notes from an export file"));
    importNoteAction->setFont(f);
    connect(importNoteAction, SIGNAL(triggered()), parent, SLOT(noteImport()));
    setupShortcut(importNoteAction, QString("File_Note_Import"));
    fileMenu->addAction(importNoteAction);

    fileMenu->addSeparator();
    // Start adding the user accounts
    QList<QString> names = global.accountsManager->nameList();
    QList<int> ids = global.accountsManager->idList();
    QList< QPair<int, QString> > pairList;
    for (int i=0; i<ids.size(); i++) {
        pairList.append(QPair<int,QString>(ids[i], names[i]));
    }
    qSort(pairList.begin(), pairList.end(), QPairFirstComparer());
    for (int i=0; i<ids.size(); i++) {
        QAction *accountAction = new QAction(pairList[i].second +" - (" +QString::number(pairList[i].first)+")", this);
        accountAction->setData(pairList[i].first);
        accountAction->setCheckable(true);
        accountAction->setFont(f);
        if (global.accountsManager->currentId == pairList[i].first)
            accountAction->setChecked(true);
        else {
            accountAction->setText(tr("Switch to ")+pairList[i].second +" - (" +QString::number(pairList[i].first)+")");
        }
        fileMenu->addAction(accountAction);
        connect(accountAction, SIGNAL(triggered()), parent, SLOT(switchUser()));
        userAccountActions.append(accountAction);
    }

    addUserAction = new QAction(tr("Add Another User..."), this);
    addUserAction->setFont(font);
    fileMenu->addAction(addUserAction);
    connect(addUserAction, SIGNAL(triggered()), parent, SLOT(addAnotherUser()));

    userMaintenanceAction = new QAction(tr("User Account Maintenance"),this);
    userMaintenanceAction->setFont(font);
    fileMenu->addAction(userMaintenanceAction);
    connect(userMaintenanceAction, SIGNAL(triggered()), parent, SLOT(userMaintenance()));

    fileMenu->addSeparator();

    openCloseAction = new QAction(tr("Open/Close Notebooks"), this);
    openCloseAction->setToolTip(tr("Open/Close Notebooks"));
    connect(openCloseAction, SIGNAL(triggered()), parent, SLOT(openCloseNotebooks()));
    openCloseAction->setFont(f);
    setupShortcut(exitAction, QString("File_Notebook_OpenClose"));
    fileMenu->addAction(openCloseAction);

    fileMenu->addSeparator();

    exitAction = new QAction(tr("Exit"), this);
    exitAction->setToolTip(tr("Close the program"));
    connect(exitAction, SIGNAL(triggered()), parent, SLOT(closeNixNote()));
    exitAction->setShortcut(QKeySequence::Close);
    exitAction->setIcon(QIcon::fromTheme("exit"));
    exitAction->setFont(f);
    //exitAction->setIcon(QFileIconProvider().icon(QFileInfo("/home/randy/Dropbox/nixnote.jar")));
    setupShortcut(exitAction, QString("File_Exit"));
    fileMenu->addAction(exitAction);
}


void NMainMenuBar::addUserAccount(QAction *action) {
    fileMenu->insertAction(addUserAction, action);
    userAccountActions.append(action);
}


void NMainMenuBar::setupEditMenu() {
    editMenu = this->addMenu(tr("&Edit"));

    undoAction = new QAction(tr("Undo"), this);
    setupShortcut(undoAction, QString("Edit_Undo"));
    undoAction->setFont(font);
    editMenu->addAction(undoAction);

    redoAction = new QAction(tr("Redo"), this);
    setupShortcut(redoAction, QString("Edit_Redo"));
    redoAction->setFont(font);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    cutAction = new QAction(tr("Cut"), this);
    setupShortcut(cutAction, QString("Edit_Cut"));
    cutAction->setFont(font);
    editMenu->addAction(cutAction);

    copyAction = new QAction(tr("Copy"), this);
    setupShortcut(copyAction, QString("Edit_Copy"));
    copyAction->setFont(font);
    editMenu->addAction(copyAction);

    pasteAction = new QAction(tr("Paste"), this);
    setupShortcut(pasteAction, QString("Edit_Paste"));
    pasteAction->setFont(font);
    editMenu->addAction(pasteAction);

    pasteAsTextAction = new QAction(tr("Paste as Unformatted Text"), this);
    setupShortcut(pasteAsTextAction, QString("Edit_Paste_Without_Formatting"));
    pasteAsTextAction->setFont(font);
    editMenu->addAction(pasteAsTextAction);

    removeFormattingAction = new QAction(tr("Remove Formatting"), this);
    setupShortcut(removeFormattingAction, QString("Edit_Remove_Formatting"));
    removeFormattingAction->setFont(font);
    editMenu->addAction(removeFormattingAction);

    editMenu->addSeparator();

    selectAllAction = new QAction(tr("Select All"), this);
    setupShortcut(selectAllAction, QString("Edit_Select_All"));
    selectAllAction->setFont(font);
    editMenu->addAction(selectAllAction);

    editMenu->addSeparator();

    findReplaceMenu = editMenu->addMenu(tr("Find and Replace"));
    findReplaceMenu->setFont(font);

    searchNotesAction = new QAction(tr("Search Notes"), this);
    setupShortcut(searchNotesAction, QString("Edit_Search_Notes"));
    searchNotesAction->setFont(font);
    findReplaceMenu->addAction(searchNotesAction);
    connect(searchNotesAction, SIGNAL(triggered()), parent->searchText, SLOT(setFocus()));

    resetSearchAction = new QAction(tr("Reset Search"), this);
    setupShortcut(resetSearchAction, QString("Edit_Reset_Search"));
    resetSearchAction->setFont(font);
    findReplaceMenu->addAction(resetSearchAction);
    connect(resetSearchAction, SIGNAL(triggered()), parent, SLOT(resetView()));

    findReplaceMenu->addSeparator();

    searchFindAction = new QAction(tr("Find in Note"), this);
    setupShortcut(searchFindAction, QString("Edit_Search_Find"));
    searchFindAction->setFont(font);
    findReplaceMenu->addAction(searchFindAction);
    connect(searchFindAction, SIGNAL(triggered()), parent, SLOT(findInNote()));


    searchFindNextAction = new QAction(tr("Find Next"), this);
    setupShortcut(searchFindNextAction, QString("Edit_Search_Find_Next"));
    searchFindNextAction->setFont(font);
    findReplaceMenu->addAction(searchFindNextAction);
    connect(searchFindNextAction, SIGNAL(triggered()), parent, SLOT(findNextInNote()));

    searchFindPrevAction = new QAction(tr("Find Previous"), this);
    setupShortcut(searchFindPrevAction, QString("Edit_Search_Find_Prev"));
    searchFindPrevAction->setFont(font);
    findReplaceMenu->addAction(searchFindPrevAction);
    connect(searchFindPrevAction, SIGNAL(triggered()), parent, SLOT(findPrevInNote()));

    findReplaceMenu->addSeparator();

    searchFindReplaceAction = new QAction(tr("Replace Within Note..."), this);
    setupShortcut(searchFindReplaceAction, QString("Edit_Search_Find_Replace"));
    searchFindReplaceAction->setFont(font);
    findReplaceMenu->addAction(searchFindReplaceAction);
    connect(searchFindReplaceAction, SIGNAL(triggered()), parent, SLOT(findReplaceInNote()));

    editMenu->addSeparator();

    preferencesAction = new QAction(tr("Preferences"), this);
    setupShortcut(preferencesAction, QString("Edit_Preferences"));
    preferencesAction->setFont(font);
    editMenu->addAction(preferencesAction);
    connect(preferencesAction, SIGNAL(triggered()), parent, SLOT(openPreferences()));

}

void NMainMenuBar::setupViewMenu() {
    viewMenu = this->addMenu(tr("&View"));

    viewNoteListWide = new QAction(tr("Wide Note List"), this);
    setupShortcut(viewNoteListWide, "View_Note_List_Wide");
    viewNoteListWide->setFont(font);
    viewMenu->addAction(viewNoteListWide);
    viewNoteListWide->setCheckable(true);

    viewNoteListNarrow = new QAction(tr("Narrow Note List"), this);
    setupShortcut(viewNoteListNarrow, "View_Note_List_Narrow");
    viewNoteListNarrow->setFont(font);
    viewNoteListNarrow->setCheckable(true);
    viewMenu->addAction(viewNoteListNarrow);
    connect(viewNoteListNarrow, SIGNAL(triggered()), parent, SLOT(viewNoteListNarrow()));
    connect(viewNoteListWide, SIGNAL(triggered()), parent, SLOT(viewNoteListWide()));

    viewSourceAction = new QAction(tr("Show Source"), this);
    setupShortcut(viewSourceAction, "View_Source");
    viewSourceAction->setFont(font);
    viewMenu->addAction(viewSourceAction);

    viewHistoryAction = new QAction(tr("Note History"), this);
    setupShortcut(viewHistoryAction, "View_Note_History");
    viewHistoryAction->setFont(font);
    viewMenu->addAction(viewHistoryAction);

    viewMenu->addSeparator();

    viewLeftPanel = new QAction(tr("Show Left Panel"), this);
    setupShortcut(viewLeftPanel, "View_Show_Left_Side");
    viewLeftPanel->setCheckable(true);
    viewLeftPanel->setChecked(true);
    viewLeftPanel->setFont(font);
    viewMenu->addAction(viewLeftPanel);
    connect(viewLeftPanel, SIGNAL(triggered()), parent, SLOT(toggleLeftPanel()));

    viewNoteList = new QAction(tr("Show Note List"), this);
    setupShortcut(viewNoteList, "View_Show_Note_List");
    viewNoteList->setCheckable(true);
    viewNoteList->setChecked(true);
    viewNoteList->setFont(font);
    viewMenu->addAction(viewNoteList);
    connect(viewNoteList, SIGNAL(triggered()), parent, SLOT(toggleNoteList()));

    viewNotePanel = new QAction(tr("Show Note Panel"), this);
    setupShortcut(viewNotePanel, "View_Show_Note_Panel");
    viewNotePanel->setCheckable(true);
    viewNotePanel->setChecked(true);
    viewNotePanel->setFont(font);
    viewMenu->addAction(viewNotePanel);
    connect(viewNotePanel, SIGNAL(triggered()), parent, SLOT(toggleTabWindow()));

    viewMenu->addSeparator();

    viewExtendedInformation = new QAction(tr("View Note Info"), this);
    setupShortcut(viewExtendedInformation, "View_Extended_Information");
    viewExtendedInformation->setFont(font);
    viewMenu->addAction(viewExtendedInformation);

    viewToolbar = new QAction(tr("View Toolbar"), this);
    setupShortcut(viewToolbar, "View_Toolbar");
    viewToolbar->setFont(font);
    viewMenu->addAction(viewToolbar);
    viewToolbar->setCheckable(true);
    viewToolbar->setChecked(true);
    connect(viewToolbar, SIGNAL(triggered()), parent, SLOT(toggleToolbar()));

    viewStatusbar = new QAction(tr("View Statusbar"), this);
    setupShortcut(viewStatusbar, "View_Statusbar");
    viewStatusbar->setFont(font);
    viewMenu->addAction(viewStatusbar);
    viewStatusbar->setCheckable(true);
    //viewStatusbar->setChecked(true);
    connect(viewStatusbar, SIGNAL(triggered()), parent, SLOT(toggleStatusbar()));

}


void NMainMenuBar::setupNoteMenu() {

    noteMenu = this->addMenu(tr("&Note"));

    newNoteAction = new QAction(tr("New Note"), noteMenu);
    setupShortcut(deleteNoteAction, QString("File_Note_New"));
    newNoteAction->setFont(font);
    noteMenu->addAction(newNoteAction);
    connect(newNoteAction, SIGNAL(triggered()), parent, SLOT(newNote()));

    newWebcamNoteAction = new QAction(tr("New Webcam Note"), noteMenu);
    setupShortcut(deleteNoteAction, QString("File_Note_New_Webcam"));
    newWebcamNoteAction->setFont(font);
    noteMenu->addAction(newWebcamNoteAction);
    connect(newWebcamNoteAction, SIGNAL(triggered()), parent, SLOT(newWebcamNote()));

    duplicateNoteAction = new QAction(tr("Duplicate Note"), noteMenu);
    setupShortcut(duplicateNoteAction, QString("File_Note_Duplicate"));
    duplicateNoteAction->setFont(font);
    noteMenu->addAction(duplicateNoteAction);
    connect(duplicateNoteAction, SIGNAL(triggered()), parent, SLOT(duplicateCurrentNote()));

    deleteNoteAction = new QAction(tr("Delete"), noteMenu);
    setupShortcut(deleteNoteAction, QString("File_Note_Delete"));
    deleteNoteAction->setFont(font);
    noteMenu->addAction(deleteNoteAction);
    connect(deleteNoteAction, SIGNAL(triggered()), parent, SLOT(deleteCurrentNote()));

    noteMenu->addSeparator();

    spellCheckAction = new QAction(tr("Spell Check"), noteMenu);
    setupShortcut(spellCheckAction, QString("File_Note_Spell_Check"));
    spellCheckAction->setFont(font);
    noteMenu->addAction(spellCheckAction);
    connect(spellCheckAction, SIGNAL(triggered()), parent, SLOT(spellCheckCurrentNote()));


    noteMenu->addSeparator();

    pinNoteAction = new QAction(tr("Pin Note"), noteMenu);
    setupShortcut(pinNoteAction, QString("NOTE_PIN"));
    pinNoteAction->setFont(font);
    noteMenu->addAction(pinNoteAction);
    connect(pinNoteAction, SIGNAL(triggered()), parent, SLOT(pinCurrentNote()));

    unpinNoteAction = new QAction(tr("UnPin Note"), noteMenu);
    setupShortcut(unpinNoteAction, QString("NOTE_UNPIN"));
    unpinNoteAction->setFont(font);
    noteMenu->addAction(unpinNoteAction);
    connect(unpinNoteAction, SIGNAL(triggered()), parent, SLOT(unpinCurrentNote()));

}


void NMainMenuBar::setupToolsMenu() {
  toolsMenu = this->addMenu(tr("&Tools"));

  synchronizeAction = new QAction(tr("Synchronize"), this);
  synchronizeAction->setToolTip(tr("Close the program"));
  connect(synchronizeAction, SIGNAL(triggered()), parent, SLOT(synchronize()));
  setupShortcut(synchronizeAction, QString("Tools_Synchronize"));
  synchronizeAction->setFont(font);
  toolsMenu->addAction(synchronizeAction);

  disconnectAction = new QAction(tr("Disconnect"), this);
  synchronizeAction->setToolTip(tr("Disconnect from Evernote"));
  connect(disconnectAction, SIGNAL(triggered()), parent, SLOT(disconnect()));
  setupShortcut(disconnectAction, QString(""));
  disconnectAction->setFont(font);
  toolsMenu->addAction(disconnectAction);
  disconnectAction->setEnabled(false);
  disconnectAction->setVisible(false);  /// We can probably delete this whole menu option

  pauseIndexingAction = new QAction(tr("Pause Indexing"), this);
  pauseIndexingAction->setToolTip(tr("Temporarily pause indexng"));
  setupShortcut(pauseIndexingAction, QString("Tools_Pause_Indexing"));
  pauseIndexingAction->setFont(font);
  connect(pauseIndexingAction, SIGNAL(triggered()), parent, SLOT(pauseIndexing()));
  pauseIndexingAction->setCheckable(true);
  toolsMenu->addAction(pauseIndexingAction);

  accountDialogAction = new QAction(tr("Account"), this);
  accountDialogAction->setToolTip(tr("Account information"));
  accountDialogAction->setFont(font);
  connect(accountDialogAction, SIGNAL(triggered()), parent, SLOT(openAccount()));
  setupShortcut(accountDialogAction, QString("Tools_Account_Information"));
  toolsMenu->addAction(accountDialogAction);

  toolsMenu->addSeparator();

  databaseStatusDialogAction = new QAction(tr("Database Status"), this);
  databaseStatusDialogAction->setToolTip(tr("Database Status"));
  setupShortcut(databaseStatusDialogAction, QString("Tools_Database_Status"));
  databaseStatusDialogAction->setFont(font);
  connect(databaseStatusDialogAction, SIGNAL(triggered()), parent, SLOT(openDatabaseStatus()));
  toolsMenu->addAction(databaseStatusDialogAction);

  reindexDatabaseAction = new QAction(tr("Reindex Database"), this);
  reindexDatabaseAction->setToolTip(tr("Reindex all notes"));
  setupShortcut(reindexDatabaseAction, QString("Tools_Database_Reindex"));
  reindexDatabaseAction->setFont(font);
  connect(reindexDatabaseAction, SIGNAL(triggered()), parent, SLOT(reindexDatabase()));
  toolsMenu->addAction(reindexDatabaseAction);

  toolsMenu->addSeparator();

  importFoldersDialogAction = new QAction(tr("Import Folders"), this);
  importFoldersDialogAction->setToolTip(tr("Import Folders"));
  importFoldersDialogAction->setFont(font);
  setupShortcut(importFoldersDialogAction, QString("Tools_Import_Folders"));
  connect(importFoldersDialogAction, SIGNAL(triggered()), parent, SLOT(openImportFolders()));
  toolsMenu->addAction(importFoldersDialogAction);


}

void NMainMenuBar::setupHelpMenu() {
    helpMenu = this->addMenu(tr("&Help"));

    openManualAction = new QAction(tr("User's Guide"), this);
    openManualAction->setToolTip(tr("Open the user manual."));
    openManualAction->setFont(font);
    connect(openManualAction, SIGNAL(triggered()),
            this, SLOT(openManual()));
    helpMenu->addAction(openManualAction);

    openMessageLogAction = new QAction(tr("Message Log"), this);
    openMessageLogAction->setToolTip(tr("View current program messages"));
    openMessageLogAction->setFont(font);
    connect(openMessageLogAction, SIGNAL(triggered()), parent, SLOT(openMessageLog()));
    helpMenu->addAction(openMessageLogAction);

    helpMenu->addSeparator();

    openEvernoteSupportAction = new QAction(tr("Evernote Support"), this);
    openEvernoteSupportAction->setToolTip(tr("Go to Evernote's support page"));
    if (global.accountsManager->getServer() == "app.yinxiang.com") {
        openEvernoteSupportAction = new QAction(tr("Yinxiang Biji Support"), this);
        openEvernoteSupportAction->setToolTip(tr("Go to Yinxiang Biji's support page"));
    }
    openEvernoteSupportAction->setFont(font);
    connect(openEvernoteSupportAction, SIGNAL(triggered()), parent, SLOT(openEvernoteSupport()));
    helpMenu->addAction(openEvernoteSupportAction);

    openTrunkAction = new QAction(tr("Evernote Trunk"), this);
    openTrunkAction->setToolTip(tr("Go to Evernote Trunk"));
    openTrunkAction->setFont(font);
    connect(openTrunkAction, SIGNAL(triggered()), parent, SLOT(openTrunk()));
    helpMenu->addAction(openTrunkAction);

    helpMenu->addSeparator();

    aboutAction = new QAction(tr("About"), this);
    aboutAction->setToolTip(tr("About"));
    aboutAction->setFont(font);
    connect(aboutAction, SIGNAL(triggered()), parent, SLOT(openAbout()));
    helpMenu->addAction(aboutAction);
}

void NMainMenuBar::setupShortcut(QAction *action, QString text) {
    if (!global.shortcutKeys->containsAction(&text))
        return;
    QKeySequence key(global.shortcutKeys->getShortcut(&text));
    action->setShortcut(key);
}



void NMainMenuBar::openManual() {
    QDesktopServices::openUrl(QString("file://") +
                              global.getProgramDirPath()+"/help/UserDocumentation.pdf");
}

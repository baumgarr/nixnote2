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

#ifndef NIXNOTE_H
#define NIXNOTE_H

#include <QtGui/QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QSystemTrayIcon>
#include <QToolBar>
#include <QSlider>
#include <QSplashScreen>

#include "watcher/filewatchermanager.h"
#include "gui/ntabwidget.h"
#include "gui/lineedit.h"
#include "sql/databaseconnection.h"
#include "gui/ntableview.h"
#include "gui/ntagview.h"
#include "gui/nsearchview.h"
#include "threads/syncrunner.h"
#include "threads/indexrunner.h"
#include "gui/widgetpanel.h"
#include "gui/nnotebookview.h"
#include "gui/nmainmenubar.h"
#include "gui/nattributetree.h"
#include "gui/ntrashtree.h"
#include "dialog/accountdialog.h"
#include "gui/findreplace.h"
#include "threads/counterrunner.h"
#include "oauth/oauthwindow.h"
#include "html/thumbnailer.h"
#include "reminders/remindermanager.h"




//****************************************
//* This is the main NixNote class that
//* starts everything else.  It is called
//* by main()
//****************************************


// Forward declare classes used later
class DatabaseConnection;
class NMainMenuBar;
class NixNote;
class SyncRunner;
class IndexRunner;
class CounterRunner;
class NTabWidget;

// Define the actual class
class NixNote : public QMainWindow
{
    Q_OBJECT

private:
    DatabaseConnection *db;  // The database connection
    NTableView *noteTableView;
    NSearchView *searchTreeView;
    NNotebookView *notebookTreeView;
    NTrashTree *trashTree;
    NTagView *tagTreeView;
    NTabWidget *tabWindow;
    QSplitter *mainSplitter;
    QSplitter *leftPanelSplitter;
    WidgetPanel *leftPanel;
    QSplitter *rightPanelSplitter;
    QScrollArea *leftScroll;
    QWidget *topRightWidget;
    QVBoxLayout *topRightLayout;
    NAttributeTree *attributeTree;
    FindReplace *findReplaceWindow;
    bool finalSync;
    QSystemTrayIcon *trayIcon;
    QString saveLastPath;   // Last path viewed in the restore dialog
    FileWatcherManager *importManager;
    OAuthWindow *oauthWindow;
    Thumbnailer *hammer;

    // Tool & menu bar
    NMainMenuBar *menuBar;
    QMenu   *trayIconContextMenu;
    QToolBar *toolBar;
    QAction *leftArrowButton;
    QAction *rightArrowButton;
    QAction *homeButton;
    QAction *syncButton;
    QAction *printNoteButton;
    QAction *deleteNoteButton;
    QAction *newNoteButton;
    QAction *newWebcamNoteButton;
    QAction *screenCaptureButton;
    QAction *newExternalNoteButton;
    QAction *trunkButton;
    QAction *usageButton;
    QAction *toolsAccountAction;

    QAction *showAction;
    QAction *minimizeToTrayAction;
    QAction *closeToTrayAction;
    QAction *closeAction;
    bool minimizeToTray;
    bool closeToTray;
    bool closeFlag;
    bool unhidingWindow;

    // Sync Button rotate
    QTimer syncButtonTimer;
    QTimer syncTimer;
    QList<QPixmap> syncIcons;
    unsigned int synchronizeIconAngle;

    QTimer heartbeatTimer;   // Timer to check shared memory for other instance commands.

    void setupGui();
    void setupNoteList();
    void setupSearchTree();
    void setupTagTree();
    void setupAttributeTree();
    void setupTrashTree();
    void setupSynchronizedNotebookTree();
    void setupTabWindow();
    void initializeGlobalSettings();
    void waitCursor(bool value);
    void saveContents();
    void saveNoteColumnPositions();
    void saveNoteColumnWidths();
    QSplashScreen *splashScreen;

public:
    NixNote(QWidget *parent = 0);  // Constructor
    ~NixNote();   //Destructor
    SyncRunner syncRunner;
    QThread syncThread;
    QThread indexThread;
    QThread counterThread;
    IndexRunner indexRunner;
    CounterRunner counterRunner;
    void closeEvent(QCloseEvent *event);
    //bool notify(QObject* receiver, QEvent* event);
    LineEdit *searchText;
    void setDebugLevel();



public slots:
    void closeNixNote();
    void synchronize();
    void syncTimerExpired();
    void disconnect();
    void updateSyncButton();
    void syncButtonReset();
    void updateSelectionCriteria(bool afterSync=false);
    void leftButtonTriggered();
    void rightButtonTriggered();
    void openNote(bool newWindow);
    void noteImport();
    void databaseRestore(bool fullRestore=true);
    void resetView();
    void newNote();
    void newExternalNote();
    void setSyncTimer();
    void notesDeleted(QList<qint32> lid);
    void openTrunk();
    void openAccount();
    void openDatabaseStatus();
    void openAbout();
    void openImportFolders();
    void setMessage(QString msg, int timeout=15000);
    void toggleLeftPanel();
    void toggleNoteList();
    void toggleTabWindow();
    void toggleToolbar();
    void toggleStatusbar();
    void findReplaceInNote();
    void findReplaceAllInNotePressed();
    void findReplaceInNotePressed();
    void findInNote();
    void findNextInNote();
    void findPrevInNote();
    void viewNoteHistory();
    void findReplaceWindowHidden();
    void checkReadOnlyNotebook();
    void heartbeatTimerTriggered();
    void notesRestored(QList<qint32>);
    void printNote();
    void fastPrintNote();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void toggleVisible();
    void trayIconBehavior();
    void changeEvent(QEvent *e);
    void openPreferences();
    void notifySyncComplete();
    void addAnotherUser();
    void switchUser();
    void userMaintenance();
    void viewNoteListWide();
    void viewNoteListNarrow();
    void resourceExternallyUpdated(QString resource);
    void screenCapture();
    void reindexDatabase();
    void noteSynchronized(qint32 lid, bool value);
    void indexThreadStarted();
    void syncThreadStarted();
    void counterThreadStarted();
    void openCloseNotebooks();
    void newWebcamNote();
    void deleteCurrentNote();
    void duplicateCurrentNote();
    void pinCurrentNote();
    void unpinCurrentNote();
    void spellCheckCurrentNote();
    void openExternalNote(qint32 lid);
    void pauseIndexing();

signals:
    void syncRequested();
    void updateCounts();
};

#endif // NIXNOTE_H

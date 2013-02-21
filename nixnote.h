#ifndef NIXNOTE_H
#define NIXNOTE_H

#include <QtGui/QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QSystemTrayIcon>
#include <QToolBar>

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
    QSystemTrayIcon *trayIcon;
    QString saveLastPath;   // Last path viewed in the restore dialog
    FileWatcherManager *importManager;

    // Tool & menu bar
    NMainMenuBar *menuBar;
    QMenu   *trayIconContextMenu;
    QToolBar *toolBar;
    QAction *leftArrowButton;
    QAction *rightArrowButton;
    QAction *homeButton;
    QAction *syncButton;
    QAction *newNoteButton;
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


public:
    NixNote(QWidget *parent = 0);  // Constructor
    ~NixNote();   //Destructor
    SyncRunner syncRunner;
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
    void updateSelectionCriteria();
    void leftButtonTriggered();
    void rightButtonTriggered();
    void openNote(bool newWindow);
    void databaseRestore();
    void resetView();
    void newNote();
    void setSyncTimer();
    void notesDeleted(QList<qint32> lid);
    void openTrunk();
    void openAccount();
    void openDatabaseStatus();
    void openAbout();
    void openImportFolders();
    void setMessage(QString msg);
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
    void findReplaceWindowHidden();
    void checkReadOnlyNotebook();
    void heartbeatTimerTriggered();
    void notesRestored(QList<qint32>);
    void printNote();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void toggleVisible();
    void trayIconBehavior();
    void changeEvent(QEvent *e);
    void openPreferences();
    void notifySyncComplete();
    void addAnotherUser();
    void switchUser();

signals:
    void syncRequested();
    void updateCounts();

};

#endif // NIXNOTE_H

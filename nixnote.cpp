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

#include "nixnote.h"
#include "threads/syncrunner.h"
#include "gui/nwebview.h"
#include "watcher/filewatcher.h"
#include "dialog/accountdialog.h"
#include "dialog/preferences/preferencesdialog.h"
#include "dialog/webcamcapturedialog.h"
#include "sql/resourcetable.h"
#include "sql/nsqlquery.h"
#include "dialog/logviewer.h"
#include "filters/filtercriteria.h"
#include "filters/filterengine.h"

#include <QThread>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QDesktopServices>
#include <QClipboard>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QStatusBar>
#include <QSlider>
#include <QPrinter>
#include <QDesktopWidget>
#include <QFileIconProvider>
#include <QSplashScreen>

#include "sql/notetable.h"
#include "dialog/screencapture.h"
#include "gui/ntabwidget.h"
#include "sql/notebooktable.h"
#include "sql/usertable.h"
#include "settings/startupconfig.h"
#include "dialog/logindialog.h"
#include "dialog/closenotebookdialog.h"
#include "gui/lineedit.h"
#include "gui/findreplace.h"
#include "gui/nattributetree.h"
#include "dialog/watchfolderdialog.h"
#include "dialog/notehistoryselect.h"
#include "gui/ntrashtree.h"
#include "filters/filterengine.h"
#include "global.h"
#include "html/enmlformatter.h"
//#include "oauth/oauthwindow.h"
//#include "oauth/oauthtokenizer.h"
#include "dialog/databasestatus.h"
#include "dialog/adduseraccountdialog.h"
#include "dialog/accountmaintenancedialog.h"
#include "communication/communicationmanager.h"
#include "utilities/encrypt.h"


#include "gui/nmainmenubar.h"
#include "dialog/logindialog.h"
#include "xml/importdata.h"
#include "xml/importenex.h"
#include "xml/exportdata.h"
#include "dialog/aboutdialog.h"

#include "qevercloud/include/QEverCloudOAuth.h"

using namespace qevercloud;

extern Global global;
class SyncRunner;

//*************************************************
//* This is the main class that is used to start
//* everything else.
//*************************************************
NixNote::NixNote(QWidget *parent) : QMainWindow(parent)
{
    splashScreen = new QSplashScreen(this, QPixmap(":splash_logo.png"));
    global.settings->beginGroup("Appearance");
    if(global.settings->value("showSplashScreen", false).toBool() && !global.syncAndExit) {
        splashScreen->show();
        QTimer::singleShot(2500, splashScreen, SLOT(close()));
    }
    global.settings->endGroup();


    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    this->setDebugLevel();

    QTranslator *nixnoteTranslator = new QTranslator();
    QLOG_DEBUG() << "Looking for transaltions: " << global.fileManager.getTranslateFilePath("nixnote2_" + QLocale::system().name() + ".qm");
    nixnoteTranslator->load(global.fileManager.getTranslateFilePath("nixnote2_" + QLocale::system().name() + ".qm"));
    QApplication::instance()->installTranslator(nixnoteTranslator);


    connect(&syncThread, SIGNAL(started()), this, SLOT(syncThreadStarted()));
    connect(&counterThread, SIGNAL(started()), this, SLOT(counterThreadStarted()));
    connect(&indexThread, SIGNAL(started()), this, SLOT(indexThreadStarted()));
    counterThread.start(QThread::LowestPriority);
    syncThread.start(QThread::LowPriority);
    indexThread.start(QThread::LowestPriority);
    this->thread()->setPriority(QThread::HighestPriority);

    heartbeatTimer.setInterval(1000);
    heartbeatTimer.setSingleShot(false);
    connect(&heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeatTimerTriggered()));
    heartbeatTimer.start();

    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    db = new DatabaseConnection("nixnote");  // Startup the database
    QLOG_TRACE() << "Setting up global settings";
    this->initializeGlobalSettings();

    // Setup the sync thread
    QLOG_TRACE() << "Setting up counter thread";
    connect(this, SIGNAL(updateCounts()), &counterRunner, SLOT(countAll()));

    // Setup the counter thread
    QLOG_TRACE() << "Setting up sync thread";
    connect(this,SIGNAL(syncRequested()),&syncRunner,SLOT(synchronize()));
    connect(&syncRunner, SIGNAL(setMessage(QString, int)), this, SLOT(setMessage(QString, int)));

    QLOG_TRACE() << "Setting up GUI";
    global.filterPosition = 0;
    this->setupGui();
    this->openNote(false);

    QLOG_TRACE() << "Connecting signals";
    connect(favoritesTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(tagTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(notebookTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(searchTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(attributeTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(trashTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(searchText, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    // Setup file watcher
    importManager = new FileWatcherManager(this);
    connect(importManager, SIGNAL(fileImported(qint32,qint32)), this, SLOT(updateSelectionCriteria()));
    importManager->setup();
    connect(&global.resourceWatcher, SIGNAL(fileChanged(QString)), this, SLOT(resourceExternallyUpdated(QString)));

    hammer = new Thumbnailer(global.db);
    hammer->startTimer(2,120);
    finalSync = false;


    // Setup reminders
    global.reminderManager = new ReminderManager();
    global.reminderManager = new ReminderManager();
    global.reminderManager->trayIcon = trayIcon;
    global.reminderManager->reloadTimers();

    global.settings->beginGroup("Appearance");
    bool showMissed = global.settings->value("showMissedReminders", false).toBool();
    global.settings->endGroup();
    if (showMissed)
        QTimer::singleShot(5000, global.reminderManager, SLOT(timerPop()));
    else
        global.setLastReminderTime(QDateTime::currentMSecsSinceEpoch());


    // Check for Java and verify encryption works
    QString test = "Test Message";
    QString  result;
    EnCrypt encrypt;
    if (!encrypt.encrypt(result, test, test)) {
        if (!encrypt.decrypt(result, result, test)) {
            if (result == test)
                global.javaFound = true;
        }
    }

    // Check if Tidy is installed
    QProcess tidyProcess;
    tidyProcess.start("tidy -h");
    tidyProcess.waitForFinished();
    if (tidyProcess.exitCode()) {
        QMessageBox mb;
        mb.critical(this, tr("Tidy Not Found"), tr("Tidy is not found on this system.\nUntil tidy is installed you cannot save any notes."));
    }

    if (global.startupNewNote) {
        this->showMinimized();
        this->newExternalNote();
    }

    // Init OAuth winwod
    //oauthWindow = NULL;

    //QDesktopServices::setUrlHandler("evernote", this, "showDesktopUrl");
    remoteQuery = new RemoteQuery();

    QLOG_DEBUG() << "Exiting NixNote constructor";
}





// Destructor to call when all done
NixNote::~NixNote()
{
    delete splashScreen;
    syncThread.quit();
    indexThread.quit();
    counterThread.quit();
    while (!syncThread.isFinished());
    while (!indexThread.isFinished());
    while(!counterThread.isFinished());

    // Cleanup any temporary files
    QDir myDir(global.fileManager.getTmpDirPath());
    QStringList list = myDir.entryList();
    for (int i=0; i<list.size(); i++) {
        if (list[i] != "." && list[i] != "..") {
            QString file = global.fileManager.getTmpDirPath()+ list[i];
            myDir.remove(file);
        }
    }
//    delete db;  // Free up memory used by the database connection
//    delete rightPanelSplitter;
//    delete leftPanelSplitter;
//    delete leftPanel;
}



//****************************************************************
//* Setup the user interface
//****************************************************************
void NixNote::setupGui() {
    // Setup the GUI
    //this->setStyleSheet("background-color: white;");
    //statusBar();    setWindowTitle(tr("NixNote 2"));
    setWindowIcon(QIcon(global.getWindowIcon()));

    //QLOG_TRACE() << "Setting up menu bar";
    searchText = new LineEdit();
    menuBar = new NMainMenuBar(this);
    setMenuBar(menuBar);

    QLOG_TRACE() << "Setting up tool bar";
    toolBar = addToolBar(tr("ToolBar"));
    //menuBar = new NMainMenuBar(this);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setObjectName("toolBar");
    //toolBar->addWidget(menuBar);
    menuBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    toolBar->setFloatable(true);
    toolBar->setMovable(true);
    toolBar->setFont(global.getGuiFont(toolBar->font()));
    toolBar->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
    //toolBar->addSeparator();
    leftArrowButton = toolBar->addAction(QIcon(":left_arrow.png"), tr("Back"));
    rightArrowButton = toolBar->addAction(QIcon(":right_arrow.png"), tr("Next"));

    leftArrowButton->setEnabled(false);
    leftArrowButton->setPriority(QAction::LowPriority);
    connect(leftArrowButton, SIGNAL(triggered(bool)),this, SLOT(leftButtonTriggered()));
    rightArrowButton->setEnabled(false);
    rightArrowButton->setPriority(QAction::LowPriority);
    connect(rightArrowButton, SIGNAL(triggered(bool)),this, SLOT(rightButtonTriggered()));
    toolBar->addSeparator();

    homeButton = toolBar->addAction(QIcon(":home.png"), tr("All Notes"));
//    homeButton->setPriority(QAction::LowPriority);   Hide the text by the icon
    syncButton = toolBar->addAction(QIcon(":synchronize.png"), tr("Sync"));
//  syncButton->setPriority(QAction::LowPriority);   // Hide the text by the icon
    toolBar->addSeparator();
    printNoteButton = toolBar->addAction(QIcon(":printer.png"), tr("Print"));
    noteButton = new QToolButton();
    toolBar->addSeparator();
    newNoteButton = new QAction(noteButton);
    newNoteButton->setIcon(QIcon(":newNote.png"));
    newNoteButton->setText(tr("New Text Note"));
    newWebcamNoteButton = new QAction(noteButton);
    newWebcamNoteButton->setIcon(QIcon(":webcam.png"));
    newWebcamNoteButton->setText(tr("New Webcam Note"));
    noteButton->addAction(newNoteButton);
    noteButton->addAction(newWebcamNoteButton);
    noteButton->setText(newNoteButton->text());
    noteButton->setIcon(newNoteButton->icon());
    noteButton->setProperty("currentNoteButton", NewTextNote);
    noteButton->setPopupMode(QToolButton::MenuButtonPopup);
    noteButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(noteButton, SIGNAL(clicked()), this, SLOT(noteButtonClicked()));
    toolBar->addWidget(noteButton);
    toolBar->addSeparator();

    deleteNoteButton = toolBar->addAction(QIcon(":delete.png"), tr("Delete"));

    toolBar->addSeparator();
    trunkButton = toolBar->addAction(QIcon(":trunk.png"), tr("Trunk"));
    trunkButton->setVisible(false);
    usageButton = toolBar->addAction(QIcon(":usage.png"), tr("Usage"));


    connect(syncButton,SIGNAL(triggered()), this, SLOT(synchronize()));
    connect(homeButton, SIGNAL(triggered()), this, SLOT(resetView()));
    connect(printNoteButton, SIGNAL(triggered()), this, SLOT(fastPrintNote()));
    connect(deleteNoteButton, SIGNAL(triggered()), this, SLOT(deleteCurrentNote()));
    connect(trunkButton, SIGNAL(triggered()), this, SLOT(openTrunk()));
    connect(newNoteButton, SIGNAL(triggered()), this, SLOT(newNote()));
    connect(newWebcamNoteButton, SIGNAL(triggered()), this, SLOT(newWebcamNote()));
    connect(newNoteButton, SIGNAL(triggered()), this, SLOT(noteButtonClicked()));
    connect(usageButton, SIGNAL(triggered()), this, SLOT(openAccount()));

    QLOG_TRACE() << "Adding main splitter";
    mainSplitter = new QSplitter(Qt::Horizontal);
    setCentralWidget(mainSplitter);

    rightPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanel = new WidgetPanel();

    this->setupNoteList();
    this->setupFavoritesTree();
    this->setupSynchronizedNotebookTree();
    this->setupTagTree();
    this->setupSearchTree();
    this->setupAttributeTree();
    this->setupTrashTree();
    this->setupTabWindow();
    leftPanel->vboxLayout->addStretch();

    connect(tagTreeView, SIGNAL(tagDeleted(qint32,QString)), favoritesTreeView, SLOT(itemExpunged(qint32, QString)));
    connect(searchTreeView, SIGNAL(searchDeleted(qint32)), favoritesTreeView, SLOT(itemExpunged(qint32)));
    connect(notebookTreeView, SIGNAL(notebookDeleted(qint32,QString)), favoritesTreeView, SLOT(itemExpunged(qint32, QString)));
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), favoritesTreeView, SLOT(itemRenamed(qint32, QString, QString)));
    connect(searchTreeView, SIGNAL(searchDeleted(qint32)), favoritesTreeView, SLOT(itemExpunged(qint32)));
    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), favoritesTreeView, SLOT(itemRenamed(qint32, QString, QString)));
    connect(notebookTreeView, SIGNAL(stackDeleted(QString)), favoritesTreeView, SLOT(stackExpunged(QString)));
    connect(notebookTreeView, SIGNAL(stackRenamed(QString,QString)), favoritesTreeView, SLOT(stackRenamed(QString, QString)));

    QLOG_TRACE() << "Setting up left panel";
    leftPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    leftScroll = new QScrollArea();
    leftScroll->setWidgetResizable(true);
    leftScroll->setWidget(leftPanel);
    leftScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leftScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mainSplitter->insertWidget(0,leftScroll);
    mainSplitter->addWidget(rightPanelSplitter);
    mainSplitter->setStretchFactor(0,1);
    mainSplitter->setStretchFactor(1,3);

    QLOG_TRACE() << "Resetting left side widgets";
    tagTreeView->resetSize();
    searchTreeView->resetSize();
    attributeTree->resetSize();
    trashTree->resetSize();

    // Restore the window state
    global.startMinimized = false;
    QLOG_TRACE() << "Restoring window state";
    global.settings->beginGroup("Appearance");
    int selectionBehavior = global.settings->value("startupNotebook", AppearancePreferences::UseLastViewedNotebook).toInt();
    global.startMinimized = global.settings->value("startMinimized", false).toBool();
    global.settings->endGroup();

    global.settings->beginGroup("SaveState");
    bool showStatusbar = global.settings->value("statusBar", true).toBool();
    if (showStatusbar) {
        menuBar->viewStatusbar->setChecked(showStatusbar);
        statusBar()->setVisible(true);
    } else {
        menuBar->viewStatusbar->setChecked(false);
        statusBar()->setVisible(false);
    }
    restoreState(global.settings->value("WindowState").toByteArray());
    restoreGeometry(global.settings->value("WindowGeometry").toByteArray());
    mainSplitter->restoreState(global.settings->value("mainSplitter", 0).toByteArray());
    rightPanelSplitter->restoreState(global.settings->value("rightSplitter", 0).toByteArray());
    if (global.settings->value("isMaximized", false).toBool())
        this->setWindowState(Qt::WindowMaximized);
    QString lidListString = global.settings->value("openTabs", "").toString().trimmed();
    bool value = global.settings->value("leftPanelVisible", true).toBool();
    if (!value) {
        menuBar->viewLeftPanel->setChecked(false);
        leftScroll->setVisible(false);
    }
    value = global.settings->value("noteListVisible", true).toBool();
    if (!value) {
        menuBar->viewNoteList->setChecked(false);
        topRightWidget->setVisible(false);
    }
    value = global.settings->value("tabWindowVisible", true).toBool();
    if (!value) {
        menuBar->viewNotePanel->setChecked(false);
        tabWindow->setVisible(false);
    }


    global.settings->endGroup();

    if (rightPanelSplitter->orientation() == Qt::Vertical)
        viewNoteListWide();
    else
        viewNoteListNarrow();

    QStringList lidList = lidListString.split(' ');
    // If we have old notes we were viewing the last time
    if (lidList.size() > 0) {
        FilterCriteria *filter = global.filterCriteria[global.filterPosition];

        for (int i=0; i<lidList.size(); i++) {
            // if we are doing multiple notes, they each need
            // to be added to the selection criteria.
            if (i>0)
                filter = new FilterCriteria();
            int lid = lidList[i].toInt();
            filter->setLid(lid);
            if (i>0)
                global.filterCriteria.append(filter);
        }

        for (int i=0; i<lidList.size(); i++) {
            global.filterPosition = i;
            if(i==0)
                openNote(false);
            else
                openNote(true);
        }
    }

    NoteTable noteTable(global.db);
    if (global.startupNote > 0 && noteTable.exists(global.startupNote)) {
        openExternalNote(global.startupNote);
    }

    // Setup the tray icon
    closeFlag = false;
    global.settings->beginGroup("SaveState");
    minimizeToTray = global.settings->value("minimizeToTray", false).toBool();
    closeToTray = global.settings->value("closeToTray", false).toBool();
    global.settings->endGroup();
    QLOG_DEBUG() << global.getWindowIcon();
    //trayIcon = new QSystemTrayIcon(QIcon(global.getWindowIcon()), this);
    trayIcon = new QSystemTrayIcon(QIcon(":trayicon.png"), this);
    trayIconContextMenu = new TrayMenu(this);
    trayIconContextMenu->addAction(newNoteButton);

    newExternalNoteButton = trayIconContextMenu->addAction(tr("Quick Note"));
    connect(newExternalNoteButton, SIGNAL(triggered()), this, SLOT(newExternalNote()));

    screenCaptureButton = new QAction(tr("Screen Capture"), this);
    trayIconContextMenu->addAction(screenCaptureButton);
    screenCaptureButton->setIcon(QIcon(":screenCapture.png"));
    noteButton->addAction(screenCaptureButton);
    connect(screenCaptureButton, SIGNAL(triggered()), this, SLOT(screenCapture()));

    trayIconContextMenu->addSeparator();
    QMenu *pinnedMenu = trayIconContextMenu->addMenu(tr("Pinned Notes"));
    trayIconContextMenu->setActionMenu(TrayMenu::PinnedMenu, pinnedMenu);
    QMenu *recentMenu = trayIconContextMenu->addMenu(tr("Recently Updated Notes"));
    trayIconContextMenu->setActionMenu(TrayMenu::RecentMenu, recentMenu);
    connect(trayIconContextMenu, SIGNAL(openNote(qint32)), this, SLOT(openExternalNote(qint32)));
    trayIconContextMenu->addSeparator();

    showAction = trayIconContextMenu->addAction(tr("Show/Hide"));
    if (!global.showTrayIcon() || global.forceNoStartMimized || !QSystemTrayIcon::isSystemTrayAvailable()) {
        closeToTray = false;
        minimizeToTray = false;
    }

    trayIconContextMenu->addSeparator();
    closeAction = trayIconContextMenu->addAction(tr("Close"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeNixNote()));
    connect(showAction, SIGNAL(triggered()), this, SLOT(toggleVisible()));

    trayIcon->setContextMenu(trayIconContextMenu);
    trayIcon->setVisible(global.showTrayIcon());
    if (trayIcon->isVisible())
        trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));



    // Setup timers
    QLOG_TRACE() << "Setting up timers";
    setSyncTimer();
    connect(&syncTimer, SIGNAL(timeout()), this, SLOT(syncTimerExpired()));
    connect(&syncButtonTimer, SIGNAL(timeout()), this, SLOT(updateSyncButton()));
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(syncButtonReset()));

    QLOG_TRACE() << "Setting up more connections for tab windows & threads";
    // Setup so we refresh whenever the sync is done.
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(updateSelectionCriteria()));
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(notifySyncComplete()));

    // connect so we refresh the note list and counts whenever a note has changed
    connect(tabWindow, SIGNAL(noteUpdated(qint32)), noteTableView, SLOT(refreshData()));
    connect(tabWindow, SIGNAL(noteUpdated(qint32)), &counterRunner, SLOT(countNotebooks()));
    connect(tabWindow, SIGNAL(noteUpdated(qint32)), &counterRunner, SLOT(countTags()));
    connect(tabWindow, SIGNAL(updateNoteList(qint32, int, QVariant)), noteTableView, SLOT(refreshCell(qint32, int, QVariant)));
    connect(noteTableView, SIGNAL(refreshNoteContent(qint32)), tabWindow, SLOT(refreshNoteContent(qint32)));
    connect(noteTableView, SIGNAL(saveAllNotes()), tabWindow, SLOT(saveAllNotes()));

    // connect so we refresh the tag tree when a new tag is added
    connect(tabWindow, SIGNAL(tagCreated(qint32)), tagTreeView, SLOT(addNewTag(qint32)));
    connect(tabWindow, SIGNAL(tagCreated(qint32)), &counterRunner, SLOT(countTags()));

    connect(tabWindow, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(tabWindow->tabBar, SIGNAL(currentChanged(int)), this, SLOT(checkReadOnlyNotebook()));

    // Finish by filtering & displaying the data
    //updateSelectionCriteria();

    // connect signal on a tag rename
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), this, SLOT(updateSelectionCriteria()));
    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), this, SLOT(updateSelectionCriteria()));

    // Reload saved selection criteria
    if (selectionBehavior != AppearancePreferences::UseAllNotebooks) {
        bool criteriaFound = false;
        FilterCriteria *criteria = new FilterCriteria();

        // Restore whatever they were looking at in the past
        if (selectionBehavior == AppearancePreferences::UseLastViewedNotebook) {

            global.settings->beginGroup("SaveState");
            qint32 notebookLid = global.settings->value("selectedNotebook", 0).toInt();
            if (notebookLid > 0 && notebookTreeView->dataStore[notebookLid] != NULL) {
                criteria->setNotebook(*notebookTreeView->dataStore[notebookLid]);
                criteriaFound = true;
            } else {
                QString selectedStack = global.settings->value("selectedStack", "").toString();
                if (selectedStack != "" && notebookTreeView->stackStore[selectedStack] != NULL) {
                    criteria->setNotebook(*notebookTreeView->stackStore[selectedStack]);
                    criteriaFound = true;
                }
            }

            QString prevSearch = global.settings->value("searchString", "").toString();
            if (prevSearch != "") {
                searchText->setText(prevSearch);
                criteria->setSearchString(prevSearch);
                criteriaFound = true;
            }

            qint32 searchLid = global.settings->value("selectedSearch", 0).toInt();
            if (searchLid > 0 && searchTreeView->dataStore[searchLid] != NULL) {
                criteria->setSavedSearch(*searchTreeView->dataStore[searchLid]);
                criteriaFound = true;
            }

            QString selectedTags = global.settings->value("selectedTags", "").toString();
            if (selectedTags != "") {
                QStringList tags = selectedTags.split(" ");
                QList<QTreeWidgetItem *> items;
                for (int i=0; i<tags.size(); i++) {
                    if (tagTreeView->dataStore[tags[i].toInt()] != NULL)
                        items.append(tagTreeView->dataStore[tags[i].toInt()]);
                }
                criteriaFound = true;
                criteria->setTags(items);
            }
            QString expandedTags = global.settings->value("expandedTags", "").toString();
            if (expandedTags != "") {
                QStringList tags = expandedTags.split(" ");
                for (int i=0; i<tags.size(); i++) {
                    NTagViewItem *item;
                    item = tagTreeView->dataStore[tags[i].toInt()];
                    if (item != NULL)
                        item->setExpanded(true);
                }
            }
            QString expandedNotebooks = global.settings->value("expandedStacks", "").toString();
            if (expandedNotebooks != "") {
                QStringList books = expandedNotebooks.split(" ");
                for (int i=0; i<books.size(); i++) {
                    NNotebookViewItem *item;
                    item = notebookTreeView->dataStore[books[i].toInt()];
                    if (item != NULL && item->stack != "" && item->parent() != NULL)
                        item->parent()->setExpanded(true);
                }
            }


            global.settings->endGroup();
        }

        // Select the default notebook
        if (selectionBehavior == AppearancePreferences::UseDefaultNotebook) {
            NotebookTable ntable(global.db);
            qint32 lid = ntable.getDefaultNotebookLid();
            if (notebookTreeView->dataStore[lid] != NULL) {
                criteria->setNotebook(*notebookTreeView->dataStore[lid]);
                criteriaFound = true;
            }
        }



        // If we have some filter criteria, save it.  Otherwise delete
        // the unused memory.
        if (criteriaFound) {
            global.filterPosition++;
            global.appendFilter(criteria);
        } else
            delete criteria;
    }

    this->updateSelectionCriteria();
    // Set default focuse to the editor window
    tabWindow->currentBrowser()->editor->setFocus();

    QStringList accountNames = global.accountsManager->nameList();
    QList<int> ids = global.accountsManager->idList();
    for (int i=0; i<ids.size(); i++) {
        if (ids[i] == global.accountsManager->currentId) {
            setWindowTitle("NixNote - " +accountNames[i]);
            i=ids.size();
        }
    }

    // Determine if we should start minimized
    QLOG_DEBUG() << "isSystemTrayAvailable:" << QSystemTrayIcon::isSystemTrayAvailable();
    if (global.startMinimized && !global.forceNoStartMimized && QSystemTrayIcon::isSystemTrayAvailable()) {
        if (minimizeToTray)
            this->hide();
        else
            this->setWindowState(Qt::WindowMinimized);
    }
    if (global.forceStartMinimized) {
        if (minimizeToTray)
            this->hide();
        else
            this->setWindowState(Qt::WindowMinimized);
    }

}


//******************************************************************************
//* This initializes some environment options based upon the command line
//******************************************************************************
void NixNote::initializeGlobalSettings() {

}

//**************************************************************
//* Move sync, couter, & index objects to their appropriate
//* thread.
//**************************************************************
void NixNote::counterThreadStarted() {
    counterRunner.moveToThread(&counterThread);
}

void NixNote::syncThreadStarted() {
    syncRunner.moveToThread(&syncThread);
    global.settings->beginGroup("Sync");
    bool syncOnStartup = global.settings->value("syncOnStartup", false).toBool();
    global.showGoodSyncMessagesInTray = global.settings->value("showGoodSyncMessagesInTray", true).toBool();
    global.settings->endGroup();
    if (syncOnStartup || global.syncAndExit)
        synchronize();
}

void NixNote::indexThreadStarted() {
    indexRunner.moveToThread(&indexThread);
    indexRunner.initialize();
}


//******************************************************************************
//* This function sets up the note list window.  This is what the users select
//* view a specific note
//******************************************************************************
void NixNote::setupNoteList() {
    QLOG_TRACE() << "Starting NixNote.setupNoteList()";

//    global.settings->beginGroup("SaveState");
//    int value = global.settings->value("listView", 1).toInt();
//    global.settings->endGroup();
//    if (value == 1)
//        global.listView = Global::ListViewWide;
//    else
//        global.listView= Global::listViewNarrow;

    // Setup a generic widget to hold the search & note table
    topRightWidget  = new QWidget(this);
    topRightLayout = new QVBoxLayout();
    topRightLayout->addWidget(searchText);
    topRightWidget->setLayout(topRightLayout);
    noteTableView = new NTableView();
    topRightLayout->addWidget(noteTableView);
    topRightLayout->setContentsMargins(QMargins(0,0,0,0));

    // Add the generic widget
    if (global.listView == Global::ListViewWide)
        rightPanelSplitter->addWidget(topRightWidget);
    else
        mainSplitter->addWidget(topRightWidget);

//    noteTableView->contextMenu->insertAction(noteTableView->deleteNoteAction, newNoteButton);
//    noteTableView->contextMenu->insertSeparator(noteTableView->deleteNoteAction);

    connect(noteTableView, SIGNAL(newNote()), this, SLOT(newNote()));
    connect(noteTableView, SIGNAL(notesDeleted(QList<qint32>,bool)), this, SLOT(notesDeleted(QList<qint32>)));
    connect(noteTableView, SIGNAL(notesRestored(QList<qint32>)), this, SLOT(notesRestored(QList<qint32>)));
    connect(&syncRunner, SIGNAL(syncComplete()), noteTableView, SLOT(refreshData()));
    connect(&syncRunner, SIGNAL(noteSynchronized(qint32, bool)), this, SLOT(noteSynchronized(qint32, bool)));

    QLOG_TRACE() << "Leaving NixNote.setupNoteList()";
}


void NixNote::noteSynchronized(qint32 lid, bool value) {
    noteTableView->refreshCell(lid, NOTE_TABLE_IS_DIRTY_POSITION, value);
}


//*****************************************************************************
//* This function sets up the user's search tree
//*****************************************************************************
void NixNote::setupSearchTree() {
    QLOG_TRACE() << "Starting NixNote.setupSearchTree()";
    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);
    searchTreeView = new NSearchView(leftPanel);
    leftPanel->addWidget(searchTreeView);
    connect(&syncRunner, SIGNAL(searchUpdated(qint32, QString)), searchTreeView, SLOT(searchUpdated(qint32, QString)));
    connect(&syncRunner, SIGNAL(searchExpunged(qint32)), searchTreeView, SLOT(searchExpunged(qint32)));
    //connect(&syncRunner, SIGNAL(syncComplete()),searchTreeView, SLOT(re);
    QLOG_TRACE() << "Exiting NixNote.setupSearchTree()";
}


//*****************************************************************************
//* This function sets up the user's tag tree
//*****************************************************************************
void NixNote::setupTagTree() {
    QLOG_TRACE() << "Starting NixNote.setupTagTree()";
    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);
    tagTreeView = new NTagView(leftPanel);
    leftPanel->addWidget(tagTreeView);
    connect(&syncRunner, SIGNAL(tagUpdated(qint32, QString, QString, qint32)),tagTreeView, SLOT(tagUpdated(qint32, QString, QString, qint32)));
    connect(&syncRunner, SIGNAL(tagExpunged(qint32)), tagTreeView, SLOT(tagExpunged(qint32)));
    connect(&syncRunner, SIGNAL(syncComplete()),tagTreeView, SLOT(rebuildTree()));
    connect(&counterRunner, SIGNAL(tagTotals(qint32,qint32, qint32)), tagTreeView, SLOT(updateTotals(qint32,qint32, qint32)));
    connect(&counterRunner, SIGNAL(tagCountComplete()), tagTreeView, SLOT(hideUnassignedTags()));
    connect(notebookTreeView, SIGNAL(notebookSelectionChanged(qint32)), tagTreeView, SLOT(notebookSelectionChanged(qint32)));
    connect(tagTreeView, SIGNAL(updateNoteList(qint32,int,QVariant)), noteTableView, SLOT(refreshCell(qint32,int,QVariant)));
    connect(tagTreeView, SIGNAL(updateCounts()), &counterRunner, SLOT(countAll()));
    QLOG_TRACE() << "Exiting NixNote.setupTagTree()";
}



//*****************************************************************************
//* This function sets up the attribute search tree
//*****************************************************************************
void NixNote::setupAttributeTree() {
    QLOG_TRACE() << "Starting NixNote.setupAttributeTree()";
    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);
    attributeTree = new NAttributeTree(leftPanel);
    leftPanel->addWidget(attributeTree);
    QLOG_TRACE() << "Exiting NixNote.setupAttributeTree()";
}


//*****************************************************************************
//* This function sets up the trash
//*****************************************************************************
void NixNote::setupTrashTree() {
    QLOG_TRACE() << "Starting NixNote.setupTrashTree()";
    trashTree = new NTrashTree(leftPanel);
    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);
    leftPanel->addWidget(trashTree);
    QLOG_TRACE() << "Exiting NixNote.setupTrashTree()";
    connect(&counterRunner, SIGNAL(trashTotals(qint32)), trashTree, SLOT(updateTotals(qint32)));
}



//*****************************************************************************
//* This function sets up the user's synchronized notebook tree
//*****************************************************************************
void NixNote::setupFavoritesTree() {
    QLOG_TRACE() << "Starting NixNote.setupFavoritesdNotebookTree()";
    favoritesTreeView = new FavoritesView(leftPanel);
    leftPanel->addWidget(favoritesTreeView);

//    connect(&syncRunner, SIGNAL(notebookUpdated(qint32, QString,QString, bool, bool)),notebookTreeView, SLOT(notebookUpdated(qint32, QString, QString, bool, bool)));
    connect(&syncRunner, SIGNAL(notebookExpunged(qint32)), favoritesTreeView, SLOT(itemExpunged(qint32)));
    connect(&syncRunner, SIGNAL(tagExpunged(qint32)), favoritesTreeView, SLOT(itemExpunged(qint32)));
//    connect(&syncRunner, SIGNAL(noteUpdated(qint32)), notebookTreeView, SLOT(itemExpunged(qint32)));
    connect(&counterRunner, SIGNAL(notebookTotals(qint32,qint32, qint32)), favoritesTreeView, SLOT(updateTotals(qint32,qint32, qint32)));
    connect(&counterRunner, SIGNAL(tagTotals(qint32,qint32,qint32)), favoritesTreeView, SLOT(updateTotals(qint32,qint32, qint32)));
    connect(favoritesTreeView, SIGNAL(updateCounts()), &counterRunner, SLOT(countAll()));

    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);

    QLOG_TRACE() << "Exiting NixNote.setupFavoritesTree()";
}



//*****************************************************************************
//* This function sets up the user's synchronized notebook tree
//*****************************************************************************
void NixNote::setupSynchronizedNotebookTree() {
    QLOG_TRACE() << "Starting NixNote.setupSynchronizedNotebookTree()";
    notebookTreeView = new NNotebookView(leftPanel);
    leftPanel->addWidget(notebookTreeView);
    connect(&syncRunner, SIGNAL(notebookUpdated(qint32, QString,QString, bool, bool)),notebookTreeView, SLOT(notebookUpdated(qint32, QString, QString, bool, bool)));
    connect(&syncRunner, SIGNAL(syncComplete()),notebookTreeView, SLOT(rebuildTree()));
    connect(&syncRunner, SIGNAL(notebookExpunged(qint32)), notebookTreeView, SLOT(notebookExpunged(qint32)));
    connect(&counterRunner, SIGNAL(notebookTotals(qint32,qint32, qint32)), notebookTreeView, SLOT(updateTotals(qint32,qint32, qint32)));
    connect(notebookTreeView, SIGNAL(updateNoteList(qint32,int,QVariant)), noteTableView, SLOT(refreshCell(qint32,int,QVariant)));
    connect(notebookTreeView, SIGNAL(updateCounts()), &counterRunner, SLOT(countAll()));
    QLOG_TRACE() << "Exiting NixNote.setupSynchronizedNotebookTree()";
}



//*****************************************************************************
//* This function sets up the tab window that is used by the browser
//*****************************************************************************
void NixNote::setupTabWindow() {
    QLOG_TRACE() << "Starting NixNote.setupTabWindow()";
    tabWindow = new NTabWidget(&syncRunner, notebookTreeView, tagTreeView);
    findReplaceWindow = new FindReplace(this);
    QWidget *tabPanel = new QWidget(this);
    tabPanel->setLayout(new QVBoxLayout());
    tabPanel->layout()->addWidget(tabWindow);
    tabPanel->layout()->addWidget(findReplaceWindow);
    rightPanelSplitter->addWidget(tabPanel);

    NBrowserWindow *newBrowser = new NBrowserWindow(this);
    connect(&syncRunner, SIGNAL(syncComplete()), &newBrowser->notebookMenu, SLOT(reloadData()));
    connect(&syncRunner, SIGNAL(syncComplete()), &newBrowser->tagEditor, SLOT(reloadTags()));
    connect(&syncRunner, SIGNAL(noteUpdated(qint32)), newBrowser, SLOT(noteSyncUpdate(qint32)));
    tabWindow->addBrowser(newBrowser, "");
    rightPanelSplitter->setStretchFactor(1,10);

    connect(noteTableView, SIGNAL(openNote(bool)), this, SLOT(openNote(bool)));
    connect(noteTableView, SIGNAL(openNoteExternalWindow(qint32)), this, SLOT(openExternalNote(qint32)));
    connect(menuBar->viewSourceAction, SIGNAL(triggered()), tabWindow, SLOT(toggleSource()));
    connect(menuBar->viewHistoryAction, SIGNAL(triggered()), this, SLOT(viewNoteHistory()));

    connect(menuBar->undoAction, SIGNAL(triggered()), tabWindow, SLOT(undoButtonPressed()));
    connect(menuBar->redoAction, SIGNAL(triggered()), tabWindow, SLOT(redoButtonPressed()));
    connect(menuBar->cutAction, SIGNAL(triggered()), tabWindow, SLOT(cutButtonPressed()));
    connect(menuBar->copyAction, SIGNAL(triggered()), tabWindow, SLOT(copyButtonPressed()));
    connect(menuBar->pasteAction, SIGNAL(triggered()), tabWindow, SLOT(pasteButtonPressed()));
    connect(menuBar->pasteAsTextAction, SIGNAL(triggered()), tabWindow, SLOT(pasteAsTextButtonPressed()));
    connect(menuBar->selectAllAction, SIGNAL(triggered()), tabWindow, SLOT(selectAllButtonPressed()));
    connect(menuBar->viewExtendedInformation, SIGNAL(triggered()), tabWindow, SLOT(viewExtendedInformation()));

    connect(findReplaceWindow->nextButton, SIGNAL(clicked()), this, SLOT(findNextInNote()));
    connect(findReplaceWindow->prevButton, SIGNAL(clicked()), this, SLOT(findPrevInNote()));
    connect(findReplaceWindow->replaceButton, SIGNAL(clicked()), this, SLOT(findReplaceInNotePressed()));
    connect(findReplaceWindow->replaceAllButton, SIGNAL(clicked()), this, SLOT(findReplaceAllInNotePressed()));
    connect(findReplaceWindow->closeButton, SIGNAL(clicked()), this, SLOT(findReplaceWindowHidden()));


    QLOG_TRACE() << "Exiting NixNote.setupTabWindow()";
}



//****************************************************************************
//* Close NixNote.   This will force a close.  Even if "close to tray" is set.
//*****************************************************************************
void NixNote::closeNixNote() {
    closeFlag = true;
    close();
}


//*****************************************************************************
//* Close the program
//*****************************************************************************
void NixNote::closeEvent(QCloseEvent *event) {
    if (closeToTray && !closeFlag) {
        event->ignore();
        hide();
        return;
    }

    saveContents();

    indexRunner.keepRunning = false;
    counterRunner.keepRunning = false;
    indexThread.quit();
    counterThread.quit();

    global.settings->beginGroup("Sync");
    bool syncOnShutdown = global.settings->value("syncOnShutdown", false).toBool();
    global.settings->endGroup();
    if (syncOnShutdown && !finalSync && global.accountsManager->oauthTokenFound()) {
        finalSync = true;
        hide();
        connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(close()));
        synchronize();
        event->ignore();
        return;
    }

    syncRunner.keepRunning = false;
    syncThread.quit();

    ConfigStore config(global.db);
    config.saveSetting(CONFIG_STORE_WINDOW_STATE, saveState());
    config.saveSetting(CONFIG_STORE_WINDOW_GEOMETRY, saveGeometry());

    QString lidList;
    QList<NBrowserWindow*> *browsers = tabWindow->browserList;
    for (int i=0;i<browsers->size(); i++) {
        lidList = lidList + QString::number(browsers->at(i)->lid) + QString(" ");
    }

    global.settings->beginGroup("SaveState");
    global.settings->setValue("WindowState", saveState());
    global.settings->setValue("WindowGeometry", saveGeometry());
    global.settings->setValue("isMaximized", isMaximized());
    global.settings->setValue("openTabs", lidList);
    global.settings->setValue("lastViewed", tabWindow->currentBrowser()->lid);
    global.settings->setValue("noteListWidth", noteTableView->width());
    global.settings->setValue("noteListHeight", noteTableView->height());
    global.settings->setValue("mainSplitter", mainSplitter->saveState());
    global.settings->setValue("rightSplitter", rightPanelSplitter->saveState());
    global.settings->setValue("listView", global.listView);

    global.settings->remove("selectedStack");
    global.settings->remove("selectedNotebook");
    global.settings->remove("expandedStacks");
    global.settings->remove("selectedTags");
    global.settings->remove("expandedTags");
    global.settings->remove("selectedSearch");
    global.settings->remove("searchString");

    // Save the current notebook/stack selection
    if (notebookTreeView->selectedItems().size() > 0) {
        NNotebookViewItem *item = (NNotebookViewItem*)notebookTreeView->selectedItems().at(0);
        qint32 saveLid = item->lid;
        if (saveLid > 0) {
            global.settings->setValue("selectedNotebook", saveLid);
        } else {
            QString saveStack = item->text(0);
            global.settings->setValue("selectedStack", saveStack);
        }
    }

    if (searchText->isSet()) {
        global.settings->setValue("searchString", searchText->text().trimmed());
    }

    if (searchTreeView->selectedItems().size() > 0) {
        NSearchViewItem *item = (NSearchViewItem*)searchTreeView->selectedItems().at(0);
        qint32 saveLid = item->data(0, Qt::UserRole).toInt();
        if (saveLid > 0) {
            global.settings->setValue("selectedSearch", saveLid);
        }
    }

    // Save any selected tags
    QString savedLids = "";
    if (tagTreeView->selectedItems().size() > 0) {
        for (int i=0; i<tagTreeView->selectedItems().size(); i++) {
            NTagViewItem *item = (NTagViewItem*)tagTreeView->selectedItems().at(i);
            qint32 saveLid = item->data(0, Qt::UserRole).toInt();
            savedLids = savedLids + QString::number(saveLid) + " ";
        }
        global.settings->setValue("selectedTags", savedLids.trimmed());
    }

    QHash<qint32, NTagViewItem*>::iterator iterator;
    savedLids = "";
    for (iterator=tagTreeView->dataStore.begin(); iterator!=tagTreeView->dataStore.end(); ++iterator) {
        if (iterator.value() != NULL) {
            qint32 saveLid = iterator.value()->data(0, Qt::UserRole).toInt();
            if (iterator.value()->isExpanded()) {
                savedLids = savedLids + QString::number(saveLid) + " ";
            }
        }
    }
    global.settings->setValue("expandedTags", savedLids.trimmed());


    QHash<qint32, NNotebookViewItem*>::iterator books;
    savedLids = "";
    for (books=notebookTreeView->dataStore.begin(); books!=notebookTreeView->dataStore.end(); ++books) {
        if (books.value() != NULL) {
            qint32 saveLid = books.value()->data(0, Qt::UserRole).toInt();
            if (books.value()->stack != "" && books.value()->parent()->isExpanded()) {
                savedLids = savedLids + QString::number(saveLid) + " ";
            }
        }
    }
    global.settings->setValue("expandedStacks", savedLids.trimmed());

    global.settings->endGroup();

    saveNoteColumnWidths();
    saveNoteColumnPositions();
    noteTableView->saveColumnsVisible();
    QMainWindow::closeEvent(event);
    QLOG_DEBUG() << "Quitting";
}




void NixNote::saveNoteColumnPositions() {
    int position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_ALTITUDE_POSITION);
    global.setColumnPosition("noteTableAltitudePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_AUTHOR_POSITION);
    global.setColumnPosition("noteTableAuthorPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_DATE_CREATED_POSITION);
    global.setColumnPosition("noteTableDateCreatedPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_DATE_DELETED_POSITION);
    global.setColumnPosition("noteTableDateDeletedPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_DATE_SUBJECT_POSITION);
    global.setColumnPosition("noteTableDateSubjectPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_DATE_UPDATED_POSITION);
    global.setColumnPosition("noteTableDateUpdatedosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_HAS_ENCRYPTION_POSITION);
    global.setColumnPosition("noteTableHasEncryptionPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_HAS_TODO_POSITION);
    global.setColumnPosition("noteTableHasTodoPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_IS_DIRTY_POSITION);
    global.setColumnPosition("noteTableIsDirtyPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_LATITUDE_POSITION);
    global.setColumnPosition("noteTableLatitudePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_LONGITUDE_POSITION);
    global.setColumnPosition("noteTableLongitudePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_NOTEBOOK_LID_POSITION);
    global.setColumnPosition("noteTableNotebookLidPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_LID_POSITION);
    global.setColumnPosition("noteTableLidPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_NOTEBOOK_POSITION);
    global.setColumnPosition("noteTableNotebookPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_SIZE_POSITION);
    global.setColumnPosition("noteTableSizePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_APPLICATION_POSITION);
    global.setColumnPosition("noteTableSourceApplicationPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_POSITION);
    global.setColumnPosition("noteTableSourcePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_SOURCE_URL_POSITION);
    global.setColumnPosition("noteTableSourceUrlPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_TAGS_POSITION);
    global.setColumnPosition("noteTableTagsPosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_TITLE_POSITION);
    global.setColumnPosition("noteTableTitlePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_REMINDER_TIME_POSITION);
    global.setColumnPosition("noteTableReminderTimePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_REMINDER_TIME_DONE_POSITION);
    global.setColumnPosition("noteTableReminderTimeDonePosition", position);
    position = noteTableView->horizontalHeader()->visualIndex(NOTE_TABLE_REMINDER_ORDER_POSITION);
    global.setColumnPosition("noteTableReminderOrderPosition", position);
}


void NixNote::saveNoteColumnWidths() {
    int width;
    width = noteTableView->columnWidth(NOTE_TABLE_ALTITUDE_POSITION);
    global.setColumnWidth("noteTableAltitudePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_AUTHOR_POSITION);
    global.setColumnWidth("noteTableAuthorPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_DATE_CREATED_POSITION);
    global.setColumnWidth("noteTableDateCreatedPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_DATE_DELETED_POSITION);
    global.setColumnWidth("noteTableDateDeletedPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_DATE_SUBJECT_POSITION);
    global.setColumnWidth("noteTableDateSubjectPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_DATE_UPDATED_POSITION);
    global.setColumnWidth("noteTableDateUpdatedPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_HAS_ENCRYPTION_POSITION);
    global.setColumnWidth("noteTableHasEncryptionPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_HAS_TODO_POSITION);
    global.setColumnWidth("noteTableHasTodoPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_IS_DIRTY_POSITION);
    global.setColumnWidth("noteTableIsDirtyPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_LATITUDE_POSITION);
    global.setColumnWidth("noteTableLatitudePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_LID_POSITION);
    global.setColumnWidth("noteTableLidPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_LONGITUDE_POSITION);
    global.setColumnWidth("noteTableLongitudePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_NOTEBOOK_LID_POSITION);
    global.setColumnWidth("noteTableNotebookLidPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_NOTEBOOK_POSITION);
    global.setColumnWidth("noteTableNotebookPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_SIZE_POSITION);
    global.setColumnWidth("noteTableSizePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_SOURCE_APPLICATION_POSITION);
    global.setColumnWidth("noteTableSourceApplicationPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_TAGS_POSITION);
    global.setColumnWidth("noteTableTagsPosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_TITLE_POSITION);
    global.setColumnWidth("noteTableTitlePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_REMINDER_TIME_POSITION);
    global.setColumnWidth("noteTableReminderTimePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_REMINDER_TIME_DONE_POSITION);
    global.setColumnWidth("noteTableReminderTimeDonePosition", width);
    width = noteTableView->columnWidth(NOTE_TABLE_REMINDER_ORDER_POSITION);
    global.setColumnWidth("noteTableReminderOrderPosition", width);
}


//*****************************************************************************
//* The sync timer has expired
//*****************************************************************************
void NixNote::syncTimerExpired() {
    // If we are already connected, we are already synchronizing so there is nothing more to do
    if (global.connected == true)
        return;
    if (!global.accountsManager->oauthTokenFound())
        return;
    tabWindow->saveAllNotes();
    emit(syncRequested());
}

//******************************************************************************
//* User synchronize was requested
//******************************************************************************
void NixNote::synchronize() {
    // If we are already connected, we are already synchronizing so there is nothing more to do
    if (global.connected == true)
        return;

    this->pauseIndexing(true);

    if (!global.accountsManager->oauthTokenFound()) {
        QString consumerKey = "baumgarr-3523";
        QString consumerSecret = "8d5ee175f8a5d3ec";
        EvernoteOAuthDialog d(consumerKey, consumerSecret, global.server);
        d.setWindowTitle(tr("Log in to Evernote"));
        if(d.exec() != QDialog::Accepted) {
            QMessageBox::critical(0, tr("NixNote"), "Login failed.\n" + d.oauthError());
            return;
        }
        QString token = QString("oauth_token=") +d.oauthResult().authenticationToken +
                       QString("&oauth_token_secret=&edam_shard=")+d.oauthResult().shardId  +
                       QString("&edam_userId=") +QString::number(d.oauthResult().userId) +
                       QString("&edam_expires=") +QString::number(d.oauthResult().expires) +
                       QString("&edam_noteStoreUrl=") + d.oauthResult().noteStoreUrl +
                       QString("&edam_webApiUrlPrefix=") +d.oauthResult().webApiUrlPrefix;

        global.accountsManager->setOAuthToken(token);
    }

    this->saveContents();
    statusBar()->clearMessage();

    tabWindow->saveAllNotes();
    syncButtonTimer.start(3);
    emit syncRequested();
}


//********************************************************************************
//* Disconnect from Evernote
//********************************************************************************
void NixNote::disconnect() {
    global.connected = false;
    menuBar->disconnectAction->setEnabled(false);
    syncButtonTimer.stop();
    pauseIndexing(false);
}




void NixNote::syncButtonReset() {
    pauseIndexing(false);
    if (syncIcons.size() == 0)
        return;
    syncButtonTimer.stop();
    syncButton->setIcon(syncIcons[0]);
}


//*****************************************************
//* Rotate the sync icon when we are connected to
//* Evernote and are transmitting & receiving info
//*****************************************************
void NixNote::updateSyncButton() {

    if (syncIcons.size() == 0) {
        double angle = 0.0;
        synchronizeIconAngle = 0;
        QPixmap pix(":synchronize.png");
        syncIcons.push_back(pix);
        for (qint32 i=0; i<=360; i++) {
            QPixmap rotatedPix(pix.size());
            QPainter p(&rotatedPix);
            rotatedPix.fill(toolBar->palette().color(QPalette::Background));
            QSize size = pix.size();
            p.translate(size.width()/2, size.height()/2);
            angle = angle+1.0;
            p.rotate(angle);
            p.setBackgroundMode( Qt::OpaqueMode);
            p.translate(-size.width()/2, -size.height()/2);
            p.drawPixmap(0,0, pix);
            p.end();
            syncIcons.push_back(rotatedPix);
        }
    }
    synchronizeIconAngle++;
    if (synchronizeIconAngle > 359)
            synchronizeIconAngle=0;
    syncButton->setIcon(syncIcons[synchronizeIconAngle]);
}



void NixNote::openNote(bool newWindow) {
    saveContents();
    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    qint32 lid;
    if (criteria->isLidSet()) {
        lid = criteria->getLid();
        if (newWindow)
            tabWindow->openNote(lid, NTabWidget::NewTab);
        else
            tabWindow->openNote(lid, NTabWidget::CurrentTab);
    } else {
        tabWindow->openNote(-1, NTabWidget::CurrentTab);
    }
    rightArrowButton->setEnabled(false);
    leftArrowButton->setEnabled(false);
    if (global.filterPosition+1 < global.filterCriteria.size())
        rightArrowButton->setEnabled(true);
    if (global.filterPosition > 0)
        leftArrowButton->setEnabled(true);
    checkReadOnlyNotebook();
}



void NixNote::openExternalNote(qint32 lid) {
    tabWindow->openNote(lid, NTabWidget::ExternalWindow);
}




//*****************************************************
//* Called when a user changes the selection criteria
//* (i.e. they select a notebook, tag, saved search...
//*****************************************************
void NixNote::updateSelectionCriteria(bool afterSync) {
    QLOG_DEBUG() << "starting NixNote.updateSelectionCriteria()";

    FilterEngine filterEngine;
    filterEngine.filter();

    QLOG_DEBUG() << "Refreshing data";

    noteTableView->refreshData();

    favoritesTreeView->updateSelection();
    tagTreeView->updateSelection();
    notebookTreeView->updateSelection();
    searchTreeView->updateSelection();
    attributeTree->updateSelection();
    trashTree->updateSelection();
    searchText->updateSelection();

    rightArrowButton->setEnabled(false);
    leftArrowButton->setEnabled(false);
    if (global.filterPosition+1 < global.filterCriteria.size())
        rightArrowButton->setEnabled(true);
    if (global.filterPosition > 0)
        leftArrowButton->setEnabled(true);

    QList<qint32> selectedNotes;
    global.filterCriteria[global.filterPosition]->getSelectedNotes(selectedNotes);
    if (selectedNotes.size() == 0) {
        tabWindow->currentBrowser()->clear();
        tabWindow->currentBrowser()->setReadOnly(true);
    }
    if (selectedNotes.size() > 0 && !afterSync) {
        tabWindow->currentBrowser()->setContent(selectedNotes.at(0));
        openNote(false);
    }

    if (global.filterCriteria[global.filterPosition]->isDeletedOnlySet() && global.filterCriteria[global.filterPosition]->getDeletedOnly())
        newNoteButton->setEnabled(false);
    else
        newNoteButton->setEnabled(true);

    emit updateCounts();
}


void NixNote::checkReadOnlyNotebook() {
    qint32 lid = tabWindow->currentBrowser()->lid;
    Note n;
    NoteTable ntable(global.db);
    NotebookTable btable(global.db);
    ntable.get(n, lid, false, false);
    qint32 notebookLid = 0;
    if (n.notebookGuid.isSet())
        notebookLid = btable.getLid(n.notebookGuid);
    if (btable.isReadOnly(notebookLid)) {
        newNoteButton->setEnabled(false);
        menuBar->deleteNoteAction->setEnabled(false);
    } else {
        newNoteButton->setEnabled(true);
        menuBar->deleteNoteAction->setEnabled(true);
    }
}


void NixNote::rightButtonTriggered() {
    global.filterPosition++;
    updateSelectionCriteria();
}


void NixNote::leftButtonTriggered() {
    global.filterPosition--;
    updateSelectionCriteria();
}


//**************************************************
// Import notes menu option chosen
//**************************************************
void NixNote::noteExport() {
    databaseBackup(false);
}



//**************************************************
//* Backup (or export) notes
//**************************************************
void NixNote::databaseBackup(bool backup) {
    QLOG_TRACE() << "Entering databaseBackup()";
    ExportData noteReader(backup);

    if (!backup) {
        noteTableView->getSelectedLids(noteReader.lids);
        if (noteReader.lids.size() == 0) {
            QMessageBox::critical(this, tr("Error"), tr("No notes selected."));
            return;
        }
    }

    QFileDialog fd;
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setConfirmOverwrite(true);
    if (backup)
        fd.setWindowTitle(tr("Backup Database"));
    else
        fd.setWindowTitle(tr("Export Notes"));
    fd.setFilter(tr("NixNote Export (*.nnex);;All Files (*.*)"));
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (saveLastPath == "")
        fd.setDirectory(QDir::homePath());
    else
        fd.setDirectory(saveLastPath);
    if (fd.exec() == 0 || fd.selectedFiles().size() == 0) {
        QLOG_DEBUG() << "Database restore canceled in file dialog.";
        return;
    }

    waitCursor(true);
    QStringList fileNames;
    fileNames = fd.selectedFiles();
    saveLastPath = fileNames[0];
    int pos = saveLastPath.lastIndexOf("/");
    if (pos > 0)
        saveLastPath.truncate(pos);

    if (backup)
        setMessage(tr("Performing backup"));
    else
        setMessage(tr("Performing export"));

    if (!fileNames[0].endsWith(".nnex")) {
        fileNames[0].append(".nnex");
    }
        noteReader.backupData(fileNames[0]);

    if (noteReader.lastError != 0) {
        setMessage(noteReader.errorMessage);
        QLOG_ERROR() <<  "Backup problem: " << noteReader.errorMessage;
        QMessageBox::critical(this, tr("Error"), noteReader.errorMessage);
        waitCursor(false);
        return;
    }


    if (backup)
        setMessage(tr("Database backup complete."));
    else
        setMessage(tr("Note extract complete."));

    waitCursor(false);

}


//**************************************************
// Import notes menu option chosen
//**************************************************
void NixNote::noteImport() {
    databaseRestore(false);
}



//**************************************************
//* Restore (or import) notes
//**************************************************
void NixNote::databaseRestore(bool fullRestore) {
    QLOG_TRACE() << "Entering databaseRestore()";

    if (fullRestore) {
        QMessageBox msgBox;
        msgBox.setText(tr("This is used to restore a database from backups.\nIt is HIGHLY recommened that this only be used to populate\nan empty database.  Restoring into a database that\n already has data can cause problems.\n\nAre you sure you want to continue?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setWindowTitle(tr("Confirm Restore"));
        int retval = msgBox.exec();

        switch (retval) {
        case QMessageBox::Cancel:    // Cancel was clicked, let's exit
            QLOG_DEBUG() << "Database restore has been canceled";
            return;
            break;
        default:  // Anything else we don't care
            break;
        }
    }

    QFileDialog fd;
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setConfirmOverwrite(true);
    if (fullRestore)
        fd.setWindowTitle(tr("Restore Database"));
    else
        fd.setWindowTitle(tr("Import Notes"));

    if (fullRestore) {
        fd.setFilter(tr("NixNote Export (*.nnex);;All Files (*.*)"));
    } else {
        fd.setFilter(tr("NixNote Export (*.nnex);;Evernote Export (*.enex);;All Files (*.*)"));
    }
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    if (saveLastPath == "")
        fd.setDirectory(QDir::homePath());
    else
        fd.setDirectory(saveLastPath);
    if (fd.exec() == 0 || fd.selectedFiles().size() == 0) {
        QLOG_DEBUG() << "Database restore canceled in file dialog.";
        return;
    }

    waitCursor(true);
    QStringList fileNames;
    fileNames = fd.selectedFiles();
    saveLastPath = fileNames[0];
    int pos = saveLastPath.lastIndexOf("/");
    if (pos > 0)
        saveLastPath.truncate(pos);

    if (fullRestore)
        setMessage(tr("Restoring database"));
    else
        setMessage(tr("Importing Notes"));

    ImportEnex enexReader;
    ImportData noteReader(fullRestore);
    if (fileNames[0].endsWith(".nnex") || fullRestore) {
        noteReader.import(fileNames[0]);

        if (noteReader.lastError != 0) {
            setMessage(noteReader.getErrorMessage());
            QLOG_ERROR() <<  "Restore problem: " << noteReader.errorMessage;
            QMessageBox::critical(this, tr("Error"), noteReader.errorMessage);
            waitCursor(false);
            return;
        }
    } else {
        fullRestore = false;
        enexReader.import(fileNames[0]);
        QLOG_DEBUG() << "Back from import";
    }

    // Finish by filtering & displaying the data
    updateSelectionCriteria();

    if (fullRestore || fileNames[0].endsWith(".enex")) {
        tagTreeView->rebuildTagTreeNeeded = true;
        tagTreeView->loadData();
        searchTreeView->loadData();
    }
    notebookTreeView->rebuildNotebookTreeNeeded = true;
    notebookTreeView->loadData();

    if (fullRestore)
        setMessage(tr("Database has been restored."));
    else
        setMessage(tr("Notes have been imported."));

    waitCursor(false);
}



//*********************************************************
//* Set wait cursor
//*********************************************************
void NixNote::waitCursor(bool value) {
    Q_UNUSED(value); /* suppress warning of unused */
}


// Show a message in the status bar
void NixNote::setMessage(QString text, int timeout) {
    statusBar()->showMessage(text, timeout);
    QLOG_INFO() << text;
}



void NixNote::notifySyncComplete() {
    bool show;
    global.settings->beginGroup("Sync");
    show = global.settings->value("enableNotification", false).toBool();
    global.settings->endGroup();
    if (!show)
        return;
    if (syncRunner.error) {
        trayIcon->showMessage(tr("Sync Error"), tr("Sync completed with errors."));
    } else
        if (global.showGoodSyncMessagesInTray)
            trayIcon->showMessage(tr("Sync Complete"), tr("Sync completed successfully."));
    if (global.syncAndExit)
        this->closeNixNote();
}

//*******************************************************
//* Check for dirty notes and save the contents
//*******************************************************
void NixNote::saveContents() {
    for (int i=0; i<tabWindow->browserList->size(); i++) {
        qint32 lid = tabWindow->browserList->at(i)->lid;
        // Check if the note is dirty
        if (tabWindow->browserList->at(i)->editor->isDirty) {
            tabWindow->browserList->at(i)->saveNoteContent();
            noteTableView->refreshCell(lid, NOTE_TABLE_IS_DIRTY_POSITION, true);
        }

    }
}


void NixNote::resetView() {
    FilterCriteria *criteria = new FilterCriteria();
    global.filterCriteria[global.filterPosition]->duplicate(*criteria);
    criteria->resetAttribute = true;
    criteria->resetDeletedOnly =true;
    criteria->resetFavorite = true;
    criteria->resetNotebook = true;
    criteria->resetSavedSearch = true;
    criteria->resetSearchString = true;
    criteria->resetTags = true;
    criteria->unsetNotebook();
    criteria->unsetDeletedOnly();
    criteria->unsetTags();
    criteria->unsetAttribute();
    criteria->unsetSavedSearch();
    criteria->unsetSearchString();
    global.filterCriteria.append(criteria);
    global.filterPosition++;
    updateSelectionCriteria();
}



// Create a new note
void NixNote::newNote() {
    if (noteButton->property("currentNoteButton").toInt() != NewTextNote) {
        noteButton->setText(newNoteButton->text());
        noteButton->setIcon(newNoteButton->icon());
        noteButton->setProperty("currentNoteButton", NewTextNote);
    }
    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\"><br/></en-note>");

    Note n;
    NotebookTable notebookTable(global.db);
    n.content = newNoteBody;
    n.title = "Untitled note";
    QString uuid = QUuid::createUuid();
    uuid = uuid.mid(1);
    uuid.chop(1);
    n.guid = uuid;
    n.active = true;
    //QDateTime now;
    n.created = QDateTime::currentMSecsSinceEpoch();
    n.updated = n.created;
    n.updateSequenceNum = 0;
    if (notebookTreeView->selectedItems().size() == 0) {
        n.notebookGuid = notebookTable.getDefaultNotebookGuid();
    } else {
        NNotebookViewItem *item = (NNotebookViewItem*)notebookTreeView->selectedItems().at(0);
        QString notebookGuid;
        notebookTable.getGuid(notebookGuid, item->lid);
        n.notebookGuid = notebookGuid;
    }
    NoteTable table(global.db);
    qint32 lid = table.add(0,n,true);

    FilterCriteria *criteria = new FilterCriteria();
    global.filterCriteria[global.filterPosition]->duplicate(*criteria);
    criteria->setLid(lid);
    global.filterCriteria.append(criteria);
    global.filterPosition++;
    openNote(false);
    updateSelectionCriteria();
    tabWindow->currentBrowser()->editor->setFocus();
}


void NixNote::newExternalNote() {
    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\"><br/></en-note>");

    Note n;
    NotebookTable notebookTable(global.db);
    n.content = newNoteBody;
    n.title = "Untitled note";
    QString uuid = QUuid::createUuid();
    uuid = uuid.mid(1);
    uuid.chop(1);
    n.guid = uuid;
    n.active = true;
    //QDateTime now;
    n.created = QDateTime::currentMSecsSinceEpoch();
    n.updated = n.created;
    n.updateSequenceNum = 0;
    if (notebookTreeView->selectedItems().size() == 0) {
        n.notebookGuid = notebookTable.getDefaultNotebookGuid();
    } else {
        NNotebookViewItem *item = (NNotebookViewItem*)notebookTreeView->selectedItems().at(0);
        QString notebookGuid;
        notebookTable.getGuid(notebookGuid, item->lid);
        n.notebookGuid = notebookGuid;
    }
    NoteTable table(global.db);
    qint32 lid = table.add(0,n,true);
    tabWindow->openNote(lid, NTabWidget::ExternalWindow);
    updateSelectionCriteria();

}

// Slot for when notes have been deleted from the notes list.
void NixNote::notesDeleted(QList<qint32> lids) {
    lids=lids;
    updateSelectionCriteria();
}

// Slot for when notes have been deleted from the notes list.
void NixNote::notesRestored(QList<qint32> lids) {
    lids=lids;
    updateSelectionCriteria();
}




// Open the trunk web site
void NixNote::openTrunk() {
    QDesktopServices::openUrl(QUrl("http://www.evernote.com/trunk"));
}


// Open Evernote support
void NixNote::openEvernoteSupport() {
    QString server = "http://www.evernote.com/about/contact/support/";
    if (global.accountsManager->getServer() == "app.yinxiang.com")
        server = "https://support.yinxiang.com";
    QDesktopServices::openUrl(QUrl(server));
}

void NixNote::openAccount() {
    AccountDialog dialog;
    dialog.exec();
}


void NixNote::openAbout() {
//    QMessageBox::about(this, tr(" NixNote 2 Alpha #5"),
//                       tr("This is ALPHA software. "
//                          "Use it at your own risk. "
//                          "\n\nLicensed under the Gnu Public License v2.\n\nBuilt on "
//                          ) + __TIMESTAMP__);
    AboutDialog about;
    about.exec();
}


void NixNote::toggleLeftPanel() {
    bool visible;
    if (leftPanel->isVisible()) {
        leftScroll->hide();
        visible = false;
    } else {
        visible = true;
        leftScroll->show();
    }
    global.settings->beginGroup("SaveState");
    global.settings->setValue("leftPanelVisible", visible);
    global.settings->endGroup();
}


void NixNote::toggleNoteList() {
    bool value;
    if (topRightWidget->isVisible()) {
        topRightWidget->hide();
        value = false;
    } else {
        value = true;
        topRightWidget->show();
    }
    global.settings->beginGroup("SaveState");
    global.settings->setValue("noteListVisible", value);
    global.settings->endGroup();
}


void NixNote::toggleTabWindow() {
    bool value;
    if (tabWindow->isVisible()) {
        tabWindow->hide();
        value = false;
    } else {
        tabWindow->show();
        value = true;
    }
    global.settings->beginGroup("SaveState");
    global.settings->setValue("tabWindowVisible", value);
    global.settings->endGroup();

}


void NixNote::toggleToolbar() {
    if (toolBar->isVisible())
        toolBar->hide();
    else
        toolBar->show();
}



void NixNote::toggleStatusbar() {
    if (statusBar()->isVisible())
        statusBar()->hide();
    else
        statusBar()->show();
    global.settings->beginGroup("saveState");
    global.settings->setValue("statusBar", statusBar()->isVisible());
    global.settings->endGroup();
}


void NixNote::viewNoteHistory() {
    this->saveContents();
    statusBar()->clearMessage();
    if (!global.accountsManager->oauthTokenFound()) {
        QString consumerKey = "baumgarr-3523";
        QString consumerSecret = "8d5ee175f8a5d3ec";
        EvernoteOAuthDialog d(consumerKey, consumerSecret, global.server);
        d.setWindowTitle(tr("Log in to Evernote"));
        if(d.exec() != QDialog::Accepted) {
            QMessageBox::critical(0, tr("NixNote"), "Login failed.\n" + d.oauthError());
            return;
        }
        QString token = QString("oauth_token=") +d.oauthResult().authenticationToken +
                       QString("&oauth_token_secret=&edam_shard=")+d.oauthResult().shardId  +
                       QString("&edam_userId=") +QString::number(d.oauthResult().userId) +
                       QString("&edam_expires=") +QString::number(d.oauthResult().expires) +
                       QString("&edam_noteStoreUrl=") + d.oauthResult().noteStoreUrl +
                       QString("&edam_webApiUrlPrefix=") +d.oauthResult().webApiUrlPrefix;

        global.accountsManager->setOAuthToken(token);
    }

    UserTable userTable(global.db);
    User user;
    userTable.getUser(user);
    if (user.privilege == PrivilegeLevel::NORMAL) {
        QMessageBox mbox;
        mbox.setText(tr("This feature is only available to premium users."));
        mbox.setWindowTitle(tr("Premium Feature"));
        mbox.exec();
        return;
    }
    NoteHistorySelect dialog;
    CommunicationManager comm(global.db);
    if (comm.connect()) {
        QList<NoteVersionId> versions;
        NoteTable ntable(global.db);
        QString guid = ntable.getGuid(tabWindow->currentBrowser()->lid);
        comm.listNoteVersions(versions, guid);
        if (versions.size() > 0) {
            dialog.loadData(versions);
            dialog.exec();
            if (!dialog.importPressed)
                return;
            Note note;
            if (!comm.getNoteVersion(note, guid, dialog.usn)) {
                QMessageBox mbox;
                mbox.setText(tr("Error retrieving note."));
                mbox.setWindowTitle(tr("Error retrieving note"));
                mbox.exec();
                return;
            }
            note.updateSequenceNum = 0;
            note.active = true;
            QUuid uuid;
            QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
            note.guid = newGuid;
            QList<Resource> resources;
            if (note.resources.isSet())
                resources = note.resources;
            for (int i=0;i<resources.size(); i++) {
                Resource r = resources[i];
                r.updateSequenceNum = 0;
                newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
                r.guid = newGuid;
                resources[i] = r;
            }
            note.resources = resources;
            ntable.add(0,note,true);
            updateSelectionCriteria();
            setMessage(tr("Note restored"));

        } else {
            QMessageBox mbox;
            mbox.setText(tr("No versions of this note can be found."));
            mbox.setWindowTitle(tr("Note Not Found"));
            mbox.exec();
            return;
        }
    }
}


void NixNote::findInNote() {
    findReplaceWindow->showFind();
}

void NixNote::findNextInNote() {
    findReplaceWindow->showFind();
    QString find = findReplaceWindow->findLine->text();
    if (find != "")
        tabWindow->currentBrowser()->editor->page()->findText(find,
            findReplaceWindow->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
}

void NixNote::findPrevInNote() {
    findReplaceWindow->showFind();
    QString find = findReplaceWindow->findLine->text();
    if (find != "")
        tabWindow->currentBrowser()->editor->page()->findText(find,
            findReplaceWindow->getCaseSensitive() | QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);

}

void NixNote::findReplaceWindowHidden() {
    for (int i=0; i<tabWindow->browserList->size(); i++) {
        NBrowserWindow *b;
        b = tabWindow->browserList->at(i);
        b->editor->page()->findText("");
    }
}

void NixNote::findReplaceInNote() {
    findReplaceWindow->showFindReplace();
}


void NixNote::findReplaceInNotePressed() {
    QString find = findReplaceWindow->findLine->text();
    QString replace = findReplaceWindow->replaceLine->text();
    if (find == "")
        return;
    bool found = false;
    found = tabWindow->currentBrowser()->editor->page()->findText(find,
        findReplaceWindow->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
    if (!found)
        return;

    QClipboard *clip = global.clipboard;
    clip->setText(replace);
    tabWindow->currentBrowser()->editor->pasteAction->trigger();
}


void NixNote::findReplaceAllInNotePressed() {
    QString find = findReplaceWindow->findLine->text();
    QString replace = findReplaceWindow->replaceLine->text();
    if (find == "")
        return;
    bool found = false;
    while (true) {
        found = tabWindow->currentBrowser()->editor->page()->findText(find,
            findReplaceWindow->getCaseSensitive() | QWebPage::FindWrapsAroundDocument);
        if (!found)
            return;
        QClipboard *clip = global.clipboard;
        clip->setText(replace);
        tabWindow->currentBrowser()->editor->pasteAction->trigger();
    }
}



void NixNote::heartbeatTimerTriggered() {
    char *buffer = (char*)malloc(global.sharedMemory->size()); //Why not new?
    global.sharedMemory->lock();
    memcpy(buffer, global.sharedMemory->data(), global.sharedMemory->size());
    memset(global.sharedMemory->data(), 0, global.sharedMemory->size());
    global.sharedMemory->unlock();

    QByteArray data = QByteArray::fromRawData(buffer, global.sharedMemory->size());
    //QLOG_ERROR() << "Shared memory data: " << data;
    if (data.startsWith("IMMEDIATE_SHUTDOWN")) {
        QLOG_ERROR() << "Immediate shutdown requested by shared memory segment.";
        this->closeNixNote();
        return;
    }
    if (data.startsWith("SHOW_WINDOW")) {
        this->raise();
        this->show();
        return;
    }
    if (data.startsWith("QUERY:")) {
        QList<qint32> results;
        QString query = data.mid(6);
        QLOG_DEBUG() << query;
        FilterCriteria filter;
        filter.setSearchString(query);
        FilterEngine engine;
        engine.filter(&filter, &results);
        QString response = "RESPONSE:";
        QString xmlString;
        QXmlStreamWriter dom(&xmlString);
        dom.setAutoFormatting(true);
        dom.writeStartDocument();
        dom.writeStartElement("response");
        NoteTable ntable(global.db);
        for (int i=0; i<results.size(); i++) {
            dom.writeStartElement("note");
            dom.writeStartElement("lid");
            dom.writeCharacters(QString::number(results[i]));
            dom.writeEndElement();
            Note n;
            ntable.get(n, results[i], false, false);
            if (n.title.isSet()) {
                dom.writeStartElement("title");
                dom.writeCharacters(n.title);
                dom.writeEndElement();
            }
            QString filename = global.fileManager.getThumbnailDirPath("")+QString::number(results[i])+".png";
            QFile file(filename);
            if (file.exists()) {
                dom.writeStartElement("preview");
                dom.writeCharacters(filename);
                dom.writeEndElement();
            }
            dom.writeEndElement();
        }
        dom.writeEndElement();
        dom.writeEndDocument();

        global.sharedMemory->lock();
        void *memptr = global.sharedMemory->data();
        response.append(xmlString);
        memcpy(memptr, response.toStdString().c_str(), response.size());
        global.sharedMemory->unlock();

    }
    if (data.startsWith("OPEN_NOTE:")) {
        QLOG_DEBUG() << data;
        QString number = data.mid(10);
        QLOG_DEBUG() << "opennote " << number;
        qint32 note = number.toInt();
        NoteTable noteTable(global.db);
        if (noteTable.exists(note))
            this->openExternalNote(note);
    }
    if (data.startsWith("NEW_NOTE")) {
        this->newExternalNote();
    }

    free(buffer); // Fixes memory leak
}


// Open the dialog status dialog box.
void NixNote::openDatabaseStatus() {
    DatabaseStatus dbstatus;
    dbstatus.exec();
}




// Open the dialog status dialog box.
void NixNote::openImportFolders() {
    WatchFolderDialog dialog;
    dialog.exec();
    if (dialog.okClicked) {
        importManager->reset();
        importManager->setup();
    }
}




// Print the current note
void NixNote::printNote() {
    tabWindow->currentBrowser()->fastPrint = false;
    tabWindow->currentBrowser()->printNote();
}



// Print the current note
void NixNote::fastPrintNote() {
    tabWindow->currentBrowser()->fastPrint = true;
    tabWindow->currentBrowser()->printNote();
}




void NixNote::toggleVisible() {
    if (isMinimized() && minimizeToTray) {
        setHidden(false);
        this->show();
        this->setFocus();
        unhidingWindow=true;
        return;
    }

    if (isVisible())
        this->hide();
    else
        this->show();
}

// The tray icon was activated.  If it was double clicked we restore the
// gui.
void NixNote::trayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        toggleVisible();
    }
}


//void NixNote::trayIconBehavior()  {
//    //closeToTray = closeToTrayAction->isChecked();
//    //minimizeToTray = minimizeToTrayAction->isChecked();

//    global.settings->beginGroup("SaveState");
//    global.settings->setValue("closeToTray", closeToTray);
//    global.settings->setValue("minimizeToTray",  minimizeToTray);
//    global.settings->endGroup();

//}


void NixNote::changeEvent(QEvent *e) {
    if (e->type() == QEvent::WindowStateChange && e->type()) {
        if (isMinimized() && minimizeToTray && !unhidingWindow) {
            e->accept();
            unhidingWindow = false;
            QTimer::singleShot(10, this, SLOT(hide()));
            return;
        }
    }
}



void NixNote::openPreferences() {
    PreferencesDialog prefs;
    prefs.exec();
    if (prefs.okButtonPressed) {
        setSyncTimer();
        bool showTrayIcon = global.showTrayIcon();
        setWindowIcon(QIcon(global.getWindowIcon()));
        trayIcon->setIcon(QIcon(":trayicon.png"));
        if (!showTrayIcon) {
            //trayIconBehavior();
            if (!this->isVisible())
                this->show();
            trayIcon->setVisible(false);

        } else {
            trayIcon->setVisible(true);
            minimizeToTray = global.minimizeToTray();
            closeToTray = global.closeToTray();
            //trayIconBehavior();
        }
        indexRunner.officeFound = global.synchronizeAttachments();
    }
    setDebugLevel();
}


void NixNote::setDebugLevel() {
    global.settings->beginGroup("Debugging");
    int level = global.settings->value("messageLevel", -1).toInt();
    global.settings->endGroup();

    // Setup the QLOG functions for debugging & messages
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    if (level == QsLogging::TraceLevel)
        logger.setLoggingLevel(QsLogging::TraceLevel);
    else if (level == QsLogging::DebugLevel)
        logger.setLoggingLevel(QsLogging::DebugLevel);
    else if (level == QsLogging::InfoLevel || level == -1)
        logger.setLoggingLevel(QsLogging::InfoLevel);
    else if (level == QsLogging::WarnLevel)
        logger.setLoggingLevel(QsLogging::WarnLevel);
    else if (level == QsLogging::ErrorLevel)
        logger.setLoggingLevel(QsLogging::ErrorLevel);
    else if (level == QsLogging::FatalLevel)
        logger.setLoggingLevel(QsLogging::FatalLevel);
    else {
        logger.setLoggingLevel(QsLogging::InfoLevel);
        QLOG_WARN() << "Invalid message logging level " << level;
    }
}


void NixNote::setSyncTimer() {
    global.settings->beginGroup("Sync");
    bool automaticSync = global.settings->value("syncAutomatically", false).toBool();
    int interval = global.settings->value("syncInterval", 15).toInt();
    if (interval < 15)
        interval = 15;
    global.settings->endGroup();
    syncTimer.blockSignals(true);
    syncTimer.stop();
    syncTimer.blockSignals(false);
    if (!automaticSync) {
        return;
    }
    syncTimer.setInterval(60*1000*interval);
    syncTimer.blockSignals(true);
    syncTimer.start();
    syncTimer.blockSignals(false);
}


void NixNote::switchUser() {
    QAction *userSwitch;
    QList<int> checkedEntries;
    int currentAcctPos = 0;
    int newAcctPos = 0;
    for (int i=0; i<menuBar->userAccountActions.size(); i++) {
        userSwitch = menuBar->userAccountActions[i];
        int actionID = userSwitch->data().toInt();
        if (actionID == global.accountsManager->currentId)
            currentAcctPos = i;
        else
            if (userSwitch->isChecked())
                newAcctPos = i;
        if (userSwitch->isChecked()) {
            checkedEntries.append(i);
        }
    }

    // If nothing is checked, we recheck the old one or
    // if more than one is checked, we uncheck the old guy
    if(checkedEntries.size() == 0) {
        menuBar->blockSignals(true);
        menuBar->userAccountActions[currentAcctPos]->setChecked(true);
        menuBar->blockSignals(false);
        return;
    }
    if(checkedEntries.size() > 1) {
        menuBar->blockSignals(true);
        menuBar->userAccountActions[currentAcctPos]->setChecked(false);
        menuBar->blockSignals(false);
        global.accountsManager->currentId = menuBar->userAccountActions[newAcctPos]->data().toInt();
        global.globalSettings->beginGroup("SaveState");
        global.globalSettings->setValue("lastAccessedAccount", global.accountsManager->currentId);
        global.globalSettings->endGroup();
        closeAction->trigger();
        global.sharedMemory->detach();
        QProcess::startDetached(QCoreApplication::applicationFilePath());
        return;
    }
}


void NixNote::addAnotherUser() {
    AddUserAccountDialog dialog;
    dialog.exec();
    if (!dialog.okPushed)
        return;
    QString name = dialog.newAccountName->text().trimmed();
    int six = dialog.newAccountServer->currentIndex();
    QString server = dialog.newAccountServer->itemData(six, Qt::UserRole).toString();
    int newid = global.accountsManager->addId(-1, name, "", server);
    QAction *newAction = new QAction(menuBar);
    newAction->setText(tr("Switch to ") +name);
    newAction->setCheckable(true);
    newAction->setData(newid);
    menuBar->addUserAccount(newAction);
    connect(newAction, SIGNAL(triggered()), this, SLOT(switchUser()));
}



void NixNote::userMaintenance() {
    AccountMaintenanceDialog dialog(menuBar, this);
    dialog.exec();
}



void NixNote::viewNoteListWide() {
    menuBar->blockSignals(true);
    menuBar->viewNoteListNarrow->setChecked(false);
    menuBar->viewNoteListWide->setChecked(true);
    menuBar->blockSignals(false);

    saveNoteColumnPositions();
    saveNoteColumnWidths();
    noteTableView->saveColumnsVisible();

    rightPanelSplitter->setOrientation(Qt::Vertical);
    global.listView = Global::ListViewWide;
    noteTableView->setColumnsVisible();
    noteTableView->repositionColumns();
    noteTableView->resizeColumns();
}


void NixNote::viewNoteListNarrow() {

    menuBar->blockSignals(true);
    menuBar->viewNoteListWide->setChecked(false);
    menuBar->viewNoteListNarrow->setChecked(true);
    menuBar->blockSignals(false);

    saveNoteColumnPositions();
    saveNoteColumnWidths();
    noteTableView->saveColumnsVisible();

    rightPanelSplitter->setOrientation(Qt::Horizontal);
    global.listView = Global::listViewNarrow;
    noteTableView->setColumnsVisible();
    noteTableView->repositionColumns();
    noteTableView->resizeColumns();
}


// This is called via global.resourceWatcher when a resource
// has been updated by an external program.  The file name is the
// resource file which starts with the lid.
void NixNote::resourceExternallyUpdated(QString resourceFile) {
    QString shortName = resourceFile;
    QString dba = global.fileManager.getDbaDirPath();
    shortName.replace(dba, "");
    int pos = shortName.indexOf(".");
    if (pos > 0)
        shortName = shortName.mid(0,pos);
    qint32 lid = shortName.toInt();
    QFile file(resourceFile);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    file.close();
    QByteArray newHash = QCryptographicHash::hash(ba, QCryptographicHash::Md5);
    ResourceTable resTable(global.db);
    QByteArray oldHash = resTable.getDataHash(lid);
    if (oldHash != newHash) {
        qint32 noteLid = resTable.getNoteLid(lid);
        resTable.updateResourceHash(lid, newHash);
        NoteTable noteTable(global.db);
        noteTable.updateEnmediaHash(noteLid, oldHash, newHash, true);
        tabWindow->updateResourceHash(noteLid, oldHash, newHash);
    }
}



void NixNote::screenCapture() {
    if (noteButton->property("currentNoteButton").toInt() != NewScreenNote) {
        noteButton->setText(screenCaptureButton->text());
        noteButton->setIcon(screenCaptureButton->icon());
        noteButton->setProperty("currentNoteButton", NewScreenNote);
    }
    sleep(1);
    ScreenCapture sc;
    sc.exec();
    QPixmap pix = sc.getSelection();

    ConfigStore cs(global.db);
    qint32 lid = cs.incrementLidCounter();

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");

    QByteArray hash = md5hash.hash(data, QCryptographicHash::Md5);

    // * Start setting up the new note
    Note newNote;
    newNote.guid = QString::number(lid);
    newNote.title = tr("Screen Capture");

    NotebookTable bookTable(global.db);
    QString notebook;
    notebook = bookTable.getDefaultNotebookGuid();
    newNote.notebookGuid = notebook;

    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\">");

    QString mime = "image/png";
    QString enMedia =QString("<en-media hash=\"") +hash.toHex() +QString("\" border=\"0\"")
            +QString(" type=\"" +mime +"\" ")
            +QString("/>");
    newNoteBody.append(enMedia + QString("</en-note>"));
    newNote.content = newNoteBody;
    newNote.active = true;
    newNote.created = QDateTime::currentMSecsSinceEpoch();;
    newNote.updated = newNote.created;
    newNote.updateSequenceNum = 0;

    NoteTable ntable(global.db);
    ntable.add(lid, newNote, true);
    QString noteGuid = ntable.getGuid(lid);
    qint32 noteLid  = lid;
    lid = cs.incrementLidCounter();


    // Start creating the new resource
    Resource newRes;
    Data d;
    d.body = data;
    d.bodyHash = hash;
    d.size = data.size();
    newRes.mime = mime;
    ResourceAttributes attributes;
    attributes.attachment = false;
    newRes.active = true;
    newRes.guid = QString::number(lid);
    newRes.noteGuid = noteGuid;
    newRes.updateSequenceNum = 0;
    newRes.data = d;
    newRes.attributes = attributes;
    ResourceTable restable(global.db);
    restable.add(lid, newRes, true, noteLid);

    updateSelectionCriteria();
}



// Reindex all notes & resources
void NixNote::reindexDatabase() {

    int response = QMessageBox::question ( this, tr("Reindex Database"), tr("Reindex the entire database?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (response != QMessageBox::Yes)
        return;

    NoteTable ntable(global.db);
    ResourceTable rtable(global.db);
    rtable.reindexAllResources();
    ntable.reindexAllNotes();

    setMessage(tr("Notes will be reindexed."));
}



// Open/Close selected notebooks
void NixNote::openCloseNotebooks() {
    CloseNotebookDialog dialog;
    dialog.exec();
    if (dialog.okPressed) {
        notebookTreeView->rebuildNotebookTreeNeeded = true;
        this->updateSelectionCriteria();
        notebookTreeView->rebuildTree();
    }
}



// Capture an image from the webcam and create a new note
void NixNote::newWebcamNote() {
    if (noteButton->property("currentNoteButton") != NewWebcamNote) {
        noteButton->setText(newWebcamNoteButton->text());
        noteButton->setIcon(newWebcamNoteButton->icon());
        noteButton->setProperty("currentNoteButton", NewWebcamNote);
    }

    WebcamCaptureDialog dialog;
    QMessageBox msgBox;
    msgBox.setText(tr("Unable to find webcam or capture image."));
    msgBox.setWindowTitle(tr("Webcam Error"));

    // Check for error reading camera
    if (!dialog.webcamReady) {
        msgBox.exec();
        return;
    }

    dialog.exec();

    if (!dialog.okPressed) {
        return;
    }

    QImage img;
    // Check for webcam error
    if (!dialog.webcamReady || !dialog.webcamImage->getImage(img)) {
        msgBox.exec();
        return;
    }

    //Start generating the new note
    Note newNote;
    NoteTable ntable(global.db);

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");

    ConfigStore cs(global.db);
    qint32 lid = cs.incrementLidCounter();

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(data, QCryptographicHash::Md5);

    // * Start setting up the new note
    newNote.guid = QString::number(lid);
    newNote.title = "Webcam Note";

    NotebookTable notebookTable(global.db);
    if (notebookTreeView->selectedItems().size() == 0) {
        newNote.notebookGuid = notebookTable.getDefaultNotebookGuid();
    } else {
        NNotebookViewItem *item = (NNotebookViewItem*)notebookTreeView->selectedItems().at(0);
        QString notebookGuid;
        notebookTable.getGuid(notebookGuid, item->lid);
        newNote.notebookGuid = notebookGuid;
    }

    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\">");


    QString enMedia =QString("<en-media hash=\"") +hash.toHex() +QString("\" border=\"0\"")
            +QString(" type=\"image/png\" ")
            +QString("/>");
    newNoteBody.append(enMedia + QString("</en-note>"));
    newNote.content = newNoteBody;
    newNote.active = true;
    newNote.created = QDateTime::currentMSecsSinceEpoch();;
    newNote.updated = newNote.created;
    newNote.updateSequenceNum = 0;


    qint32 noteLid = lid;
    ntable.add(lid, newNote, true);
    QString noteGuid = ntable.getGuid(lid);
    lid = cs.incrementLidCounter();


    // Start creating the new resource
    Resource newRes;
    Data d;
    ResourceAttributes attributes;
    d.body = data;
    d.bodyHash = hash;
    d.size = data.size();
    newRes.mime = "image/png";
    attributes.fileName = "";
    attributes.attachment = false;
    newRes.active = true;
    newRes.guid = QString::number(lid);
    newRes.noteGuid = noteGuid;
    newRes.updateSequenceNum = 0;
    newRes.data = d;
    newRes.attributes = attributes;
    ResourceTable restable(global.db);
    restable.add(lid, newRes, true, noteLid);


    FilterCriteria *criteria = new FilterCriteria();
    global.filterCriteria[global.filterPosition]->duplicate(*criteria);
    criteria->setLid(noteLid);
    global.filterCriteria.append(criteria);
    global.filterPosition++;
    openNote(false);
    updateSelectionCriteria();
    tabWindow->currentBrowser()->editor->setFocus();
    return;
}


// Delete the note we are currently viewing
void NixNote::deleteCurrentNote() {
    qint32 lid = tabWindow->currentBrowser()->lid;

    QString typeDelete;
    QString msg;
    FilterCriteria *f  = global.filterCriteria[global.filterPosition];
    bool expunged = false;
    typeDelete = tr("Delete ");

    if (f->isDeletedOnlySet() && f->getDeletedOnly()) {
        typeDelete = tr("Permanently delete ");
        expunged = true;
    }

    msg = typeDelete + tr("this note?");

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Verify Delete"));
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int rc = msgBox.exec();
    if (rc != QMessageBox::Yes)
        return;

    NoteTable ntable(global.db);
    NSqlQuery sql(*global.db);
    sql.prepare("Delete from filter where lid=:lid");
    ntable.deleteNote(lid, true);
    if (expunged)
        ntable.expunge(lid);
    sql.bindValue(":lid", lid);
    sql.exec();
    sql.finish();
    delete global.cache[lid];
    global.cache.remove(lid);
    QList<qint32> lids;
    lids.append(lid);
    emit(notesDeleted(lids));
}



// Duplicate the current note
void NixNote::duplicateCurrentNote() {
    tabWindow->currentBrowser()->saveNoteContent();
    qint32 oldLid = tabWindow->currentBrowser()->lid;
    qint32 newLid;
    NoteTable ntable(global.db);
    newLid = ntable.duplicateNote(oldLid);

    FilterCriteria *criteria = new FilterCriteria();
    global.filterCriteria[global.filterPosition]->duplicate(*criteria);
    criteria->setLid(newLid);
    global.filterCriteria.append(criteria);
    global.filterPosition++;
    openNote(false);
    updateSelectionCriteria();
    tabWindow->currentBrowser()->editor->setFocus();
}


void NixNote::pinCurrentNote() {
    qint32 lid = tabWindow->currentBrowser()->lid;
    NoteTable ntable(global.db);
    ntable.pinNote(lid, true);
    updateSelectionCriteria();
}


void NixNote::unpinCurrentNote() {
    qint32 lid = tabWindow->currentBrowser()->lid;
    NoteTable ntable(global.db);
    ntable.pinNote(lid, false);
    updateSelectionCriteria();
}



void NixNote::spellCheckCurrentNote() {
    tabWindow->currentBrowser()->spellCheckPressed();
}


void NixNote::pauseIndexing(bool value) {
    if (menuBar->pauseIndexingAction->isChecked()) {
       indexRunner.pauseIndexing = true;
       return;
    }
    indexRunner.pauseIndexing = value;
}


void NixNote::openMessageLog() {
    LogViewer viewer;
    viewer.exec();
}


void NixNote::noteButtonClicked() {
    if (noteButton->property("currentNoteButton").toInt() == NewTextNote)
        newNote();
    if (noteButton->property("currentNoteButton").toInt() == NewWebcamNote)
        newWebcamNote();
    if (noteButton->property("currentNoteButton").toInt() == NewScreenNote)
        screenCapture();
}


void NixNote::showDesktopUrl(const QUrl &url) {
    QLOG_DEBUG() << url.toString();
}



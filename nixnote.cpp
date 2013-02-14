#include "nixnote.h"
#include "threads/syncrunner.h"
#include "gui/nwebview.h"

#include <QThread>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QDesktopServices>
#include <QClipboard>
#include <QStatusBar>

#include "sql/notetable.h"
#include "gui/ntabwidget.h"
#include "sql/notebooktable.h"
#include "settings/startupconfig.h"
#include "dialog/logindialog.h"
#include "gui/lineedit.h"
#include "gui/findreplace.h"
#include "gui/nattributetree.h"
#include "gui/ntrashtree.h"
#include "filters/filterengine.h"
#include "global.h"
#include "html/enmlformatter.h"
#include "oauth/oauthwindow.h"
#include "oauth/oauthtokenizer.h"
#include "dialog/databasestatus.h"

#include "gui/nmainmenubar.h"
#include "dialog/logindialog.h"
#include "xml/importdata.h"

extern Global global;
class SyncRunner;

//*************************************************
//* This is the main class that is used to start
//* everything else.
//*************************************************
NixNote::NixNote(QWidget *parent) : QMainWindow(parent)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    heartbeatTimer.setInterval(1000);
    heartbeatTimer.setSingleShot(false);
    connect(&heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeatTimerTriggered()));
    heartbeatTimer.start();

    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

        db = new DatabaseConnection();  // Startup the database
        QLOG_TRACE() << "Setting up global settings";
        this->initializeGlobalSettings();

        // Setup the sync thread
        QLOG_TRACE() << "Setting up sync thread";
        connect(this,SIGNAL(syncRequested()),&syncRunner,SLOT(synchronize()));
        connect(&syncRunner, SIGNAL(setMessage(QString)), this, SLOT(setMessage(QString)));
        syncRunner.start(QThread::NormalPriority);

        indexRunner.start(QThread::LowestPriority);

        QLOG_TRACE() << "Setting up GUI";
        this->setupGui();
        global.filterPosition = 0;
        this->openNote(false);

        QLOG_TRACE() << "Connecting signals";
        connect(tagTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(notebookTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(searchTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(attributeTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(trashTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(searchText, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        QLOG_DEBUG() << "Exiting NixNote constructor";
}





// Destructor to call when all done
NixNote::~NixNote()
{
    syncRunner.quit();
    indexRunner.quit();
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
    statusBar();
    setWindowTitle(tr("NixNote 2"));
    setWindowIcon(QIcon(":windowIcon.png"));

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

    homeButton = toolBar->addAction(QIcon(":home.png"), tr("Home"));
    homeButton->setPriority(QAction::LowPriority);
    syncButton = toolBar->addAction(QIcon(":synchronize.png"), tr("Synchronize"));
    syncButton->setPriority(QAction::LowPriority);
    toolBar->addSeparator();
    trunkButton = toolBar->addAction(QIcon(":trunk.png"), tr("Trunk"));
    newNoteButton = toolBar->addAction(QIcon(":newNote.png"), tr("New Note"));
    usageButton = toolBar->addAction(QIcon(":usage.png"), tr("Usage"));
    connect(syncButton,SIGNAL(triggered()), this, SLOT(synchronize()));
    connect(homeButton, SIGNAL(triggered()), this, SLOT(resetView()));
    connect(trunkButton, SIGNAL(triggered()), this, SLOT(openTrunk()));
    connect(newNoteButton, SIGNAL(triggered()), this, SLOT(newNote()));
    connect(usageButton, SIGNAL(triggered()), this, SLOT(openAccount()));

    QLOG_TRACE() << "Adding main splitter";
    mainSplitter = new QSplitter(Qt::Horizontal);
    setCentralWidget(mainSplitter);

    rightPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanel = new WidgetPanel();

    this->setupNoteList();
    this->setupSynchronizedNotebookTree();
    this->setupTagTree();
    this->setupSearchTree();
    this->setupAttributeTree();
    this->setupTrashTree();
    this->setupTabWindow();
    leftPanel->vboxLayout->addStretch();

    QLOG_TRACE() << "Setting up left panel";
    leftPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    leftScroll = new QScrollArea();
    leftScroll->setWidgetResizable(true);
    leftScroll->setWidget(leftPanel);
    leftScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leftScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mainSplitter->addWidget(leftScroll);
    mainSplitter->addWidget(rightPanelSplitter);
    mainSplitter->setStretchFactor(0,1);
    mainSplitter->setStretchFactor(1,3);

    QLOG_TRACE() << "Resetting left side widgets";
    tagTreeView->resetSize();
    searchTreeView->resetSize();
    attributeTree->resetSize();
    trashTree->resetSize();

    // Restore the window state
    QLOG_TRACE() << "Restoring window state";
    global.settings->beginGroup("SaveState");
    restoreState(global.settings->value("WindowState").toByteArray());
    restoreGeometry(global.settings->value("WindowGeometry").toByteArray());
    if (global.settings->value("isMaximized", false).toBool())
        this->setWindowState(Qt::WindowMaximized);
    QString lidListString = global.settings->value("openTabs", "").toString().trimmed();
    QString lastViewedLid = global.settings->value("lastViewed", 0).toString().trimmed();
    global.settings->endGroup();

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

    // Setup timers
    QLOG_TRACE() << "Setting up timers";
    connect(&syncButtonTimer, SIGNAL(timeout()), this, SLOT(updateSyncButton()));
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(syncButtonReset()));

    QLOG_TRACE() << "Setting up more connections for tab windows & threads";
    // Setup so we refresh whenever the sync is done.
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(updateSelectionCriteria()));

    // connect so we refresh the note list whenever a note has changed
    connect(tabWindow, SIGNAL(noteUpdated(qint32)), noteTableView, SLOT(refreshData()));

    // connect so we refresh the tag tree when a new tag is added
    connect(tabWindow, SIGNAL(tagCreated(qint32)), tagTreeView, SLOT(addNewTag(qint32)));

    connect(tabWindow, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
    connect(tabWindow->tabBar, SIGNAL(currentChanged(int)), this, SLOT(checkReadOnlyNotebook()));

    // Finish by filtering & displaying the data
    //updateSelectionCriteria();

    // connect signal on a tag rename
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), this, SLOT(updateSelectionCriteria()));
    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), this, SLOT(updateSelectionCriteria()));
    this->updateSelectionCriteria();
    // Set default focuse to the editor window
    tabWindow->currentBrowser()->editor->setFocus();

}


//******************************************************************************
//* This initializes some environment options based upon the command line
//******************************************************************************
void NixNote::initializeGlobalSettings() {

}

//******************************************************************************
//* This function sets up the note list window.  This is what the users select
//* view a specific note
//******************************************************************************
void NixNote::setupNoteList() {
   QLOG_TRACE() << "Starting NixNote.setupNoteList()";

   // Setup a generic widget to hold the search & note table
   topRightWidget  = new QWidget(this);
   topRightLayout = new QVBoxLayout(this);
   topRightLayout->addWidget(searchText);
   topRightWidget->setLayout(topRightLayout);
   noteTableView = new NTableView();
  // rightPanelSplitter->addWidget(noteTableView);
   topRightLayout->addWidget(noteTableView);
   topRightLayout->setContentsMargins(QMargins(0,0,0,0));

   // Add the generic widget
   rightPanelSplitter->addWidget(topRightWidget);
   connect(&syncRunner, SIGNAL(syncComplete()), noteTableView, SLOT(refreshData()));

   noteTableView->contextMenu->insertAction(noteTableView->deleteNoteAction, newNoteButton);
   noteTableView->contextMenu->insertSeparator(noteTableView->deleteNoteAction);
   connect(noteTableView, SIGNAL(notesDeleted(QList<qint32>,bool)), this, SLOT(notesDeleted(QList<qint32>)));
   connect(noteTableView, SIGNAL(notesRestored(QList<qint32>)), this, SLOT(notesRestored(QList<qint32>)));

   QLOG_TRACE() << "Leaving NixNote.setupNoteList()";
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
    connect(&syncRunner, SIGNAL(syncComplete()),tagTreeView, SLOT(rebuildTree()));
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
    connect(&syncRunner, SIGNAL(tagUpdated(qint32, QString)),tagTreeView, SLOT(tagUpdated(qint32, QString)));
    connect(&syncRunner, SIGNAL(tagExpunged(qint32)), tagTreeView, SLOT(tagExpunged(qint32)));
    connect(&syncRunner, SIGNAL(syncComplete()),tagTreeView, SLOT(rebuildTree()));
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
}



//*****************************************************************************
//* This function sets up the user's synchronized notebook tree
//*****************************************************************************
void NixNote::setupSynchronizedNotebookTree() {
    QLOG_TRACE() << "Exiting NixNote.setupSynchronizedNotebookTree()";
    QLabel *lbl = new QLabel();
    lbl->setTextFormat(Qt::RichText);
    //lbl->setText("<hr>");
    //leftPanel->addWidget(lbl);

    notebookTreeView = new NNotebookView(leftPanel);
    leftPanel->addWidget(notebookTreeView);
    connect(&syncRunner, SIGNAL(notebookUpdated(qint32, QString)),notebookTreeView, SLOT(notebookUpdated(qint32, QString)));
    connect(&syncRunner, SIGNAL(syncComplete()),notebookTreeView, SLOT(rebuildTree()));
    connect(&syncRunner, SIGNAL(notebookExpunged(qint32)), notebookTreeView, SLOT(notebookExpunged(qint32)));
    QLOG_TRACE() << "Exiting NixNote.setupSynchronizedNotebookTree()";
}



//*****************************************************************************
//* This function sets up the tab window that is used by the browser
//*****************************************************************************
void NixNote::setupTabWindow() {
    QLOG_TRACE() << "Exiting NixNote.setupTabWindow()";
    tabWindow = new NTabWidget(&syncRunner, notebookTreeView, tagTreeView);
    findReplaceWindow = new FindReplace(this);
    QWidget *tabPanel = new QWidget(this);
    tabPanel->setLayout(new QVBoxLayout(this));
    tabPanel->layout()->addWidget(tabWindow);
    tabPanel->layout()->addWidget(findReplaceWindow);
    rightPanelSplitter->addWidget(tabPanel);

    NBrowserWindow *newBrowser = new NBrowserWindow();
    connect(&syncRunner, SIGNAL(syncComplete()), &newBrowser->notebookMenu, SLOT(reloadData()));
    connect(&syncRunner, SIGNAL(syncComplete()), &newBrowser->tagEditor, SLOT(reloadTags()));
    connect(&syncRunner, SIGNAL(noteUpdated(qint32)), newBrowser, SLOT(noteSyncUpdate(qint32)));
    tabWindow->addBrowser(newBrowser, "");
    rightPanelSplitter->setStretchFactor(1,10);

    connect(noteTableView, SIGNAL(openNote(bool)), this, SLOT(openNote(bool)));
    connect(menuBar->viewSourceAction, SIGNAL(triggered()), tabWindow, SLOT(toggleSource()));

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




//*****************************************************************************
//* Close the program
//*****************************************************************************
void NixNote::closeEvent(QCloseEvent *) {
    saveContents();

    syncRunner.quit();
    indexRunner.quit();

    ConfigStore config;
    config.saveSetting(CONFIG_STORE_WINDOW_STATE, saveState());
    config.saveSetting(CONFIG_STORE_WINDOW_GEOMETRY, saveGeometry());

    QString lidList;
    QList<NBrowserWindow*> *browsers = tabWindow->browserList;
    for (int i=0;i<browsers->size(); i++) {
        lidList = lidList + QString::number(browsers->at(i)->lid) + QString(" ");
    }

    int order = noteTableView->proxy->sortOrder();
    int col = noteTableView->proxy->sortColumn();

    global.settings->beginGroup("SaveState");
    global.settings->setValue("WindowState", saveState());
    global.settings->setValue("WindowGeometry", saveGeometry());
    global.settings->setValue("isMaximized", isMaximized());
    global.settings->setValue("openTabs", lidList);
    global.settings->setValue("lastViewed", tabWindow->currentBrowser()->lid);
    global.settings->setValue("sortOrder", order);
    global.settings->setValue("sortColumn", col);
    global.settings->endGroup();
}

void NixNote::closeNixNote() {
   this->close();
}




//******************************************************************************
//* User synchronize was requested
//******************************************************************************
void NixNote::synchronize() {

    QFile oauthFile(global.fileManager.getHomeDirPath("oauth.txt"));
    if (!oauthFile.exists()) {
        OAuthWindow window;
        window.exec();
        if (window.error) {
            setMessage(window.errorMessage);
            return;
        }
        if (window.response == "")
            return;

        oauthFile.open(QIODevice::WriteOnly);
        QByteArray b;
        b.append(window.response);
        oauthFile.write(b);
        oauthFile.close();
    }
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
}




void NixNote::syncButtonReset() {
    indexRunner.pauseIndexing = false;
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
            rotatedPix.fill(toolBar->palette().color(QPalette::Button));
            QSize size = pix.size();
            p.translate(size.width()/2, size.height()/2);
            angle = angle+1.0;
            p.rotate(angle);
            p.setBackgroundMode( Qt::OpaqueMode );
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
        tabWindow->openNote(lid, newWindow);
    } else {
        tabWindow->openNote(-1, false);
    }
    rightArrowButton->setEnabled(false);
    leftArrowButton->setEnabled(false);
    if (global.filterPosition+1 < global.filterCriteria.size())
        rightArrowButton->setEnabled(true);
    if (global.filterPosition > 0)
        leftArrowButton->setEnabled(true);
    checkReadOnlyNotebook();
}

//*****************************************************
//* Called when a user changes the selection criteria
//* (i.e. they select a notebook, tag, saved search...
//*****************************************************
void NixNote::updateSelectionCriteria() {
    QLOG_TRACE() << "starting NixNote.updateSelectionCriteria()";

    FilterEngine filterEngine;
    filterEngine.filter();

    QLOG_TRACE() << "Refreshing data";

    noteTableView->refreshData();

    tagTreeView->updateSelection();
    notebookTreeView->updateSelection();
    searchTreeView->updateSelection();
    attributeTree->updateSelection();
    trashTree->updateSelection();
    searchText->updateSelection();
    //noteTableView->refreshSelection();


    rightArrowButton->setEnabled(false);
    leftArrowButton->setEnabled(false);
    if (global.filterPosition+1 < global.filterCriteria.size())
        rightArrowButton->setEnabled(true);
    if (global.filterPosition > 0)
        leftArrowButton->setEnabled(true);

    //openNote(false);

    QList<qint32> selectedNotes;
    global.filterCriteria[global.filterPosition]->getSelectedNotes(selectedNotes);
    if (selectedNotes.size() == 0) {
        tabWindow->currentBrowser()->clear();
    }
    if (selectedNotes.size() > 0) {
        tabWindow->currentBrowser()->setContent(selectedNotes.at(0));
        openNote(false);
    }

    if (global.filterCriteria[global.filterPosition]->isDeletedOnlySet() && global.filterCriteria[global.filterPosition]->getDeletedOnly())
        newNoteButton->setEnabled(false);
    else
        newNoteButton->setEnabled(true);
}


void NixNote::checkReadOnlyNotebook() {
    qint32 lid = tabWindow->currentBrowser()->lid;
    Note n;
    NoteTable ntable;
    NotebookTable btable;
    ntable.get(n, lid, false,false);
    qint32 notebookLid = btable.getLid(n.notebookGuid);
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
//* Backup & Restore
//**************************************************

void NixNote::databaseRestore() {
    QLOG_TRACE() << "Entering databaseRestore()";

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



    QFileDialog fd;
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setConfirmOverwrite(true);
    fd.setWindowTitle(tr("Restore Database"));
    fd.setFilter(tr("NixNote Export (*.nnex);;All Files (*.*)"));
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

    setMessage(tr("Restoring database"));
    ImportData noteReader(true);
    noteReader.import(fileNames[0]);

    if (noteReader.lastError != 0) {
        setMessage(noteReader.getErrorMessage());
        QLOG_ERROR() <<  "Restore problem: " << noteReader.lastError;
        waitCursor(false);
        return;
    }

    // Finish by filtering & displaying the data
    updateSelectionCriteria();
 //   noteTableView->refreshData();
    tagTreeView->rebuildTagTreeNeeded = true;
    tagTreeView->loadData();
    notebookTreeView->rebuildNotebookTreeNeeded = true;
    notebookTreeView->loadData();
    searchTreeView->loadData();
    setMessage(tr("Database has been restored."));
    waitCursor(false);
}



//*********************************************************
//* Set wait cursor
//*********************************************************
void NixNote::waitCursor(bool value) {
    value=value; /* suppress warning of unused */
}


// Show a message in the status bar
void NixNote::setMessage(QString text) {
    statusBar()->showMessage(text);
}


//*******************************************************
//* Check for dirty notes and save the contents
//*******************************************************
void NixNote::saveContents() {
    for (int i=0; i<tabWindow->browserList->size(); i++) {

        // Check if the note is dirty
        if (tabWindow->browserList->at(i)->editor->isDirty) {
            tabWindow->browserList->at(i)->saveNoteContent();
        }

    }
}


void NixNote::resetView() {
    FilterCriteria *criteria = new FilterCriteria();
    global.filterCriteria[global.filterPosition]->duplicate(*criteria);
    criteria->resetAttribute = true;
    criteria->resetDeletedOnly =true;
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
    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\"><br/></en-note>");

    Note n;
    NotebookTable notebookTable;
    n.content = newNoteBody.toStdString();
    n.__isset.content = true;
    n.title = "Untitled note";
    QString uuid = QUuid::createUuid();
    uuid = uuid.mid(1);
    uuid.chop(1);
    n.guid = uuid.toStdString();
    n.__isset.guid = true;
    n.__isset.title = true;
    n.__isset.active = true;
    n.active = true;
    QDateTime now;
    n.created = QDateTime::currentMSecsSinceEpoch();
    n.updated = n.created;
    n.__isset.created = true;
    n.__isset.updated = true;
    n.updateSequenceNum = 0;
    n.__isset.updateSequenceNum = true;
    NoteTable table;
    n.notebookGuid = notebookTable.getDefaultNotebookGuid().toStdString();
    n.__isset.notebookGuid = true;
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



// Slot for when notes have been deleted from the notes list.
void NixNote::notesDeleted(QList<qint32> lids) {
    updateSelectionCriteria();
}

// Slot for when notes have been deleted from the notes list.
void NixNote::notesRestored(QList<qint32> lids) {
    updateSelectionCriteria();
}




// Open the trunk web site
void NixNote::openTrunk() {
    QDesktopServices::openUrl(QUrl("http://www.evernote.com/trunk"));
}

void NixNote::openAccount() {
    AccountDialog dialog;
    dialog.exec();
}


void NixNote::openAbout() {
    QMessageBox::about(this, tr(" NixNote 2 "),
                       tr("This is ALPHA software. "
                          "Use it at your own risk. "
                          "\n\nLicensed under the Gnu Public License v2."
                          ));
}


void NixNote::toggleLeftPanel() {
    if (leftPanel->isVisible())
        leftScroll->hide();
    else
        leftScroll->show();
}


void NixNote::toggleNoteList() {
    if (topRightWidget->isVisible())
        topRightWidget->hide();
    else
        topRightWidget->show();
}


void NixNote::toggleTabWindow() {
    if (tabWindow->isVisible())
        tabWindow->hide();
    else
        tabWindow->show();
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
    char *buffer = (char*)malloc(global.sharedMemory->size());
    global.sharedMemory->lock();
    memcpy(buffer, global.sharedMemory->data(), global.sharedMemory->size());
    memset(global.sharedMemory->data(), 0, global.sharedMemory->size());
    global.sharedMemory->unlock();

    QByteArray data = QByteArray::fromRawData(buffer, global.sharedMemory->size());
    if (data.startsWith("IMMEDIATE_SHUTDOWN")) {
        QLOG_ERROR() << "Immediate shutdown requested by shared memory segment.";
        this->close();
        return;
    }
    if (data.startsWith("SHOW_WINDOW")) {
        this->raise();
        this->show();
        return;
    }
}



void NixNote::openDatabaseStatus() {
    DatabaseStatus dbstatus;
    dbstatus.exec();
}

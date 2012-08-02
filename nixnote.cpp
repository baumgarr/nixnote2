#include "nixnote.h"
#include "threads/syncrunner.h"
#include "gui/nwebview.h"

#include <QThread>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>

#include "sql/notetable.h"
#include "gui/ntabwidget.h"
#include "settings/startupconfig.h"
#include "dialog/logindialog.h"
#include "gui/lineedit.h"
#include "gui/nattributetree.h"
#include "gui/ntrashtree.h"
#include "filters/filterengine.h"
#include "java/javamachine.h"
#include "global.h"
#include "html/enmlformatter.h"

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
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

        db = new DatabaseConnection();  // Startup the database
        QLOG_TRACE() << "Setting up global settings";
        this->initializeGlobalSettings();

        // Setup the sync thread
        QLOG_TRACE() << "Setting up sync thread";
        connect(this,SIGNAL(syncRequested()),&syncRunner,SLOT(synchronize()));
       // syncRunner.start(QThread::NormalPriority);

       // indexRunner.start(QThread::LowestPriority);

        QLOG_TRACE() << "Setting up GUI";
        this->setupGui();

        QLOG_TRACE() << "Connecting signals";
        connect(tagTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(notebookTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(searchTreeView, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(attributeTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(trashTree, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
        connect(searchText, SIGNAL(updateSelectionRequested()), this, SLOT(updateSelectionCriteria()));
}





// Destructor to call when all done
NixNote::~NixNote()
{
    syncRunner.quit();
    indexRunner.quit();
    delete db;  // Free up memory used by the database connection
}



//****************************************************************
//* Setup the user interface
//****************************************************************
void NixNote::setupGui() {
    // Setup the GUI
    //this->setStyleSheet("background-color: white;");
    setWindowTitle(tr("Nixnote 2"));
    setWindowIcon(QIcon(":notebook.png"));

    QLOG_TRACE() << "Setting up menu bar";
    menuBar = new NMainMenuBar(this);
    setMenuBar(menuBar);

    QLOG_TRACE() << "Setting up tool bar";
    toolBar = addToolBar(tr("ToolBar"));
    toolBar->setObjectName("toolBar");
    leftArrowButton = toolBar->addAction(QIcon(":left_arrow.png"), tr("Back"));
    rightArrowButton = toolBar->addAction(QIcon(":right_arrow.png"), tr("Next"));

    leftArrowButton->setEnabled(false);
    connect(leftArrowButton, SIGNAL(triggered(bool)),this, SLOT(leftButtonTriggered()));
    rightArrowButton->setEnabled(false);
    connect(rightArrowButton, SIGNAL(triggered(bool)),this, SLOT(rightButtonTriggered()));

    homeButton = toolBar->addAction(QIcon(":home.png"), tr("Home"));
    syncButton = toolBar->addAction(QIcon(":synchronize.png"), tr("Synchronize"));
    connect(syncButton,SIGNAL(triggered()), this, SLOT(synchronize()));

    QLOG_TRACE() << "Adding main splitter";
    mainSplitter = new QSplitter(Qt::Horizontal);
    setCentralWidget(mainSplitter);

    rightPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanelSplitter = new QSplitter(Qt::Vertical);
    leftPanel = new WidgetPanel();

    this->setupNoteList();
    searchText = new LineEdit();
    leftPanel->addWidget(searchText);
    this->setupSynchronizedNotebookTree();
    this->setupTagTree();
    this->setupSearchTree();
    this->setupAttributeTree();
    this->setupTrashTree();
    this->setupTabWindow();
    leftPanel->vboxLayout->addStretch();

    QLOG_TRACE() << "Setting up fleft panel";
    leftPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
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

    QLOG_TRACE() << "Restoring window state";
    // Restore the window state
    ConfigStore config;
    QByteArray value;
    if (config.getSetting(value, CONFIG_STORE_WINDOW_STATE))
        restoreState(value);
    if (config.getSetting(value, CONFIG_STORE_WINDOW_GEOMETRY))
        restoreGeometry(value);

    // Setup timers
    QLOG_TRACE() << "Setting up timers";
    connect(&syncButtonTimer, SIGNAL(timeout()), this, SLOT(updateSyncButton()));
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(syncButtonReset()));

    QLOG_TRACE() << "Setting up more connections for tab windows & threads";
    // Setup so we refresh whenever the sync is done.
    connect(&syncRunner, SIGNAL(syncComplete()), this, SLOT(updateSelectionCriteria()));

    // connect so we refresh the note list whenever a note has chaneged
    connect(tabWindow, SIGNAL(noteUpdated(int)), noteTableView, SLOT(refreshData()));

    // connect so we refresh the tag tree when a new tag is added
    connect(tabWindow, SIGNAL(tagCreated(int)), tagTreeView, SLOT(addNewTag(int)));

    // Finish by filtering & displaying the data
    updateSelectionCriteria();

    // connect signal on a tag rename
    connect(tagTreeView, SIGNAL(tagRenamed(int,QString,QString)), this, SLOT(updateSelectionCriteria()));
    connect(notebookTreeView, SIGNAL(notebookRenamed(int,QString,QString)), this, SLOT(updateSelectionCriteria()));
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
   noteTableView = new NTableView();
   rightPanelSplitter->addWidget(noteTableView);
   connect(&syncRunner, SIGNAL(syncComplete()), noteTableView, SLOT(refreshData()));
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
    connect(&syncRunner, SIGNAL(searchUpdated(int, QString)), searchTreeView, SLOT(searchUpdated(int, QString)));
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
    connect(&syncRunner, SIGNAL(tagUpdated(int, QString)),tagTreeView, SLOT(tagUpdated(int, QString)));
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
    lbl->setText("<hr>");
    leftPanel->addWidget(lbl);

    notebookTreeView = new NNotebookView(leftPanel);
    leftPanel->addWidget(notebookTreeView);
    connect(&syncRunner, SIGNAL(notebookUpdated(int, QString)),notebookTreeView, SLOT(notebookUpdated(int, QString)));
    connect(&syncRunner, SIGNAL(syncComplete()),notebookTreeView, SLOT(rebuildTree()));
    QLOG_TRACE() << "Exiting NixNote.setupSynchronizedNotebookTree()";
}



//*****************************************************************************
//* This function sets up the tab window that is used by the browser
//*****************************************************************************
void NixNote::setupTabWindow() {
    QLOG_TRACE() << "Exiting NixNote.setupTabWindow()";
    tabWindow = new NTabWidget();
    rightPanelSplitter->addWidget(tabWindow);
    NBrowserWindow *newBrowser = new NBrowserWindow();
    connect(tagTreeView, SIGNAL(tagRenamed(int,QString,QString)), newBrowser, SLOT(tagRenamed(int,QString,QString)));
    connect(tagTreeView, SIGNAL(tagDeleted(int, QString)), newBrowser, SLOT(tagDeleted(int, QString)));
    connect(tagTreeView, SIGNAL(tagAdded(int)), newBrowser, SLOT(addTagName(int)));

    connect(notebookTreeView, SIGNAL(notebookRenamed(int,QString,QString)), newBrowser, SLOT(notebookRenamed(int,QString,QString)));
    connect(notebookTreeView, SIGNAL(notebookDeleted(int, QString)), newBrowser, SLOT(notebookDeleted(int, QString)));
    connect(notebookTreeView, SIGNAL(notebookAdded(int)), newBrowser, SLOT(notebookAdded(int)));

    connect(notebookTreeView, SIGNAL(stackRenamed(QString,QString)), newBrowser, SLOT(stackRenamed(QString,QString)));
    connect(notebookTreeView, SIGNAL(stackDeleted(QString)), newBrowser, SLOT(stackDeleted(QString)));
    connect(notebookTreeView, SIGNAL(stackAdded(QString)), newBrowser, SLOT(stackAdded(QString)));
    tabWindow->addBrowser(newBrowser, "");
    rightPanelSplitter->setStretchFactor(1,10);

    connect(noteTableView, SIGNAL(openNote(bool)), this, SLOT(openNote(bool)));
    QLOG_TRACE() << "Exiting NixNote.setupTabWindow()";
}




//*****************************************************************************
//* Close the program
//*****************************************************************************
void NixNote::closeEvent(QCloseEvent *) {
    ConfigStore config;
    config.saveSetting(CONFIG_STORE_WINDOW_STATE, saveState());
    config.saveSetting(CONFIG_STORE_WINDOW_GEOMETRY, saveGeometry());
}

void NixNote::closeNixNote() {
   this->close();
}




//******************************************************************************
//* User synchronize was requested
//******************************************************************************
void NixNote::synchronize() {
    while (!global.connected) {
        LoginDialog login;
        login.exec();
        if (login.okPressed == true) {
            global.password = login.password.text().toStdString();
            global.username = login.userid.text().toStdString();
            if (syncRunner.enConnect()) {
                global.connected = true;
                indexRunner.pauseIndexing = true;
                menuBar->disconnectAction->setEnabled(true);
            }
        } else
            return;
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
        for (int i=0; i<=360; i++) {
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
    int lid;
    if (criteria->isContentSet()) {
        lid = criteria->getContent();
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
}

//*****************************************************
//* Called when a user changes the selection criteria
//* (i.e. they select a notebook, tag, saved search...
//*****************************************************
void NixNote::updateSelectionCriteria() {
    QLOG_TRACE() << "starting NixNote.updateSelectionCriteria()";
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

    openNote(false);

    FilterEngine filterEngine;
    filterEngine.filter();
    QLOG_TRACE() << "Refreshing data";
    noteTableView->refreshData();
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

}

void NixNote::setMessage(QString text) {
    QLOG_INFO() << text;

}


//*******************************************************
//* Check for dirty notes and save the contents
//*******************************************************
void NixNote::saveContents() {
    for (int i=0; i<tabWindow->browserList->size(); i++) {

        // Check if the note is dirty
        if (tabWindow->browserList->at(i)->editor.isDirty) {
            QString contents = tabWindow->browserList->at(i)->editor.editorPage->mainFrame()->toHtml();
            EnmlFormatter formatter;
            formatter.setHtml(contents);
            formatter.rebuildNoteEnml();
            int lid = tabWindow->browserList->at(i)->lid;
            NoteTable table;
            table.updateNoteContent(lid, formatter.getEnml());
        }

    }
}

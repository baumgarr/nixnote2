#include "nmainmenubar.h"
#include "global.h"
#include <QAbstractAnimation>
#include <QFileIconProvider>

extern Global global;

NMainMenuBar::NMainMenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    this->parent = (NixNote*)parent;
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();
    setupToolsMenu();
    setupHelpMenu();
}


void NMainMenuBar::setupFileMenu() {
    fileMenu = this->addMenu(tr("&File"));

    QFont f;
    f.setPointSize(8);
    restoreDatabaseAction = new QAction(tr("Restore Database"), this);
    restoreDatabaseAction->setToolTip(tr("Restore from a backup"));
    restoreDatabaseAction->setFont(f);
    connect(restoreDatabaseAction, SIGNAL(triggered()), parent, SLOT(databaseRestore()));
    setupShortcut(restoreDatabaseAction, QString("File_Restore_Database"));
    fileMenu->addAction(restoreDatabaseAction);


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

void NMainMenuBar::setupEditMenu() {
    editMenu = this->addMenu(tr("&Edit"));

    undoAction = new QAction(tr("Undo"), this);
    setupShortcut(undoAction, QString("Edit_Undo"));
    editMenu->addAction(undoAction);

    redoAction = new QAction(tr("Undo"), this);
    setupShortcut(redoAction, QString("Edit_Redo"));
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    cutAction = new QAction(tr("Cut"), this);
    setupShortcut(cutAction, QString("Edit_Cut"));
    editMenu->addAction(cutAction);

    copyAction = new QAction(tr("Copy"), this);
    setupShortcut(copyAction, QString("Edit_Copy"));
    editMenu->addAction(copyAction);

    pasteAction = new QAction(tr("Paste"), this);
    setupShortcut(pasteAction, QString("Edit_Paste"));
    editMenu->addAction(pasteAction);

    pasteAsTextAction = new QAction(tr("Paste as Text"), this);
    setupShortcut(pasteAsTextAction, QString("Edit_Paste_Without_Formatting"));
    editMenu->addAction(pasteAsTextAction);

    editMenu->addSeparator();

    selectAllAction = new QAction(tr("Select All"), this);
    setupShortcut(selectAllAction, QString("Edit_Select_All"));
    editMenu->addAction(selectAllAction);

}

void NMainMenuBar::setupViewMenu() {
    viewMenu = this->addMenu(tr("&View"));
    viewSourceAction = new QAction(tr("Show Source"), this);
    viewMenu->addAction(viewSourceAction);
}

void NMainMenuBar::setupToolsMenu() {
  toolsMenu = this->addMenu(tr("&Tools"));
  QFont f;
  f.setPointSize(8);

  synchronizeAction = new QAction(tr("Synchronize"), this);
  synchronizeAction->setToolTip(tr("Close the program"));
  connect(synchronizeAction, SIGNAL(triggered()), parent, SLOT(synchronize()));
  setupShortcut(synchronizeAction, QString("Tools_Synchronize"));
  synchronizeAction->setFont(f);
  toolsMenu->addAction(synchronizeAction);

  disconnectAction = new QAction(tr("Disconnect"), this);
  synchronizeAction->setToolTip(tr("Disconnect from Evernote"));
  connect(disconnectAction, SIGNAL(triggered()), parent, SLOT(disconnect()));
  setupShortcut(disconnectAction, QString("Tools_Synchronize"));
  disconnectAction->setFont(f);
  toolsMenu->addAction(disconnectAction);
  disconnectAction->setEnabled(false);

  accountDialogAction = new QAction(tr("Account"), this);
  accountDialogAction->setToolTip(tr("Account information"));
  connect(accountDialogAction, SIGNAL(triggered()), parent, SLOT(openAccount()));
  toolsMenu->addAction(accountDialogAction);

}

void NMainMenuBar::setupHelpMenu() {
    helpMenu = this->addMenu(tr("&Help"));
    aboutAction = new QAction(tr("About"), this);
    aboutAction->setToolTip(tr("About"));
    connect(aboutAction, SIGNAL(triggered()), parent, SLOT(openAbout()));
    helpMenu->addAction(aboutAction);
}

void NMainMenuBar::setupShortcut(QAction *action, QString text) {
    if (!global.shortcutKeys->containsAction(&text))
        return;
    QKeySequence key(global.shortcutKeys->getShortcut(&text));
    action->setShortcut(key);
}

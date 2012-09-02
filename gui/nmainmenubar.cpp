#include "nmainmenubar.h"
#include <QAbstractAnimation>
#include <QFileIconProvider>

NMainMenuBar::NMainMenuBar(NixNote *parent) :
    QMenuBar(parent)
{
    this->parent = parent;
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
}

void NMainMenuBar::setupViewMenu() {
    viewMenu = this->addMenu(tr("&View"));
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
}

void NMainMenuBar::setupHelpMenu() {
    helpMenu = this->addMenu(tr("&Help"));
}

void NMainMenuBar::setupShortcut(QAction *action, QString key) {
    /* suppress unused */
    action=action;
    key=key;
}

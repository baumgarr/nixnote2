#ifndef NMAINMENUBAR_H
#define NMAINMENUBAR_H

#include <QMenuBar>
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
    void setupToolsMenu();
    void setupHelpMenu();
    void setupShortcut(QAction *action, QString key);

public:
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QAction *exitAction;

    QAction *synchronizeAction;
    QAction *disconnectAction;
    QAction *restoreDatabaseAction;
    QAction *viewSourceAction;
    QAction *accountDialogAction;
    QAction *aboutAction;

    explicit NMainMenuBar(QWidget *parent = 0);

signals:

public slots:

};

#endif // NMAINMENUBAR_H

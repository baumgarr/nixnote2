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
    QFont font;


    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupToolsMenu();
    void setupHelpMenu();
    void setupShortcut(QAction *action, QString text);

public:
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QMenu *findReplaceMenu;

    QAction *exitAction;
    QAction *printAction;
    QAction *synchronizeAction;
    QAction *importFoldersDialogAction;
    QAction *disconnectAction;
    QAction *databaseStatusDialogAction;
    QAction *restoreDatabaseAction;
    QAction *accountDialogAction;
    QAction *aboutAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteAsTextAction;
    QAction *deleteNoteAction;
    QAction *selectAllAction;
    QAction *preferencesAction;

    QAction *searchNotesAction;
    QAction *searchFindAction;
    QAction *searchFindNextAction;
    QAction *searchFindPrevAction;
    QAction *searchFindReplaceAction;
    QAction *resetSearchAction;

    QAction *viewSourceAction;
    QAction *viewExtendedInformation;
    QAction *viewLeftPanel;
    QAction *viewNoteList;
    QAction *viewNotePanel;
    QAction *viewToolbar;
    QAction *viewStatusbar;

    explicit NMainMenuBar(QWidget *parent = 0);

signals:

public slots:

};

#endif // NMAINMENUBAR_H

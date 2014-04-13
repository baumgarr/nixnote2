#include "traymenu.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

TrayMenu::TrayMenu(QWidget *parent) :
    QMenu(parent)
{
    signalMapper = new QSignalMapper();
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(noteChosen(int)));

    connect(this, SIGNAL(aboutToShow()), this, SLOT(buildPinnedMenu()));
}

void TrayMenu::setPinnedMenu(QMenu *menu)  {
    pinnedMenu = menu;
}

void TrayMenu::buildPinnedMenu() {
    for (int i=pinnedActions.size()-1; i>=0; i--) {
        signalMapper->removeMappings(pinnedActions[i]);
        pinnedMenu->removeAction(pinnedActions[i]);
    }
    pinnedActions.clear();

    QList< QPair< qint32, QString> > pinned;
    NoteTable noteTable(global.db);
    noteTable.getAllPinned(pinned);

    for (int i=0; i<pinned.size(); i++) {
        QAction *newAction = pinnedMenu->addAction(pinned[i].second);
        signalMapper->setMapping(newAction, pinned[i].first);
        connect(newAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
        pinnedActions.append(newAction);
    }
}


void TrayMenu::noteChosen(int note) {
    emit (openNote(note));
}

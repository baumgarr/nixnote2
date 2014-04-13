#include "traymenu.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

TrayMenu::TrayMenu(QWidget *parent) :
    QMenu(parent)
{
    signalMapper = new QSignalMapper();
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(noteChosen(int)));

    connect(this, SIGNAL(aboutToShow()), this, SLOT(buildActionMenu()));
}

void TrayMenu::setActionMenu(ActionMenuType type, QMenu *menu)  {
    if (type == PinnedMenu)
        pinnedMenu = menu;
    if (type == RecentMenu)
        recentlyUpdatedMenu = menu;
}

void TrayMenu::buildActionMenu() {
    for (int i=actions.size()-1; i>=0; i--) {
        signalMapper->removeMappings(actions[i]);
        pinnedMenu->removeAction(actions[i]);
        recentlyUpdatedMenu->removeAction(actions[i]);
    }
    actions.clear();

    QList< QPair< qint32, QString> > records;
    NoteTable noteTable(global.db);
    noteTable.getAllPinned(records);
    buildMenu(pinnedMenu, records);
    records.clear();;
    noteTable.getRecentlyUpdated(records);
    buildMenu(recentlyUpdatedMenu, records);
}


void TrayMenu::buildMenu(QMenu *actionMenu, QList< QPair <qint32, QString> > records) {
    for (int i=0; i<records.size(); i++) {
        QAction *newAction = actionMenu->addAction(records[i].second);
        signalMapper->setMapping(newAction, records[i].first);
        connect(newAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
        actions.append(newAction);
    }

}

void TrayMenu::noteChosen(int note) {
    emit (openNote(note));
}

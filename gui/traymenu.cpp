/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#include "traymenu.h"
#include "sql/notetable.h"
#include "sql/favoritestable.h"
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
    if (type == FavoriteNotesMenu)
        favoriteNotesMenu = menu;
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

    records.clear();
    FavoritesTable ftable(global.db);
    QList<qint32> lids;
    ftable.getAll(lids);
    for (int i=0; i<lids.size(); i++) {
        FavoritesRecord record;
        ftable.get(record, lids[i]);
        if (record.type == FavoritesRecord::Note) {
            QPair<qint32, QString> pair;
            pair.first = record.target.toInt();
            pair.second = record.displayName;
            records.append(pair);
        }
    }
    favoriteNotesMenu->clear();
    buildMenu(favoriteNotesMenu, records);
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

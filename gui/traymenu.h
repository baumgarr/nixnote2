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

#ifndef TRAYMENU_H
#define TRAYMENU_H

#include <QMenu>
#include <QSignalMapper>

class TrayMenu : public QMenu
{
    Q_OBJECT
private:
    QList<QAction*> actions;
    QMenu *pinnedMenu;
    QMenu *recentlyUpdatedMenu;
    QSignalMapper *signalMapper;
    void buildMenu(QMenu *actionMenu, QList<QPair<qint32, QString> > records);

public:
    enum ActionMenuType {
        PinnedMenu,
        RecentMenu
    };

    explicit TrayMenu(QWidget *parent = 0);
    void setActionMenu(ActionMenuType type, QMenu *menu);

signals:
    void openNote(qint32);

public slots:
    void buildActionMenu();
    void noteChosen(int note);

};

#endif // TRAYMENU_H

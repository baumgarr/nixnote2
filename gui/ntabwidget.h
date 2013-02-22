/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#ifndef NTABWIDGET_H
#define NTABWIDGET_H

#include "gui/nbrowserwindow.h"
#include "gui/ntagview.h"
#include "gui/nnotebookview.h"
#include "threads/syncrunner.h"

#include <QTabBar>
#include <QStackedWidget>

class SyncRunner;

class NTabWidget : public QWidget
{
    Q_OBJECT
private:
    void setupConnections(NBrowserWindow *browserWindow);
    NTagView *tagTreeView;
    NNotebookView *notebookTreeView;
    SyncRunner *syncThread;

public:
    explicit NTabWidget(SyncRunner *s, NNotebookView *n, NTagView *t);
    ~NTabWidget();
    QTabBar *tabBar;
    QVBoxLayout vboxlayout;
    QList<NBrowserWindow *> *browserList;
    void addBrowser(NBrowserWindow *v, QString title);
    void setTitle(QString t);
    void setTitle(qint32 index, QString title);
    QStackedWidget stack;
    NBrowserWindow *currentBrowser();


signals:
    void updateSelectionRequested();
    void noteUpdated(qint32);
    void tagCreated(qint32);

public slots:
    void closeTab(int index);
    void moveTab(int to, int from);
    void openNote(qint32 lid, bool newWindow);
    void tagCreationSignaled(qint32 lid);
    void noteSyncSignaled(qint32 lid);
    void noteUpdateSignaled(qint32);
    void evernoteLinkClicked(qint32 openLid, bool newWindow);

    void undoButtonPressed();
    void redoButtonPressed();
    void cutButtonPressed();
    void copyButtonPressed();
    void pasteButtonPressed();
    void pasteAsTextButtonPressed();
    void selectAllButtonPressed();
    void viewExtendedInformation();
    void toggleSource();

};

#endif // NTABWIDGET_H

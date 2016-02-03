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
#include "gui/externalbrowse.h"
#include "dialog/htmlentitiesdialog.h"
#include "nixnote.h"

#include <QTabBar>
#include <QStackedWidget>

class SyncRunner;
class NBrowserWindow;
class NixNote;

class NTabWidget : public QWidget
{
    Q_OBJECT
private:
    void setupConnections(NBrowserWindow *browserWindow);
    void setupExternalBrowserConnections(NBrowserWindow *browserWindow);
    NTagView *tagTreeView;
    NNotebookView *notebookTreeView;
    SyncRunner *syncThread;
    HtmlEntitiesDialog *htmlEntities;
    NixNote *parent;

public:
    explicit NTabWidget(NixNote *p, SyncRunner *s, NNotebookView *n, NTagView *t);
    ~NTabWidget();
    QTabBar *tabBar;
    QVBoxLayout vboxlayout;
    QList<NBrowserWindow *> *browserList;
    QList<ExternalBrowse *> *externalList;
    void addBrowser(NBrowserWindow *v, QString title);
    void setTitle(QString t);
    void setTitle(qint32 index, QString title);
    QStackedWidget stack;
    NBrowserWindow *currentBrowser();
    enum OpenNoteMode {
        CurrentTab = 0,
        NewTab = 1,
        ExternalWindow = 2
    };
    void reloadIcons();
    void changeEditorStyle();
    bool findBrowser(NBrowserWindow *retval, qint32 lid);


signals:
    void updateSelectionRequested();
    void noteUpdated(qint32);
    void tagCreated(qint32);
    void updateNoteList(qint32, int, QVariant); // A note was edited so we update the note list
    void updateNoteTitle(QString uuid, qint32 lid, QString content);
    void escapeKeyPressed();

public slots:
    void closeTab(int index);
    void closeTab();
    void moveTab(int to, int from);
    void openNote(qint32 lid, OpenNoteMode mode);
    void tagCreationSignaled(qint32 lid);
    void noteSyncSignaled(qint32 lid);
    void noteUpdateSignaled(qint32);
    void evernoteLinkClicked(qint32 openLid, bool newTab, bool newWindow);
    void escapeKeyListener();
    void saveAllNotes();
    void undoButtonPressed();
    void redoButtonPressed();
    void cutButtonPressed();
    void copyButtonPressed();
    void pasteButtonPressed();
    void pasteAsTextButtonPressed();
    void selectAllButtonPressed();
    void viewExtendedInformation();
    void toggleSource();
    void updateResourceHash(qint32 noteLid, QByteArray oldHash, QByteArray newHash);  // Update the hash of a resource in a note
    void refreshNoteContent(qint32 lid);  // refresh a note's contents
    void updateNoteListSignaled(qint32, int, QVariant); // A note was edited so we update the note list
    void noteContentEdited(QString uuid, qint32 lid, QString content);
    void noteTitleEdited(QString uuid, qint32 lid, QString title);
    void noteAuthorEdited(QString uuid, qint32 lid, QString title);
    void noteNotebookEdited(QString uuid, qint32 lid, qint32 notebookLid, QString notebookName);
    void noteDateEdited(QString uuid, qint32 lid, int dateID, QDateTime dt);
    void noteUrlEdited(QString uuid, qint32 lid, QString title);
    void noteLocationEdited(QString uuid, qint32 lid, double longitude, double latitude, double altitude, QString name);
    void noteTagsEdited(QString uuid, qint32 lid, QStringList names);
    void noteAlarmEdited(QString uuid, qint32 lid, bool strikeout, QString text);
    void showHtmlEntities();
    void htmlEntitiesClicked(QString entity);

    void nextTab();
    void prevTab();
};

#endif // NTABWIDGET_H

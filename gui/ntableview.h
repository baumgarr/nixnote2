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

#ifndef NTABLEVIEW_H
#define NTABLEVIEW_H

#include <QTableView>
#include <QMenu>
#include "ntableviewheader.h"
#include "models/notemodel.h"
#include "datedelegate.h"
#include "numberdelegate.h"
#include "reminderorderdelegate.h"
#include "truefalsedelegate.h"
#include "filters/notesortfilterproxymodel.h"
#include "gui/imagedelegate.h"

class NTableViewHeader;

class NTableView : public QTableView
{
    Q_OBJECT
private:
    //DatabaseConnection *db;
    //unsigned int filterPosition;
    DateDelegate *dateDelegate;
    NumberDelegate *blankNumber;
    NumberDelegate *kbNumber;
    TrueFalseDelegate *trueFalseDelegate;
    ImageDelegate *thumbnailDelegate;
    ReminderOrderDelegate *reminderOrderDelegate;
    QModelIndex dragStartIndex;


public:
    explicit NTableView(QWidget *parent = 0);
    ~NTableView();
    NTableViewHeader *tableViewHeader;
    NoteModel *noteModel;
    NoteModel *model();
    NoteSortFilterProxyModel *proxy;
    void mouseReleaseEvent(QMouseEvent *event);
    void openSelectedLids(bool newWindow);
    void refreshSelection();
    void keyPressEvent(QKeyEvent *event);

    QMenu *contextMenu;
    QMenu *colorMenu;
    QAction *addNoteAction;
    QAction *deleteNoteAction;
    QAction *restoreNoteAction;
    QAction *openNoteAction;
    QAction *openNoteExternalWindowAction;
    QAction *openNoteNewTabAction;
    QAction *copyNoteLinkAction;
    QAction *copyNoteAction;
    QAction *pinNoteAction;
    QAction *unpinNoteAction;
    QAction *mergeNotesAction;
    QAction *createTableOfContentsAction;

    QAction *noteTitleColorWhiteAction;
    QAction *noteTitleColorRedAction;
    QAction *noteTitleColorBlueAction;
    QAction *noteTitleColorGreenAction;
    QAction *noteTitleColorYellowAction;
    QAction *noteTitleColorBlackAction;
    QAction *noteTitleColorGrayAction;
    QAction *noteTitleColorCyanAction;
    QAction *noteTitleColorMagentaAction;

    void getSelectedLids(QList<qint32> &lids);
    QList<qint32> priorLidOrder;
    QList<qint32> priorSelectedLids;
    bool isLidSelected(qint32 lid);
    qint32 selectAnyNoteFromList();
    void resizeColumns();
    void repositionColumns();
    void saveColumnsVisible();
    void setColumnsVisible();

signals:
    void noteAdded(qint32 newNote);
    void openNote(bool newWindow);
    void notesDeleted(QList<qint32> lid, bool expunged);
    void notesRestored(QList<qint32>);
    void refreshNoteContent(qint32 lid);
    void openNoteExternalWindow(qint32 lid);
    void saveAllNotes();
    void newNote();

public slots:
    void refreshData();
    void contextMenuEvent(QContextMenuEvent *event);
    void deleteSelectedNotes();
    void restoreSelectedNotes();
    void openNoteContextMenuTriggered();
    void openNoteExternalWindowTriggered();
    void openNoteNewTabTriggered();
    void copyNoteLink();
    void toggleColumnVisible(int position, bool visible);
    void copyNote();
    void pinNote();
    void unpinNote();
    void createNewNote();
    void mergeNotes();
    void createTableOfContents();
    void refreshCell(qint32 lid, int cell, QVariant data);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void setTitleColorWhite();
    void setTitleColorRed();
    void setTitleColorBlue();
    void setTitleColorGreen();
    void setTitleColorYellow();
    void setTitleColorBlack();
    void setTitleColorGray();
    void setTitleColorCyan();
    void setTitleColorMagenta();
    void setTitleColor(QString color);

    void downNote();
    void upNote();

};

#endif // NTABLEVIEW_H

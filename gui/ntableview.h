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
#include "filters/notesortfilterproxymodel.h"
#include <boost/shared_ptr.hpp>

class NTableViewHeader;

class NTableView : public QTableView
{
    Q_OBJECT
private:
    //unsigned int filterPosition;
    DateDelegate *dateDelegate;
    NumberDelegate *blankNumber;
    NumberDelegate *kbNumber;


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
    QAction *deleteNoteAction;
    QAction *restoreNoteAction;
    QAction *openNoteAction;
    QAction *copyNoteLinkAction;
    QAction *copyNoteAction;

    void getSelectedLids(QList<qint32> &lids);
    bool isLidSelected(qint32 lid);
    qint32 selectAnyNoteFromList();

signals:
    void noteAdded(qint32 newNote);
    void openNote(bool newWindow);
    void notesDeleted(QList<qint32> lid, bool expunged);
    void notesRestored(QList<qint32>);


public slots:
    void refreshData();
    void contextMenuEvent(QContextMenuEvent *event);
    void deleteSelectedNotes();
    void restoreSelectedNotes();
    void openNoteContextMenuTriggered();
    void copyNoteLink();
    void toggleColumnVisible(int position, bool visible);
    void copyNote();

};

#endif // NTABLEVIEW_H

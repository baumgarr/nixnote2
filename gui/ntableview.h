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
    NTableViewHeader *header;
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
    void openNote(bool newWindow);
    void notesDeleted(QList<qint32> lid, bool expunged);
    void notesRestored(QList<qint32>);


public slots:
    void refreshData();
    void focusOutEvent(QFocusEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void deleteSelectedNotes();
    void restoreSelectedNotes();
    void openNoteContextMenuTriggered();
    void copyNoteLink();
    void copyNote();

};

#endif // NTABLEVIEW_H

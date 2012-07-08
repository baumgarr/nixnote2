#ifndef NTABLEVIEW_H
#define NTABLEVIEW_H

#include <QTableView>
#include "ntableviewheader.h"
#include "models/notemodel.h"
#include "filters/notesortfilterproxymodel.h"
#include <boost/shared_ptr.hpp>

class NTableViewHeader;

class NTableView : public QTableView
{
    Q_OBJECT
private:
    unsigned int filterPosition;
    QList<int> validLids;

public:
    explicit NTableView(QWidget *parent = 0);
    ~NTableView();
    NTableViewHeader *header;
    NoteModel *noteModel;
    NoteModel *model();
    NoteSortFilterProxyModel *proxy;
    void mouseReleaseEvent(QMouseEvent *event);
    void getSelectedLids(bool newWindow);
    void refreshSelection();

signals:
    void openNote(bool newWindow);


public slots:
    void refreshData();

};

#endif // NTABLEVIEW_H

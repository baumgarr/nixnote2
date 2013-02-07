#ifndef NTABLEVIEW_H
#define NTABLEVIEW_H

#include <QTableView>
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
    void getSelectedLids(bool newWindow);
    void refreshSelection();
    void keyPressEvent(QKeyEvent *event);

signals:
    void openNote(bool newWindow);


public slots:
    void refreshData();
    void focusOutEvent(QFocusEvent *event);

};

#endif // NTABLEVIEW_H

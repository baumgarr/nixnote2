#ifndef NTAGVIEW_H
#define NTAGVIEW_H
#include "ntagviewitem.h"
#include <QTreeWidget>

class NTagView : public QTreeWidget
{
    Q_OBJECT
private:
    NTagViewItem *root;
    QHash<int, NTagViewItem*> dataStore;
    virtual void mousePressEvent(QMouseEvent *event);
    int filterPosition;


private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();

public:
    explicit NTagView(QWidget *parent = 0);
    ~NTagView();
    void resetSize();
    void updateSelection();
    void loadData();
    bool rebuildTagTreeNeeded;
    void dragEnterEvent(QDragEnterEvent *event);

signals:
    void updateSelectionRequested();

public slots:
    void tagUpdated(int lid, QString name);
    void rebuildTree();
    void buildSelection();
    void addNewTag(int lid);

protected:
    void dragMoveEvent(QDragMoveEvent *event);

};

#endif // NTAGVIEW_H

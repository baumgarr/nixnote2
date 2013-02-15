#ifndef NTRASHTREE_H
#define NTRASHTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>

class NTrashTree : public QTreeWidget
{
    Q_OBJECT
private:
    int filterPosition;
    QTreeWidgetItem *root;
    QMenu contextMenu;
    QAction *restoreAction;
    QAction *expungeAction;
    virtual void mousePressEvent(QMouseEvent *event);

public:
    explicit NTrashTree(QWidget *parent = 0);
    void updateSelection();
    void resetSize();
    qint32 count;

signals:
    void updateSelectionRequested();

public slots:
    void contextMenuEvent(QContextMenuEvent *event);
    void updateTotals(qint32 total);

private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void buildSelection();
    void restoreAll();
    void expungeAll();
    
};

#endif // NTRASHTREE_H

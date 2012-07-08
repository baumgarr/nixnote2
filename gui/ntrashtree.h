#ifndef NTRASHTREE_H
#define NTRASHTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

class NTrashTree : public QTreeWidget
{
    Q_OBJECT
private:
    int filterPosition;
    QTreeWidgetItem *root;
    virtual void mousePressEvent(QMouseEvent *event);

public:
    explicit NTrashTree(QWidget *parent = 0);
    void updateSelection();
    void resetSize();

signals:
    void updateSelectionRequested();

public slots:

private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void buildSelection();
    
};

#endif // NTRASHTREE_H

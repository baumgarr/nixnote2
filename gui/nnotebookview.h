#ifndef NNOTEBOOKVIEW_H
#define NNOTEBOOKVIEW_H
#include "nnotebookviewitem.h"
#include <QTreeWidget>

class NNotebookView : public QTreeWidget
{
    Q_OBJECT
private:
    NNotebookViewItem *root;
    QHash<int, NNotebookViewItem*> dataStore;
    QHash<QString, NNotebookViewItem*> stackStore;
    virtual void mousePressEvent(QMouseEvent *event);
    int filterPosition;


private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();

public:
    explicit NNotebookView(QWidget *parent = 0);
    ~NNotebookView();
    void resetSize();
    void updateSelection();
    void loadData();
    bool rebuildNotebookTreeNeeded;

signals:
    void updateSelectionRequested();

public slots:
    void notebookUpdated(int lid, QString name);
    void rebuildTree();
    void buildSelection();
};

#endif // NNOTEBOOKVIEW_H

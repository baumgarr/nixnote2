#ifndef NSEARCHVIEW_H
#define NSEARCHVIEW_H

#include <QTreeWidget>
#include <QHash>
#include <QMenu>
#include <QAction>
#include <QShortcut>

#include "global.h"
#include "nsearchviewitem.h"
#include "gui/treewidgeteditor.h"

//****************************************************
//* This is the tree for the list of saved searches.
//****************************************************



class NSearchView : public QTreeWidget
{
    Q_OBJECT
private:
    NSearchViewItem *root;
    QHash<qint32, NSearchViewItem*> dataStore;
    virtual void mousePressEvent(QMouseEvent *event);
    int filterPosition;
    QMenu context;
    QAction *addAction;
    QAction *propertiesAction;
    QAction *deleteAction;
    QAction *renameAction;
    QShortcut *renameShortcut;
    QShortcut *addShortcut;
    QShortcut *deleteShortcut;
    TreeWidgetEditor *editor;


private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void editComplete();

public:
    explicit NSearchView(QWidget *parent = 0);
    ~NSearchView();
    void resetSize();
    void loadData();
    void contextMenuEvent(QContextMenuEvent *event);

signals:
    void updateSelectionRequested();

public slots:
    void searchUpdated(qint32 lid, QString name);
    void searchExpunged(qint32 lid);
    void buildSelection();
    void updateSelection();
    void addRequested();
    void propertiesRequested();
    void deleteRequested();
    void renameRequested();


};

#endif // NSEARCHVIEW_H

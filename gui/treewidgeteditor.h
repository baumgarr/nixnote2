#ifndef TREEWIDGETEDITOR_H
#define TREEWIDGETEDITOR_H

#include <QLineEdit>
#include <QTreeWidget>

class TreeWidgetEditor : public QLineEdit
{
    Q_OBJECT
private:
    QTreeWidget *parent;
    QTreeWidgetItem *originalWidgetItem;
    QWidget *originalWidget;
    int column;

public:
    explicit TreeWidgetEditor(QTreeWidget *parent = 0);
    void setTreeWidgetItem(QTreeWidgetItem *w, int col);
    int lid;
    
signals:
    void editComplete();
    
public slots:

protected:
    virtual void focusOutEvent(QFocusEvent *e);
    
};

#endif // TREEWIDGETEDITOR_H

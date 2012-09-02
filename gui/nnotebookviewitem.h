#ifndef NNOTEBOOKVIEWITEM_H
#define NNOTEBOOKVIEWITEM_H

#include <QTreeWidgetItem>

class NNotebookViewItem : public QTreeWidgetItem
{
public:
    explicit NNotebookViewItem();
    explicit NNotebookViewItem(QTreeWidget* parent);
    QString stack;
    QList<qint32> childrenLids;

    bool operator<(const QTreeWidgetItem &other)const;
    void setRootColor(bool val) ;


signals:

public slots:

};

#endif // NNOTEBOOKVIEWITEM_H

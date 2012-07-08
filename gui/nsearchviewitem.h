#ifndef NSEARCHVIEWITEM_H
#define NSEARCHVIEWITEM_H

#include <QTreeWidgetItem>

class NSearchViewItem : public QTreeWidgetItem
{
public:
    explicit NSearchViewItem(QTreeWidget* parent);
    explicit NSearchViewItem();
    void setRootColor(bool value);

    bool operator<(const QTreeWidgetItem &other)const;

signals:

public slots:

};

#endif // NSEARCHVIEWITEM_H

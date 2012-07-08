#ifndef NTAGVIEWITEM_H
#define NTAGVIEWITEM_H

#include <QTreeWidgetItem>

class NTagViewItem : public QTreeWidgetItem
{
public:
    explicit NTagViewItem();
    explicit NTagViewItem(QTreeWidget* parent);
    int parentLid;
    QList<int> childrenGuids;
    QString parentGuid;

    bool operator<(const QTreeWidgetItem &other)const;
    void setRootColor(bool val) ;


signals:

public slots:

};

#endif // NTAGVIEWITEM_H

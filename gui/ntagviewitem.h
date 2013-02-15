#ifndef NTAGVIEWITEM_H
#define NTAGVIEWITEM_H

#include <QTreeWidgetItem>

class NTagViewItem : public QTreeWidgetItem
{
public:
    explicit NTagViewItem();
    explicit NTagViewItem(QTreeWidget* parent);
    qint32 parentLid;
    QList<qint32> childrenGuids;
    QString parentGuid;
    qint32 count;

    bool operator<(const QTreeWidgetItem &other)const;
    void setRootColor(bool val) ;


signals:

public slots:

};

#endif // NTAGVIEWITEM_H

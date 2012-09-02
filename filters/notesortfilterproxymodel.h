#ifndef NOTESORTFILTERPROXYMODEL_H
#define NOTESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QMap>
#include <stdint.h>

class NoteSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit NoteSortFilterProxyModel();
    ~NoteSortFilterProxyModel();
    bool filterAcceptsRow(qint32 source_row, const QModelIndex &source_parent) const;
    //void sort(int column, Qt::SortOrder order);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    QMap<qint32, qint32> *lidMap;

signals:

public slots:

};

#endif // NOTESORTFILTERPROXYMODEL_H

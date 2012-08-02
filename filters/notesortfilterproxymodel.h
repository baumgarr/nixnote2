#ifndef NOTESORTFILTERPROXYMODEL_H
#define NOTESORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QMap>

class NoteSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit NoteSortFilterProxyModel();
    ~NoteSortFilterProxyModel();
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    //void sort(int column, Qt::SortOrder order);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    QMap<int, int> *lidMap;

signals:

public slots:

};

#endif // NOTESORTFILTERPROXYMODEL_H

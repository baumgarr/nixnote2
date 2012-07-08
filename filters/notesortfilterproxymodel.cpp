#include "notesortfilterproxymodel.h"

NoteSortFilterProxyModel::NoteSortFilterProxyModel() :
    QSortFilterProxyModel()
{
}


bool NoteSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    return true;
}



void NoteSortFilterProxyModel::sort(int column, Qt::SortOrder order) {
    this->QAbstractProxyModel::sort(column, order);
}

bool NoteSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QVariant leftData = this->sourceModel()->data(left);
    QVariant rightData = this->sourceModel()->data(right);

    if (leftData.type() == QVariant::Invalid || rightData.type() == QVariant::Invalid)
        return true;

    if (leftData.type() == QVariant::Icon || rightData.type() == QVariant::Icon)
        return true;

    if (leftData.type() == QVariant::Image || rightData.type() == QVariant::Image)
        return true;

    if (leftData.type() == QVariant::String && rightData.type() == QVariant::String) {
        QString left = leftData.toString();
        QString right = rightData.toString();
        return left.toLower() < right.toLower();
    }
    return true;


}

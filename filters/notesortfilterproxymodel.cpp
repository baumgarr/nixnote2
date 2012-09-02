#include "notesortfilterproxymodel.h"
#include "global.h"

NoteSortFilterProxyModel::NoteSortFilterProxyModel() :
    QSortFilterProxyModel()
{
    QLOG_TRACE() << "Entering NoteSortFilterProxyModel constructor";
    lidMap = new QMap<qint32,qint32>();
    QLOG_TRACE() << "Exiting NoteSortFilterProxyModel constructor";
}


NoteSortFilterProxyModel::~NoteSortFilterProxyModel()
{
    QLOG_TRACE() << "Entering NoteSortFilterProxyModel destructor";
    delete lidMap;
    QLOG_TRACE() << "Exiting NoteSortFilterProxyModel constructor";
}


bool NoteSortFilterProxyModel::filterAcceptsRow(qint32 source_row, const QModelIndex &source_parent) const {
    QModelIndex idx = sourceModel()->index(source_row,NOTE_TABLE_LID_POSITION, source_parent);
    qint32 rowLid = sourceModel()->data(idx).toInt();
    if (lidMap->contains(rowLid)) {
        lidMap->remove(rowLid);
        lidMap->insert(rowLid, source_row);
        return true;
    }
    return false;
}


//void NoteSortFilterProxyModel::sort(int column, Qt::SortOrder order) {
//    this->QAbstractProxyModel::sort(column, order);
//}

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

    if (leftData.type() == QVariant::Int && rightData.type() == QVariant::Int)
        return leftData.toInt() < rightData.toInt();

    if (leftData.type() == QVariant::LongLong && rightData.type() == QVariant::LongLong)
        return leftData.toLongLong() < rightData.toLongLong();

    if (leftData.type() == QVariant::Double && rightData.type() == QVariant::Double)
        return leftData.toDouble() < rightData.toDouble();

    return true;
}

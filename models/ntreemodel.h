#ifndef NTREEMODEL_H
#define NTREEMODEL_H

#include <QAbstractItemModel>

class NTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit NTreeModel(QObject *parent = 0);

signals:

public slots:

};

#endif // NTREEMODEL_H

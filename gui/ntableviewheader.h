#ifndef NTABLEVIEWHEADER_H
#define NTABLEVIEWHEADER_H

#include <QTableView>
#include <QHeaderView>


class NTableViewHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit NTableViewHeader(Qt::Orientation orientation, QWidget *parent = 0);

signals:

public slots:

};

#endif // NTABLEVIEWHEADER_H

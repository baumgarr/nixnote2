#ifndef NTAGVIEWDELEGATE_H
#define NTAGVIEWDELEGATE_H

#include <QStyledItemDelegate>

class NTagViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NTagViewDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // NTAGVIEWDELEGATE_H

#ifndef NTRASHVIEWDELEGATE_H
#define NTRASHVIEWDELEGATE_H

#include <QStyledItemDelegate>

class NTrashViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NTrashViewDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // NTRASHVIEWDELEGATE_H

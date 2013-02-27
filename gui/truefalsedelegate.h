#ifndef TRUEFALSEDELEGATE_H
#define TRUEFALSEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

class TrueFalseDelegate : public QStyledItemDelegate
        
{
public:
    TrueFalseDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // TRUEFALSEDELEGATE_H

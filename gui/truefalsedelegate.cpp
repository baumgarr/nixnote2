#include "truefalsedelegate.h"
#include "global.h"
#include "sql/notetable.h"

TrueFalseDelegate::TrueFalseDelegate()
{
}


void TrueFalseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool value = index.data().toBool();
    QModelIndex ix;  // Dummy model index so we don't put the actual value in the column
    QStyledItemDelegate::paint(painter,option, ix);
    if(value)
    {
        painter->save();
        QPixmap dot(":black_dot.png");
        int centerDot = dot.width()/2;
        int len = (option.rect.right() - option.rect.left())/2;
        len = len-centerDot;
        painter->drawPixmap(option.rect.x()+len,option.rect.y()+2,dot);
        painter->restore();
    }

}

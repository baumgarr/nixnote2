#include "ntrashviewdelegate.h"
#include <QPainter>
#include "ntrashtree.h"

NTrashViewDelegate::NTrashViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}




void NTrashViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    NTrashTree  *tree = (NTrashTree*)options.widget;
    qint32 count = tree->count;
    QString countString = QString("(")+QString::number(count) + QString(")");

    // shift text right to make icon visible
    QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left()+iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());

    painter->setClipRect(clip);
    options.font.setBold(true);
    QFontMetrics fm = options.fontMetrics;
    painter->setPen(Qt::darkGray);
    painter->drawText(fm.width(index.data().toString()+QString("   ")),fm.height()-fm.descent()-1,countString);
    \
    painter->restore();
}

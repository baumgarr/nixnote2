#ifndef NNOTEBOOKVIEWDELEGATE_H
#define NNOTEBOOKVIEWDELEGATE_H

#include <QStyledItemDelegate>

class NNotebookViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NNotebookViewDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // NNOTEBOOKVIEWDELEGATE_H

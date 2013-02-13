#ifndef POPPLERGRAPHICSVIEW_H
#define POPPLERGRAPHICSVIEW_H

#include <QGraphicsView>

class PopplerGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PopplerGraphicsView(QWidget *parent = 0);
    explicit PopplerGraphicsView(QGraphicsScene *&scene);
    QString filename;
    
signals:
    
public slots:
     void mousePressEvent(QMouseEvent * e);

    
};

#endif // POPPLERGRAPHICSVIEW_H

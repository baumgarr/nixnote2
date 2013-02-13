#include "popplergraphicsview.h"
#include "global.h"
#include <QDesktopServices>
#include <QGraphicsView>
#include <QUrl>

extern Global global;

PopplerGraphicsView::PopplerGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
}

PopplerGraphicsView::PopplerGraphicsView(QGraphicsScene *&scene) :
    QGraphicsView(scene)
{
}



void PopplerGraphicsView::mousePressEvent(QMouseEvent * e) {
   QDesktopServices::openUrl(QUrl(filename));
   QGraphicsView(parent);
}

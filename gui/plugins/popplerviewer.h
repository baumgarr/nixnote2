#ifndef POPPLERVIEWER_H
#define POPPLERVIEWER_H

#include <QImage>
#include <QStandardItemModel>
#include <QList>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <poppler-qt4.h>

#include "gui/plugins/popplergraphicsview.h"

class PopplerViewer : public QWidget
{
    Q_OBJECT

public:
    PopplerViewer(const QString &mimeType, const QString &lid, QWidget *parent = 0);

private:
    QGraphicsScene *scene;
    PopplerGraphicsView *view;
    QGraphicsPixmapItem *item;
    QImage *image;
    QString mimeType;
    QLabel *imageLabel;
    int currentPage;
    int totalPages;
    QPushButton *pageLeft;
    QPushButton *pageRight;
    qint32 lid;
    QString printImageFile;

public:
    Poppler::Document *doc;

public slots:
    void pageRightPressed();
    void pageLeftPressed();
};

#endif // POPPLERVIEWER_H

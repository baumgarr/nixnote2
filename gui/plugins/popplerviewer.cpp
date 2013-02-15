#include "popplerviewer.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <poppler-qt4.h>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPushButton>
#include <global.h>

extern Global global;

PopplerViewer::PopplerViewer(const QString &mimeType, const QString &reslid, QWidget *parent) :
    QWidget(parent)
{
    this->mimeType = mimeType;
    this->lid = reslid.toInt();
    printImageFile = global.fileManager.getTmpDirPath() + QString::number(lid) +QString("-print.png");
    QString file = global.fileManager.getDbaDirPath() + reslid +".pdf";
    doc = Poppler::Document::load(file);
    if (doc == NULL)
        return;
    currentPage = 0;
    totalPages = doc->numPages();

    image = new QImage(doc->page(currentPage)->renderToImage());
    scene = new QGraphicsScene();
    view = new PopplerGraphicsView(scene);
    view->filename = file;
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    image->save(printImageFile);   // This is in case we want to print a note.  Otherwise it isn't used.
    scene->addItem(item);
    view->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    pageLeft = new QPushButton();
    pageRight = new QPushButton();
    pageRight->setMaximumWidth(30);
    pageLeft->setMaximumWidth(30);
    pageLeft->setIcon(QIcon(":left_arrow.png"));
    pageRight->setIcon(QIcon(":right_arrow.png"));
    buttonLayout->addWidget(pageLeft);
    buttonLayout->addWidget(pageRight);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(buttonLayout);
    layout->addWidget(view);
    this->setLayout(layout);
    connect(pageRight, SIGNAL(clicked()), this, SLOT(pageRightPressed()));
    connect(pageLeft, SIGNAL(clicked()), this, SLOT(pageLeftPressed()));
    if (totalPages == 1) {
        pageRight->setEnabled(false);
    }
    pageLeft->setEnabled(false);
}


void PopplerViewer::pageRightPressed() {
    if (currentPage+1 < totalPages) {
        currentPage++;
        if (image != NULL)
            delete image;
        image = new QImage(doc->page(currentPage)->renderToImage());
        image->save(printImageFile);
        if (item != NULL)
            delete item;
        item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
        scene->addItem(item);
        if (currentPage+1 == totalPages)
            pageRight->setEnabled(false);
        if (currentPage>0)
            pageLeft->setEnabled(true);
    }

}

void PopplerViewer::pageLeftPressed() {
    if (currentPage>0) {
        currentPage--;
        if (image != NULL)
            delete image;
        image = new QImage(doc->page(currentPage)->renderToImage());
        if (item != NULL)
            delete item;
        item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
        scene->addItem(item);
        if (currentPage==0)
            pageLeft->setEnabled(false);
        if (totalPages>1)
            pageRight->setEnabled(true);
    }
}


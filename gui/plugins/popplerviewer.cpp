/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

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
    pageLabel = new QLabel(tr("Page ") +QString::number(currentPage+1) + QString(tr(" of ") +QString::number(totalPages)), this);
    pageLeft = new QPushButton();
    pageRight = new QPushButton();
    pageRight->setMaximumWidth(30);
    pageLeft->setMaximumWidth(30);
    pageLeft->setIcon(QIcon(":left_arrow.png"));
    pageRight->setIcon(QIcon(":right_arrow.png"));
    buttonLayout->addStretch(100);
    buttonLayout->addWidget(pageLeft);
    buttonLayout->addWidget(pageLabel);
    buttonLayout->addWidget(pageRight);
    buttonLayout->addStretch(100);
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
        pageLabel->setText(tr("Page ") +QString::number(currentPage+1) + QString(tr(" of ") +QString::number(totalPages)));

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


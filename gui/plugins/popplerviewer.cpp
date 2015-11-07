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
#if QT_VERSION < 0x050000
#include <poppler-qt4.h>
#else
#include <poppler-qt5.h>
#endif
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPushButton>
#include "filters/filterengine.h"
#include <global.h>

extern Global global;

PopplerViewer::PopplerViewer(const QString &mimeType, const QString &reslid, QWidget *parent) :
    QWidget(parent)
{
    pageLabel = new QLabel(this);
    this->mimeType = mimeType;
    this->lid = reslid.toInt();
    printImageFile = global.fileManager.getTmpDirPath() + QString::number(lid) +QString("-print.png");
    QString file = global.fileManager.getDbaDirPath() + reslid +".pdf";
    doc = Poppler::Document::load(file);
    if (doc == NULL || doc->isLocked())
        return;

    currentPage = 0;

    totalPages = doc->numPages();

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    searchHits.empty();
    QList<QRectF> searchLocations;
    if (criteria->isSearchStringSet() && criteria->getSearchString() != "") {
        FilterEngine engine;
        if (engine.resourceContains(lid, criteria->getSearchString(), &searchHits)) {
            pageLabel->setStyleSheet("QLabel { background-color : yellow; }");
            findNextPage(searchHits, &searchLocations);
        }
    }


    image = new QImage(doc->page(currentPage)->renderToImage());    

    QPixmap finalPix = highlightImage();

    scene = new QGraphicsScene();
    view = new PopplerGraphicsView(scene);
    view->filename = file;
    item = new QGraphicsPixmapItem(finalPix);
    finalPix.save(printImageFile);   // This is in case we want to print a note.  Otherwise it isn't used.
    scene->addItem(item);

    view->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    pageLabel->setText(tr("Page ") +QString::number(currentPage+1) + QString(tr(" of ") +QString::number(totalPages)));

    pageLeft = new QPushButton();
    pageRight = new QPushButton();
    pageRight->setMaximumWidth(30);
    pageLeft->setMaximumWidth(30);
    pageLeft->setIcon(global.getIconResource(":leftArrowIcon"));
    pageRight->setIcon(global.getIconResource(":rightArrowIcon"));
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
    if (currentPage == 1)
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
        QPixmap finalPix = highlightImage();
        item = new QGraphicsPixmapItem(finalPix);
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

        QPixmap finalPix = highlightImage();
        item = new QGraphicsPixmapItem(finalPix);

        scene->addItem(item);
        if (currentPage==0)
            pageLeft->setEnabled(false);
        if (totalPages>1)
            pageRight->setEnabled(true);
    }
}


// Search for the next page containing text
void PopplerViewer::findNextPage(QStringList searchHits, QList<QRectF> *searchLocations) {

    int page = currentPage;
    bool found = false;
    QLOG_DEBUG() << searchHits.size();
    searchLocations->clear();

    while (page < doc->numPages() && !found) {
        for (int i=0; i<searchHits.size(); i++) {
            QList<QRectF> results = doc->page(page)->search(searchHits[i], Poppler::Page::CaseInsensitive);
            searchLocations->append(results);
            if (results.size() > 0) {
                currentPage = page;
                return;
            }
        }
        page++;
    }
}


QPixmap PopplerViewer::highlightImage() {
    // Highlight any search terms
    QPixmap overlayPix(image->size());
    overlayPix.fill(Qt::transparent);
    QPainter p2(&overlayPix);
    p2.setBackgroundMode(Qt::TransparentMode);
    p2.setRenderHint(QPainter::Antialiasing,true);
    QColor yellow(Qt::yellow);
    p2.setBrush(yellow);

    QList<QRectF> searchLocations;
    for (int i=0; i<searchHits.size(); i++) {
        searchLocations.append(doc->page(currentPage)->search(searchHits[i], Poppler::Page::CaseInsensitive));
    }
    for (int i=0; i<searchLocations.size(); i++) {
        QRectF highlightRect = searchLocations[i];
        p2.drawRect(highlightRect.x(), highlightRect.y(), highlightRect.width(), highlightRect.height());
    }
    if (searchLocations.size() > 0)
        pageLabel->setStyleSheet("QLabel { background-color : yellow; }");
    else
        pageLabel->setStyleSheet("");



    // Create the actual overlay.  We do this in two steps to avoid
    // constantly painting the same area
    QPixmap finalPix(image->size());
    finalPix.fill(Qt::transparent);
    QPainter p3(&finalPix);
    p3.setBackgroundMode(Qt::TransparentMode);
    p3.setRenderHint(QPainter::Antialiasing,true);
    p3.drawPixmap(0,0,QPixmap::fromImage(*image));
    p3.setOpacity(0.4);
    p3.drawPixmap(0,0,overlayPix);
    p3.end();
    return finalPix;
}

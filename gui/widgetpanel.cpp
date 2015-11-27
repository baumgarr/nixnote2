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

#include "widgetpanel.h"
#include <QVBoxLayout>
#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QScrollArea>
#include <QDragMoveEvent>
#include "global.h"

extern Global global;


WidgetPanel::WidgetPanel(QWidget *parent) :
    QWidget(parent)
{
    vboxLayout = new QVBoxLayout();
    this->setLayout(vboxLayout);
    vboxLayout->setSpacing(10);
    vboxLayout->setSizeConstraint(QLayout::SetNoConstraint);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->setStyleSheet("QTreeView {background:transparent; border:none; margin:0px; padding: 0px;} ");
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    connect(&timer, SIGNAL(timeout()), this, SLOT(scrollTimer()));
}


WidgetPanel::~WidgetPanel() {
    delete vboxLayout;
}

void WidgetPanel::addWidget(QWidget *widget) {
    widget->setParent(this);
    vboxLayout->addWidget(widget);
}




void WidgetPanel::paintEvent(QPaintEvent *e)
 {
    //Suppress unused
    Q_UNUSED(e);
    return;
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }



QSize WidgetPanel::sizeHint() {
    return minimumSizeHint();
}



void WidgetPanel::addFavoritesView(FavoritesView *view) {
    view->setObjectName("Favorites");
    this->favoritesView = view;
    addWidget(view);
}

void WidgetPanel::addNotebookView(NNotebookView *view) {
    view->setObjectName("Notebooks");
    notebookView = view;
    addWidget(view);
}

void WidgetPanel::addTagView(NTagView *view) {
    view->setObjectName("Tags");
    tagView = view;
    addWidget(view);
}

void WidgetPanel::addSearchView(NSearchView *view) {
    view->setObjectName("SavedSearches");
    searchView = view;
    addWidget(view);
}


void WidgetPanel::addTrashTree(NTrashTree *view) {
    view->setObjectName("Trash");
    trashTree = view;
    addWidget(view);
}


void WidgetPanel::addAttributeTree(NAttributeTree *view) {
    view->setObjectName("Attributes");
    attributeTree = view;
    addWidget(view);
}


void WidgetPanel::addSeparator(QLabel *separator) {
    addWidget(separator);
}




void WidgetPanel::scrollTimer() {
    QScrollArea *scrollArea = (QScrollArea*)this->parentWidget();
    if (scrollUp) {
        int visibleY = this->visibleRegion().boundingRect().y();
        if (visibleY == 0) {
            timer.stop();
            return;
        }
        scrollArea->scroll(0,1);
    } else {
        bool bottomVisible = this->visibleRegion().contains(tagView->geometry().bottomLeft());
        if (!bottomVisible)
            scrollArea->scroll(0,-1);
        else
            timer.stop();
    }
}



void WidgetPanel::dragEnterHandler(QDragEnterEvent *event) {
    Q_UNUSED(event);
    priorMousePosition = QCursor::pos();
}


void WidgetPanel::dragMoveHandler(QDragMoveEvent *event) {

   //QPoint pos = event->source()->mapToGlobal(event->pos());
    QWidget *source = qobject_cast<QWidget *>(event->source());
    QPoint pos = QCursor::pos();
    pos = this->mapFromGlobal(pos);
    int mouse = QCursor::pos().y();


    // Check if we are close to the top.  If so, then start scrolling up.
    if (pos.y()<priorMousePosition.y()) {
        int visibleY = this->visibleRegion().boundingRect().y();
        int top =  this->mapFromGlobal(this->geometry().topLeft()).y() *-1;
        if (!scrollUp)
            timer.stop();
        if (mouse-top < 50 && visibleY > 0) {
            scrollUp = true;
            timer.start(5);
        }
    }

    // Check if we are close to the bottom.  If so, start scrolling down
    if (pos.y()>priorMousePosition.y()) {
        timer.stop();
        scrollUp=false;
        if (source->objectName()=="Tags") {
           int bottom = source->mapToGlobal(source->visibleRegion().boundingRect().bottomLeft()).y();
           int actualBottom = source->mapToGlobal(source->geometry().bottomLeft()).y();
           bool bottomVisible = this->visibleRegion().contains(source->geometry().bottomLeft());
           if (bottom-mouse < 50 && bottom < actualBottom && !bottomVisible) {
                timer.start(5);
            }
        }
    }
    priorMousePosition = pos;
}


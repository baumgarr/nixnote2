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

#include "tagviewer.h"
#include "global.h"

#include <QIcon>
#include <QFontMetrics>
#include <QStyle>

// Suppress C++ string wanings
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic push


TagViewer::TagViewer(QWidget *parent) :
    QLabel(parent)
{

    clearButton = new QToolButton(this);
    clearButton->setIcon(QIcon(":fileclose.png"));
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->setCursor(Qt::PointingHandCursor);
    setStyleSheet("QFrame{border: 1px solid darkgray; border-radius: 8px;}");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    QFont f = font();
    f.setPointSize(8);
    setFont(f);
//    int width = this->minimumWidth()+clearButton->minimumWidth()+2;
//    setMinimumWidth(width);
//    resize();
}

//QSize TagViewer::sizeHint() {
//    QSize size;
//    size.setWidth(this->minimumWidth()+clearButton->minimumWidth()+2);
//    size.setHeight(this->minimumHeight());
//}

void TagViewer::resizeEvent(QResizeEvent *)
{
    resize();
}


void TagViewer::updateCloseButton(const QString& text)
{
//    clearButton->setVisible(!text.isEmpty());
}


void TagViewer::hideEvent(QHideEvent* event) {
    event=event;  // suppress unused
    hide();
    clearButton->hide();
}

void TagViewer::showEvent(QShowEvent* event) {
    event=event;  // suppress unused
    show();
    clearButton->show();
}


void TagViewer::resize() {
    QFontMetrics fm(font());
    int width = fm.width(text());
    int height = fm.height();

//    setMaximumWidth(width+sz.width()+2+clearButton->minimumWidth());
//    setMinimumWidth(width+sz.width()+2+clearButton->minimumWidth());

    int x = width + clearButton->minimumWidth()+20;
    setMinimumWidth(x);
    setMinimumWidth(x);

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().left() + width + frameWidth,
                      (rect().bottom()- height - 5 )/2);
}


void TagViewer::buttonClicked() {
    emit(closeClicked(text()));
}



#pragma GCC diagnostic pop

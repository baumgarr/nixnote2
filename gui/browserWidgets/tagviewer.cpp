#include "tagviewer.h"
#include "global.h"

#include <QIcon>
#include <QFontMetrics>
#include <QStyle>

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


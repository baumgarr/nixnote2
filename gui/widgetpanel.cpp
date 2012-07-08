#include "widgetpanel.h"
#include <QVBoxLayout>

WidgetPanel::WidgetPanel(QWidget *parent) :
    QWidget(parent)
{
    vboxLayout = new QVBoxLayout();
    this->setLayout(vboxLayout);
    vboxLayout->setSpacing(0);
    vboxLayout->setSizeConstraint(QLayout::SetNoConstraint);
}

void WidgetPanel::addWidget(QWidget *widget) {
    vboxLayout->addWidget(widget);
}

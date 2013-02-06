#include "widgetpanel.h"
#include <QVBoxLayout>

WidgetPanel::WidgetPanel(QWidget *parent) :
    QWidget(parent)
{
    vboxLayout = new QVBoxLayout();
    this->setLayout(vboxLayout);
    vboxLayout->setSpacing(10);
    vboxLayout->setSizeConstraint(QLayout::SetNoConstraint);
}

WidgetPanel::~WidgetPanel() {
    delete vboxLayout;
}

void WidgetPanel::addWidget(QWidget *widget) {
    vboxLayout->addWidget(widget);
}







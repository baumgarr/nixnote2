#include "colormenu.h"

ColorMenu::ColorMenu(QObject *parent) :
    QObject(parent)
{
    this->parent = parent;
    currentColor.setNamedColor("black");
    populateList();
}




void ColorMenu::populateList() {
    QStringList list = QColor::colorNames();
    for (int i=0; i<list.size(); i++) {
        QPixmap pix(QSize(22,22));
        pix.fill(QColor(list[i]));
        QAction *newAction = new QAction(QIcon(pix), "", parent);
        newAction->setToolTip(list[i]);
        newAction->setText(list[i]);
        menu.addAction(newAction);
        connect(newAction, SIGNAL(hovered()), this, SLOT(itemHovered()));
    }
}


QColor *ColorMenu::getColor() {
    return &currentColor;
}


QMenu *ColorMenu::getMenu() {
    return &menu;
}



void ColorMenu::setDefault(QColor color) {
    currentColor = color;
}

void ColorMenu::itemHovered() {
    if (menu.activeAction() != NULL && menu.activeAction()->toolTip() != NULL) {
        currentColor.setNamedColor(menu.activeAction()->toolTip());
    }
}

#include "nsearchviewitem.h"

NSearchViewItem::NSearchViewItem(QTreeWidget* parent):QTreeWidgetItem(parent){}

NSearchViewItem::NSearchViewItem():QTreeWidgetItem(){}


bool NSearchViewItem::operator<(const QTreeWidgetItem &other)const {
     int column = treeWidget()->sortColumn();
     return text(column).toLower() < other.text(column).toLower();
}


void NSearchViewItem::setRootColor(bool val) {
    if (val) {
        QFont f;
        f.setBold(true);
        setFont(0, f);
    }
}

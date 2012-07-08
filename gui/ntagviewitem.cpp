#include "ntagviewitem.h"

NTagViewItem::NTagViewItem(QTreeWidget* parent):QTreeWidgetItem(parent) {
    parentLid = 0;
    childrenGuids.empty();
    parentGuid = "";
}


NTagViewItem::NTagViewItem():QTreeWidgetItem(){
    parentLid = 0;
    childrenGuids.empty();
    parentGuid = "";
}

void NTagViewItem::setRootColor(bool val) {
    if (val) {
        QFont f;
        f.setBold(true);
        setFont(0, f);
    }
}


  bool NTagViewItem::operator<(const QTreeWidgetItem &other)const {
    int column = treeWidget()->sortColumn();
    return text(column).toLower() < other.text(column).toLower();
  }

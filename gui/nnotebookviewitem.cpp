#include "nnotebookviewitem.h"

NNotebookViewItem::NNotebookViewItem(QTreeWidget* parent):QTreeWidgetItem(parent) {
    count = 0;
}


NNotebookViewItem::NNotebookViewItem():QTreeWidgetItem(){
}

void NNotebookViewItem::setRootColor(bool val) {
    if (val) {
        QFont f;
        f.setBold(true);
        setFont(0, f);
    }
}


bool NNotebookViewItem::operator<(const QTreeWidgetItem &other)const {
    int column = treeWidget()->sortColumn();
    return text(column).toLower() < other.text(column).toLower();
}




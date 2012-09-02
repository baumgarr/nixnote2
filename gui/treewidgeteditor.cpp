#include "treewidgeteditor.h"
#include <QTreeWidget>

TreeWidgetEditor::TreeWidgetEditor(QTreeWidget *parent) :
    QLineEdit(parent)
{
    this->parent = parent;
    QFont f = font();
    f.setPointSize(8);
    setFont(f);
    lid = 0;
    stackName = "";
}


void TreeWidgetEditor::setTreeWidgetItem(QTreeWidgetItem *w, int col) {
   this->originalWidgetItem = w;
   this->column = col;
   this->originalWidget = parent->itemWidget(w, col);

}

void TreeWidgetEditor::focusOutEvent(QFocusEvent *e) {
    e=e; // suppress unused
    originalWidgetItem->setData(0, Qt::DisplayRole, text().trimmed());
    parent->setItemWidget(originalWidgetItem, column, originalWidget);
    emit(editComplete());
}

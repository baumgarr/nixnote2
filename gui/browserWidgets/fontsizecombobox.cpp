#include "fontsizecombobox.h"

#include <QLineEdit>

FontSizeComboBox::FontSizeComboBox(QWidget *parent) :
    QComboBox(parent)
{
    expanded = false;
    setEditable(true);
    QLineEdit *e;
    e = lineEdit();
    e->setReadOnly(false);
}



void FontSizeComboBox::showPopup() {
    expanded = true;
    QComboBox::showPopup();
}



void FontSizeComboBox::hidePopup() {
    expanded = false;
    QComboBox::hidePopup();
}

bool FontSizeComboBox::isExpanded() {
    return expanded;
}


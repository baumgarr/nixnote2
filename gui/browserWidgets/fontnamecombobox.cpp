#include "fontnamecombobox.h"
#include <QLineEdit>

#include "global.h"



extern Global global;



FontNameComboBox::FontNameComboBox(QWidget *parent) :
    QComboBox(parent)
{
    expanded = false;
    setEditable(true);
    QLineEdit *e;
    e = lineEdit();
    e->setReadOnly(true);

}



void FontNameComboBox::showPopup() {
    expanded = true;
    QComboBox::showPopup();
}



void FontNameComboBox::hidePopup() {
    QComboBox::hidePopup();
    expanded = false;
}

bool FontNameComboBox::isExpanded() {
    return expanded;
}




//void FontNameComboBox::focusInEvent(QFocusEvent *event) {
//    //expanded = true;
//    QComboBox::focusInEvent(event);
//    QLOG_DEBUG() << "focusin";
//}

//void FontNameComboBox::focusOutEvent(QFocusEvent *event) {
//    //expanded = false;
//    QComboBox::focusOutEvent(event);
//    QLOG_DEBUG() << "focusout";
//}

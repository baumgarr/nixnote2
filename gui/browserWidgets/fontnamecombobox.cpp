/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

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

    QString css = global.getThemeCss("fontNameComboCss");
    if (css!="")
        this->setStyleSheet(css);


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

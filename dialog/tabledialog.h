/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>

class TableDialog  : public QDialog
{
    Q_OBJECT
private:
    QGridLayout grid; // = new QGridLayout();
    QGridLayout input; // = new QGridLayout();
    QGridLayout msgGrid; // = new QGridLayout();
    QGridLayout button; // = new QGridLayout();
    bool 			okPressed;
    QSpinBox		rows;
    QSpinBox 		cols;
    QLineEdit       width;
    QPushButton 	ok;
    QPushButton     cancel;
    QLabel          error;
    QIntValidator	widthValidator;
    QComboBox		 unit;


public:
    TableDialog(QWidget *parent);
    bool isOkPressed();
    int getRows();
    int getCols();
    int getWidth();
    bool isPercent();

private slots:
    void validateInput();
    void validateWidth();
    void unitChanged();
    void okButtonPressed();
    void cancelButtonPressed();

};


#endif // TABLEDIALOG_H

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

#ifndef EMAILDIALOG_H
#define EMAILDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>

class EmailDialog : public QDialog
{
    Q_OBJECT
private:
    QLabel          *toLabel;
    QLabel          *bccLabel;
    QLabel          *ccLabel;
    QLabel          *subjectLabel;
    QLabel          *noteLabel;
    QStringList     tokenizeString(QString);

public:
    explicit EmailDialog(QWidget *parent = 0);
    bool sendPressed;
    bool cancelPressed;

    QCheckBox       *ccSelf;
    QLineEdit       *toAddress;
    QLineEdit       *bccAddress;
    QLineEdit       *ccAddress;
    QLineEdit       *subject;

    QPushButton     *sendButton;
    QPushButton     *cancelButton;
    QPlainTextEdit  *note;
    QStringList     getToAddresses();
    QStringList     getCcAddresses();
    QStringList     getBccAddresses();

signals:

public slots:
    void cancelButtonPressed();
    void sendButtonPressed();
    void toAddressChanged();

};

#endif // EMAILDIALOG_H

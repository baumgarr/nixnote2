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

#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QWebPage>

class FindReplace : public QWidget
{
    Q_OBJECT


public:
    explicit FindReplace(QWidget *parent = 0);
    void showFind();
    void showFindReplace();
    QLineEdit *findLine;
    QPushButton *closeButton;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QLineEdit *replaceLine;
    QCheckBox *caseSensitive;
    QWebPage::FindFlags getCaseSensitive();
    
signals:
    
public slots:
    void textChanged();
    
};

#endif // FINDREPLACE_H

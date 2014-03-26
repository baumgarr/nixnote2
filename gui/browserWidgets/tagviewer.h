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

#ifndef TAGVIEWER_H
#define TAGVIEWER_H

#include <QLabel>
#include <QToolButton>

class TagViewer : public QLabel
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *);
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);

private:
    QToolButton *clearButton;

public:
    explicit TagViewer(QWidget *parent = 0);
    void resize();
    
signals:
    void closeClicked(QString);
    
private slots:
//    void updateCloseButton(const QString &text);
    void buttonClicked();
    
};

#endif // TAGVIEWER_H

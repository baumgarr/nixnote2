/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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



#ifndef EXTERNALBROWSE_H
#define EXTERNALBROWSE_H

#include <QMdiSubWindow>
#include <gui/nbrowserwindow.h>

class ExternalBrowse : public QMdiSubWindow
{
    Q_OBJECT
private:
    QShortcut *focusTitleShortcut;
    QShortcut *focusTagShortcut;
    QShortcut *focusNoteShortcut;
    QShortcut *focusUrlShortcut;
    QShortcut *focusAuthorShortcut;
    QShortcut *focusNotebookShortcut;
    QShortcut *focusFontShortcut;
    QShortcut *focusFontSizeShortcut;
    void setupShortcut(QShortcut *shortcut, QString keySequence);

public:
    explicit ExternalBrowse(qint32 lid, QWidget *parent = 0);
    NBrowserWindow *browser;
    
signals:
    
public slots:
    void closeEvent(QCloseEvent *closeEvent);
    void setTitle(QString text);
    
};

#endif // EXTERNALBROWSE_H

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


#ifndef NOTEHISTORYSELECT_H
#define NOTEHISTORYSELECT_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <evernote/NoteStore.h>
#include <evernote/UserStore.h>

using namespace std;
using namespace evernote::edam;

class NoteHistorySelect : public QDialog
{
    Q_OBJECT
private:
    QPushButton cancelButton;
    QPushButton importButton;
    vector<NoteVersionId> *versions;
    QListWidget list;

public:
    explicit NoteHistorySelect(QWidget *parent = 0);
    bool importPressed;
    qint32 usn;
    void loadData(vector<NoteVersionId> &versions);

signals:
    
public slots:
    void importButtonPressed();
    void cancelButtonPressed();
    void enableImport();
    
};

#endif // NOTEHISTORYSELECT_H

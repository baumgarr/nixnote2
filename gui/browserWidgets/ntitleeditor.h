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


#ifndef NTITLEEDITOR_H
#define NTITLEEDITOR_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "evernote/Limits_constants.h"

#include <QtWebKit>
#include <QLineEdit>


using namespace evernote::edam;
using namespace evernote::limits;

class NTitleEditor : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString initialTitle;
    QString priorTitle;
    qint32 currentLid;
    QString defaultTitle;
    QString cleanupTitle(QString text);

public:
    explicit NTitleEditor(QWidget *parent = 0);
    void setTitle(qint32 lid, QString title, QString defaultT);
    void setTitleFromContent(QString s);

public slots:
    void setActiveColor();
    void titleChanged(QString text);
    void checkNoteTitleChange();

signals:
  void focussed(bool hasFocus);
  void titleChanged();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
    
};

#endif // NTITLEEDITOR_H

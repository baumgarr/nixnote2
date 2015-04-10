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

#ifndef AUTHOREDITOR_H
#define AUTHOREDITOR_H

#include <QLineEdit>

class AuthorEditor : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString initialText;
    QString priorText;
    qint32 currentLid;

public:
    explicit AuthorEditor(QWidget *parent = 0);
    QString getText();

public slots:
    void setActiveColor();
    void textModified(QString text);
    void setAuthor(qint32 lid, QString text);

signals:
  void focussed(bool hasFocus);
  void textUpdated();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
};

#endif // AUTHOREDITOR_H

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

#ifndef TAGEDITORNEWTAG_H
#define TAGEDITORNEWTAG_H

#include <QLineEdit>
#include <QCompleter>
#include <QEvent>

class TagEditorNewTag : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QCompleter *completer;
    QStringList currentTags;
    qint32 account;

public:
    explicit TagEditorNewTag(QWidget *parent = 0);
    QString getText();
    void setTags(QStringList s);
    void resetText();
    void addTag(QString s);

public slots:
    void setActiveColor();
    void textModified(QString text);
//    void gainedFocus(bool focus);
    void notebookSelectionChanged(qint32 notebook);
    void loadCompleter();
    void mouseCompleterSelection(QString);
    void enterPressed();

signals:
  void focussed(bool hasFocus);
  void tabPressed();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
  bool event(QEvent *event);

};

#endif // TAGEDITORNEWTAG_H

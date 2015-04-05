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

#ifndef TAGEDITOR_H
#define TAGEDITOR_H

#include <QWidget>
#include <QLabel>
#include "tageditornewtag.h"
#include "tagviewer.h"
#include <QHBoxLayout>
#include "gui/flowlayout.h"

#define MAX_TAGS 100

class TagEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TagEditor(QWidget *parent = 0);
    void addTag(QString text);
    void setTags(QStringList names);
    void getTags(QStringList &names);
    void clear();
    void loadTags();
    void setCurrentLid(qint32 l);
    void setAccount(qint32 a);
    void tagRenamed(qint32 lid, QString oldName, QString newName);
    TagEditorNewTag newTag;
    void reloadIcons();

private:
    qint32 currentLid;
    qint32 account;
    QLabel tagIcon;
    TagViewer tags[MAX_TAGS];
    FlowLayout *layout;
    QStringList tagNames;
    void emptyTags();
    bool checkNewTagEditor();
    bool newEditorHasFocus;

signals:
    void buttonClicked(QString);
    void tagsUpdated();
    void newTagCreated(qint32 lid);
    
public slots:
    void removeTag(QString text);
    void newTagFocusLost(bool focus);
    void newTagTabPressed();
    void reloadTags();

protected:
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);


    
};

#endif // TAGEDITOR_H

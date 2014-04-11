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


#ifndef NOTECACHE_H
#define NOTECACHE_H

#include "qevercloud/include/QEverCloud.h"
#include <QObject>


using namespace qevercloud  ;


class NoteCache : public QObject
{
    Q_OBJECT
public:
    explicit NoteCache(QObject *parent = 0);
    QByteArray noteContent;
    bool isReadOnly;
    bool isContentReadOnly;
    bool isInkNote;

signals:

public slots:

};

#endif // NOTECACHE_H

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



#ifndef CROSSMEMORYMAPPER_H
#define CROSSMEMORYMAPPER_H

#include <QObject>
#include <QSharedMemory>

class CrossMemoryMapper : public QObject
{
    Q_OBJECT
private:
    QString key;
    QSharedMemory *sharedMemory;
    char *buffer;

public:
    explicit CrossMemoryMapper(QObject *parent = 0);
    explicit CrossMemoryMapper(QString key, QObject *parent = 0);
    ~CrossMemoryMapper();
    void setKey(QString key);
    bool allocate(int size);
    bool detach();
    bool attach();
    void clearMemory();
    bool unlock();
    bool lock();
    bool isAttached();
    QByteArray read();
    void write(QByteArray data);
    void write(QString data);

signals:

public slots:

};

#endif // CROSSMEMORYMAPPER_H

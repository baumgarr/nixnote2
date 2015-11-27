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


#include "crossmemorymapper.h"
#include <iostream>

CrossMemoryMapper::CrossMemoryMapper(QObject *parent) :
    QObject(parent)
{
}

CrossMemoryMapper::CrossMemoryMapper(QString key, QObject *parent) :
    QObject(parent)
{
    this->setKey(key);
    buffer = NULL;
}


void CrossMemoryMapper::setKey(QString key) {
    this->key = key;
    sharedMemory = new QSharedMemory(key, this);
}


CrossMemoryMapper::~CrossMemoryMapper() {
    if (buffer != NULL)
        free(buffer);
    if (key != "" && sharedMemory->isAttached())
         sharedMemory->detach();
}

bool CrossMemoryMapper::allocate(int size) {
    if (key == "")
        return false;
    if (!sharedMemory->create(size, QSharedMemory::ReadWrite)) {
        return false;
    }
    buffer = (char*)malloc(sharedMemory->size());
    return true;
}


void CrossMemoryMapper::clearMemory() {
    lock();
    memset(sharedMemory->data(), 0, sharedMemory->size());
    unlock();
}



bool CrossMemoryMapper::lock() {
    if (sharedMemory == NULL)
        return false;
    return sharedMemory->lock();
}


bool CrossMemoryMapper::unlock() {
    if (sharedMemory == NULL)
        return false;
    return sharedMemory->unlock();
}


bool CrossMemoryMapper::attach() {
    if (key == "")
        return false;
    if (sharedMemory == NULL)
        return false;
    if (buffer == NULL)
        buffer = (char*)malloc(sharedMemory->size());
    return sharedMemory->attach();
}


bool CrossMemoryMapper::detach() {
    sharedMemory->unlock();
    return sharedMemory->detach();
}


QByteArray CrossMemoryMapper::read() {
    if (!isAttached())
        attach();
    if (sharedMemory == NULL || !sharedMemory->isAttached() || buffer == NULL)
        return QByteArray();

    if (buffer != NULL)
        free(buffer);
    buffer = (char*)malloc(sharedMemory->size());

    memcpy(buffer, sharedMemory->data(), sharedMemory->size());
    this->unlock();
    clearMemory();
    QByteArray data = QByteArray(QByteArray::fromRawData(buffer, sharedMemory->size()));
    return data;
}

void CrossMemoryMapper::write(QString value) {
    write(value.toAscii());
}

void CrossMemoryMapper::write(QByteArray data) {
    QString svalue(data);
    this->lock();
    void *memptr = sharedMemory->data();
    memcpy(memptr, svalue.toStdString().c_str(), data.size());
    this->unlock();
}


bool CrossMemoryMapper::isAttached() {
    if (sharedMemory == NULL)
        return false;
    return sharedMemory->isAttached();
}

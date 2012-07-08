#include "notecache.h"



NoteCache::NoteCache(QObject *parent) :
    QObject(parent)
{
    noteContent = "";
    isReadOnly = false;
    isContentReadOnly = false;
}

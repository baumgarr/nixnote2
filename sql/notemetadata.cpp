#include "notemetadata.h"

NoteMetaData::NoteMetaData()
{
    color = -1;
    pinned = false;
    dirty = false;
}


QString NoteMetaData::getGuid() {
    return guid;
}


void NoteMetaData::setGuid(QString g) {
    guid = g;
}

int NoteMetaData::getColor() {
    return color;
}
void NoteMetaData::setColor(int c) {
    color = c;
}
void NoteMetaData::setPinned(bool value) {
    pinned = value;
}
bool NoteMetaData::isPinned() {
    return pinned;
}
void NoteMetaData::setDirty(bool v) {
    dirty = v;
}
bool NoteMetaData::isDirty() {
    return dirty;
}
void NoteMetaData::copy(NoteMetaData m) {
    color = m.getColor();
    guid = m.getGuid();
    pinned = m.isPinned();
    dirty = m.isDirty();
}

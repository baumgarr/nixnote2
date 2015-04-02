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

#include "notemetadata.h"



// Constructor
NoteMetaData::NoteMetaData()
{
    color = -1;
    pinned = false;
    dirty = false;
}



// Get the GUID for the note
QString NoteMetaData::getGuid() {
    return guid;
}



// Set the GUID for the note
void NoteMetaData::setGuid(QString g) {
    guid = g;
}


// Get the title color for the note
int NoteMetaData::getColor() {
    return color;
}


// Set the title color for the note
void NoteMetaData::setColor(int c) {
    color = c;
}


// Set this note as pinned
void NoteMetaData::setPinned(bool value) {
    pinned = value;
}


// Is this note pinned?
bool NoteMetaData::isPinned() {
    return pinned;
}


// Set this note as needing to be synced
void NoteMetaData::setDirty(bool v) {
    dirty = v;
}



// Does this note need to be synchornized
bool NoteMetaData::isDirty() {
    return dirty;
}



// Copy this metadata
void NoteMetaData::copy(NoteMetaData m) {
    color = m.getColor();
    guid = m.getGuid();
    pinned = m.isPinned();
    dirty = m.isDirty();
}

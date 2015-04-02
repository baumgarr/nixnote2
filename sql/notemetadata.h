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

#ifndef NOTEMETADATA_H
#define NOTEMETADATA_H

#include <QString>

class NoteMetaData;

class NoteMetaData
{
public:
    NoteMetaData();             // Constructor
    QString getGuid();          // Get the GUID for this record
    void setGuid(QString g);    // Set the GUID for this record

    int getColor();             // Get the title color for a note
    void setColor(int c);       // Set the title color for a note
    void setPinned(bool value); // Set this note pinned?
    bool isPinned();            // Is this note pinned?
    void setDirty(bool v);      // Set this note as needing a sync
    bool isDirty();             // Does this note need a sync?
    void copy(NoteMetaData m);  // Copy this record

private:
    QString guid;                // Note GUID
    int color;                   // Note color
    bool pinned;                 // Is it pinned?
    bool dirty;                  // Does it need a sync
};

#endif // NOTEMETADATA_H

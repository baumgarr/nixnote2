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


//****************************************************
//*  This is i generic utility to dump Evernote
//*  structures to the debug log.  It is pretty
//*  basic and just dumps whatever it is given.
//*  If a value is not set, it says that it is not
//*   set.
//*****************************************************

#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H

#include "qevercloud/include/QEverCloud.h"

using namespace qevercloud;

class DebugTool
{
private:
    // These are generic dumpField functions called by the other methods.
    void dumpField(Optional<QString> field, QString name);
    void dumpField(Optional<qint32> field, QString name);
    void dumpField(Optional<bool> field, QString name);
    void dumpField(Optional<short int> field, QString name);
    void dumpField(Optional<long long> field, QString name);
    void dumpField(Optional<QByteArray> field, QString name, bool hexValue=false);
    void dumpField(Optional<QStringList> field, QString name);
    void dumpField(Optional< QList< QString> > field, QString name);
    void dumpField(Optional<double> field, QString name);

public:
    DebugTool();

    void dumpTag(Tag tag);                         // Dump a tag to the log
    void dumpNote(Note note);                      // Dump a note to the log
    void dumpResource(Resource resource);          // Dump a resource to the log
    void dumpData(Data data);                      // Dump a data segment (from a resource) to the log
    void dumpSavedSearch(SavedSearch search);      // Dump a saved search to the log
    void dumpNotebook(Notebook notebook);          // Dump a notebeook to the log
};

#endif // DEBUGTOOL_H

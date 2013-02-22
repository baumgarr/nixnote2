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

#include "tagscanner.h"
#include "global.h"

TagScannerRecord::TagScannerRecord(QObject *parent) {
    parent=parent;  // suppress unused
    this->startPosition = -1;
    this->endPosition = -1;
}



TagScanner::TagScanner(QObject *parent) :
    QObject(parent)
{
    content = "";
    recTable.clear();
}

void TagScanner::setData(QString data) {
    content = data;
    qint32 position = content.indexOf(QString("<"), Qt::CaseInsensitive);

    // Scan through & find all of the tags in a note
    while (position >=0) {
        TagScannerRecord newRec;

        newRec.startPosition = position;   // Position of the <
        newRec.endPosition = content.indexOf(QString(">"), position);   // Position of the next >
        qint32 space = content.indexOf(" ", position);  // If there is a space, then we remove any trailing values

        // Check for an invaild end position
        if (space > newRec.endPosition || space == -1)
            space = newRec.endPosition;

        // Get the tag's name
        QString tagName = content.mid(position+1, space-position-1);

        // Insert the tag qint32o the table along with its record position
        recTable.insert(tagName, newRec);

        // Start looking for the next tag
        position = content.indexOf(QString("<"),newRec.endPosition, Qt::CaseInsensitive);
    }
}

qint32 TagScanner::findAll(QList<TagScannerRecord> &recs, QString tagName) {
    recs.empty();
    qint32 position = content.indexOf(QString("<")+tagName, Qt::CaseInsensitive);
    if (position > 0)
        return 0;

    while (position >=0) {
        TagScannerRecord newRec;
        newRec.startPosition = position;
        newRec.endPosition = content.indexOf(QString(">"), position);
        recs.append(newRec);
        position = content.indexOf(QString("<")+tagName, Qt::CaseInsensitive);
        QLOG_DEBUG() << "Found: " << content.mid(newRec.startPosition, newRec.endPosition);
    }
    return recs.size();
}

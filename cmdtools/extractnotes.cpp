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


#include "extractnotes.h"
#include "xml/exportdata.h"
#include "global.h"
#include "filters/filtercriteria.h"
#include "filters/filterengine.h"
#include "sql/notetable.h"

extern Global global;

ExtractNotes::ExtractNotes(QObject *parent) :
    QObject(parent)
{
    this->deleteAfterExtract=false;
    this->verifyDelete=true;
}



void ExtractNotes::extract() {
    if (outputFile.trimmed() == "") {
        std::cout << QString(tr("Output file not specified.")).toStdString() << endl;
        return;
    }
    if (query != "") {
        FilterCriteria *filter = new FilterCriteria();
        global.filterCriteria.append(filter);
        global.filterPosition = 0;
        FilterEngine engine;
        filter->setSearchString(query);
        QList<qint32> lids;
        engine.filter(filter, &lids);
        this->lids.append(lids);
    }

    ExportData exports(backup, true, this);
    if (!backup) {
        exports.lids.append(this->lids);
    }
    exports.backupData(this->outputFile);

    if (deleteAfterExtract) {
        if (verifyDelete) {
            std::string verify;
            std::cout << QString(tr("Deleting notes:")).toStdString() << endl;
            std::cout << QString(tr("Type DELETE to very: ")).toStdString();
            std::cin >> verify;
            QString qVerify = QString::fromStdString(verify);
            if (qVerify.toLower() != "delete")
                return;
        }
        NoteTable ntable(global.db);
        for (int i=0; i<lids.size(); i++) {
            ntable.deleteNote(lids[i],true);
        }
    }

}


void ExtractNotes::backupDB() {
    if (outputFile.trimmed() == "") {
        std::cout << QString(tr("Output file not specified.")).toStdString() << endl;
        return;
    }
    ExportData exports(backup, true, this);
    exports.backupData(this->outputFile);
}

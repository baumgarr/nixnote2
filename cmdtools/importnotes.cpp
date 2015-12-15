#include "importnotes.h"
#include <iostream>
#include <QFile>
#include "xml/importdata.h"

ImportNotes::ImportNotes(QObject *parent) :
    QObject(parent)
{
    inputFile = "";
}




// Perform the import via the command line
int ImportNotes::import() {

    if (inputFile.trimmed() == "") {
        std::cout << QString(tr("Input file not specified.")).toStdString() << std::endl;
        return 16;
    }
    QFile f(inputFile);
    if (!f.exists()) {
        std::cout << QString(tr("Input file not found.")).toStdString() << std::endl;
        return 16;
    }

    ImportData importData(false,true,0);
    importData.import(inputFile);
    if (importData.lastError != 0) {
        std::cout << importData.errorMessage.toStdString() << std::endl;
        return 16;
    }
    return 0;
}

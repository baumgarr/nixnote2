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

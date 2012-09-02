#ifndef TAGSCANNER_H
#define TAGSCANNER_H

#include <QObject>
#include <QHash>
#include <stdint.h>


class TagScannerRecord {

public:
    TagScannerRecord(QObject *parent = 0);
    qint32 startPosition;
    qint32 endPosition;
};







class TagScanner : public QObject
{
    Q_OBJECT
private:
    QString content;
    QHash<QString, TagScannerRecord> recTable;

public:
    explicit TagScanner(QObject *parent = 0);
    void setData(QString data);
    qint32 findAll(QList<TagScannerRecord> &recs, QString tagName);

signals:

public slots:

};







#endif // TAGSCANNER_H

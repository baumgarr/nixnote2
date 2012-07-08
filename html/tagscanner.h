#ifndef TAGSCANNER_H
#define TAGSCANNER_H

#include <QObject>
#include <QHash>



class TagScannerRecord {

public:
    TagScannerRecord(QObject *parent = 0);
    int startPosition;
    int endPosition;
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
    int findAll(QList<TagScannerRecord> &recs, QString tagName);

signals:

public slots:

};







#endif // TAGSCANNER_H

#ifndef BATCHIMPORT_H
#define BATCHIMPORT_H

#include <QObject>
#include <QXmlStreamReader>

class BatchImport : public QObject
{
    Q_OBJECT
private:
    QString fileName;
    int lastError;
    QString errorMessage;
    QXmlStreamReader *reader;
    QString textValue();
    bool booleanValue();
    long longValue();
    qlonglong longlongValue();
    double doubleValue();
    short shortValue();
    int intValue();


public:
    explicit BatchImport(QObject *parent = 0);
    void import(QString file);
    void addNoteNode();

signals:

public slots:

};

#endif // BATCHIMPORT_H

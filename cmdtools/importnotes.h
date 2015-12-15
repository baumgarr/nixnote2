#ifndef IMPORTNOTES_H
#define IMPORTNOTES_H

#include <QObject>

class ImportNotes : public QObject
{
    Q_OBJECT
public:
    explicit ImportNotes(QObject *parent = 0);
    QString inputFile;
    int import();

signals:

public slots:

};

#endif // IMPORTNOTES_H

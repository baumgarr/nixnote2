#ifndef INKNOTEREQUEST_H
#define INKNOTEREQUEST_H

#include <QObject>
#include <QImage>

class InkNoteRequest : public QObject
{
    Q_OBJECT
public:
    explicit InkNoteRequest(QObject *parent = 0);
    int totalSlices;
    int currentSlice;
    bool isReady;
    qint32 lid;
    QString guid;
    QImage img;
    
signals:
    
public slots:
    
};

#endif // INKNOTEREQUEST_H

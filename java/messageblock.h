#ifndef MESSAGEBLOCK_H
#define MESSAGEBLOCK_H

#include <QObject>

class MessageBlock : public QObject
{
    Q_OBJECT
public:
    explicit MessageBlock(QObject *parent = 0);
    bool error;
    QString errorMessage;

signals:

public slots:

};

#endif // MESSAGEBLOCK_H

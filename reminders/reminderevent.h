#ifndef REMINDEREVENT_H
#define REMINDEREVENT_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

class ReminderEvent : public QObject
{
    Q_OBJECT
public:
    explicit ReminderEvent(QObject *parent = 0);
    qint32 lid;
    QTimer timer;
    qlonglong time;
    bool setTimer(qint32 lid, QDateTime time);
    bool setTimer(qint32 lid, qlonglong time);
    void stopTimer();
    
signals:
    
public slots:
    
};

#endif // REMINDEREVENT_H

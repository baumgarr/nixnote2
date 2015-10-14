#ifndef EVENTLOOPFINISHER_H
#define EVENTLOOPFINISHER_H

#include <QObject>
#include <QEventLoop>

/** @cond HIDDEN_SYMBOLS  */

namespace qevercloud {

class EventLoopFinisher : public QObject
{
    Q_OBJECT
public:
    explicit EventLoopFinisher(QEventLoop* loop, int exitCode, QObject *parent = 0);

public slots:
    void stopEventLoop();
private:
    QEventLoop* loop_;
    int exitCode_;
};

/** @endcond */
}
#endif // EVENTLOOPFINISHER_H

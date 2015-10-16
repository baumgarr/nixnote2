#include "EventLoopFinisher.h"

/** @cond HIDDEN_SYMBOLS  */

qevercloud::EventLoopFinisher::EventLoopFinisher(QEventLoop* loop, int exitCode, QObject *parent) :
    QObject(parent), loop_(loop), exitCode_(exitCode)
{
}

void qevercloud::EventLoopFinisher::stopEventLoop()
{
    loop_->exit(exitCode_);
}

/** @endcond */

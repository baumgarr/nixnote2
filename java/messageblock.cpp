#include "messageblock.h"

MessageBlock::MessageBlock(QObject *parent) :
    QObject(parent)
{
    error = false;
    errorMessage = "";
}

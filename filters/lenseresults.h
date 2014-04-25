#ifndef LENSERESULTS_H
#define LENSERESULTS_H

#include <QObject>
#include "qevercloud/include/QEverCloud.h"

class LenseResults : public QObject
{
    Q_OBJECT
public:
    explicit LenseResults(QObject *parent = 0);
    Note note;

signals:

public slots:

};

#endif // LENSERESULTS_H

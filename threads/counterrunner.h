#ifndef COUNTERRUNNER_H
#define COUNTERRUNNER_H

#include <QThread>
#include "global.h"
#include <QPair>
#include <QList>

extern Global global;

class CounterRunner : public QThread
{
    Q_OBJECT
private:
    QList<QPair<qint32, qint32>*> *notebookCounts;
    QList<QPair<qint32, qint32>*> *tagCounts;
    qint32 trashCounts;

public:
    explicit CounterRunner(QObject *parent = 0);
    bool keepRunning;
    void run();
    
signals:
    void trashTotals(qint32);
    void notebookTotals(qint32, qint32);
    void tagTotals(qint32, qint32);
    
public slots:
    void countAll();
    void countTrash();
    void countNotebooks();
    void countTags();
    
};

#endif // COUNTERTHREAD_H

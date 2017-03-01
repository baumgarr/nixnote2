#ifndef BROWSERRUNNER_H
#define BROWSERRUNNER_H

#include <QObject>
#include "sql/databaseconnection.h"

class BrowserRunner : public QObject
{
    Q_OBJECT
private:
    DatabaseConnection *db;
    bool init;
    void initialize();

public:
    explicit BrowserRunner(QObject *parent = 0);

    bool keepRunning;
    bool isIdle;

signals:

public slots:
    void updateNoteContent(qint32 lid, QString content, bool isDirty);
};

#endif // BROWSERRUNNER_H

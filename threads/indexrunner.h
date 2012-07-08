#ifndef INDEXRUNNER_H
#define INDEXRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>


#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

#include <html/thumbnailer.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <QTimer>

using namespace evernote::edam;
using namespace std;

class IndexRunner : public QThread
{
    Q_OBJECT
private:
    Thumbnailer *hammer;
    QTimer *indexTimer;
    void indexRecognition(int lid, Resource &r);
    void indexNote(int lid, Note &n);
    QTextDocument *textDocument;

public:
    bool keepRunning;
    bool pauseIndexing;
    IndexRunner();
    ~IndexRunner();
    void run();

signals:

 public slots:
    void index();
};

#endif // INDEXRUNNER_H

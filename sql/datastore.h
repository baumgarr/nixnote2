#ifndef DATASTORETABLE_H
#define DATASTORETABLE_H

#include "global.h"
#include <QSqlTableModel>
#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include <QList>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace evernote::edam;
using namespace std;


using namespace evernote::edam;

class DataStore : public QSqlTableModel
{
    Q_OBJECT
private:
    void createTable();

public:
    explicit DataStore(QObject *parent = 0);
    int getDirtyNoteGuids(QList<QString> &retVal);

signals:

public slots:

};


#endif // DATASTORETABLE_H

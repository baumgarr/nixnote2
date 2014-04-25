#ifndef REMOTEQUERY_H
#define REMOTEQUERY_H

#include <QObject>
#include <QtDBus/QtDBus>
#include <QList>
#include <QString>
#include "sql/notetable.h"


#define DBUS_SERVICE_NAME "com.canonical.Unity.Scope.Notes.NixNote"

class RemoteQuery : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Scope.Notes.NixNote.RemoteQuery")
private:
    Note *note;
    qint32 lid;

public:
    explicit RemoteQuery(QObject *parent = 0);
    void initDbus();

signals:

public slots:
    Q_SCRIPTABLE QString query(const QString &arg);
    Q_SCRIPTABLE QString getNoteTitle();
    Q_SCRIPTABLE bool setNote(qint32 lid);
    Q_SCRIPTABLE QString getNotePreview();
    Q_SCRIPTABLE QString getNoteDateUpdated();
    Q_SCRIPTABLE QString getNoteDateCreated();
    Q_SCRIPTABLE QString getNoteTags();
};


#endif // REMOTEQUERY_H

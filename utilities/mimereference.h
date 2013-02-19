#ifndef MIMEREFERENCE_H
#define MIMEREFERENCE_H

#include <QObject>
#include <QHash>
#include <string>

using namespace std;

class MimeReference : public QObject
{
    Q_OBJECT
public:
    explicit MimeReference(QObject *parent = 0);
    QHash<QString,QString> *table;
    QString getExtensionFromMime(QString mime, QString filename);
    QString getMimeFromExtension(QString fileExtension);
    QString getMimeFromExtension(string fileExtension);
    QString getMimeFromFileName(QString fileName);
    QString getMimeFromFileName(string filename);
    QString getExtensionFromMime(string extension, string filename);
    
signals:
    
public slots:
    
};

#endif // MIMEREFERENCE_H

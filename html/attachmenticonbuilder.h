#ifndef ATTACHMENTICONBUILDER_H
#define ATTACHMENTICONBUILDER_H

#include <QObject>

class AttachmentIconBuilder : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentIconBuilder(QObject *parent = 0);
    QString buildIcon(qint32 lid, QString fileName);
    
signals:
    
public slots:
    
};

#endif // ATTACHMENTICONBUILDER_H

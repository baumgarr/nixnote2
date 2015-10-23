#ifndef DATABASEUPGRADE_H
#define DATABASEUPGRADE_H

#include <QObject>

class DatabaseUpgrade : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseUpgrade(QObject *parent = 0);
    void fixSql(bool toQt5=true);

signals:

public slots:

};

#endif // DATABASEUPGRADE_H

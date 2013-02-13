#ifndef DATABASESTATUS_H
#define DATABASESTATUS_H

#include <QDialog>

class DatabaseStatus : public QDialog
{
    Q_OBJECT
public:
    explicit DatabaseStatus(QWidget *parent = 0);
    QPushButton *ok;
    
signals:
    
public slots:
    void okPushed();
    
};

#endif // DATABASESTATUS_H

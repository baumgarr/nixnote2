#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>

class AccountDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AccountDialog(QWidget *parent = 0);
    QPushButton *ok;
    
signals:
    
public slots:
    void okPushed();
    
};

#endif // ACCOUNTDIALOG_H

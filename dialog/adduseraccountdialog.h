#ifndef ADDUSERACCOUNTDIALOG_H
#define ADDUSERACCOUNTDIALOG_H

#include <QDialog>
#include <QList>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

class AddUserAccountDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddUserAccountDialog(QWidget *parent = 0);
    ~AddUserAccountDialog();
    QPushButton *okButton;
    QPushButton *cancelButton;
    bool okPushed;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *labelLayout;
    QLabel *newAccountLabel;
    QLineEdit *newAccountName;
    QStringList accountNames;
    
signals:
    
public slots:
    void okClicked();
    void cancelClicked();
    void textChanged(QString text);
    
};

#endif // ADDUSERACCOUNTDIALOG_H

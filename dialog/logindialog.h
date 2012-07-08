#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class LoginDialog : public QDialog
{
    Q_OBJECT
private:

    QPushButton ok;
    QLabel useridLabel;
    QLabel passwordLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout passwordGrid;
    QGridLayout buttonGrid;




public:
    explicit LoginDialog(QWidget *parent = 0);
    QLineEdit	userid;
    QLineEdit	password;
    bool okPressed;


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

};

#endif // LOGINDIALOG_H


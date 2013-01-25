#ifndef ENCRYPTDIALOG_H
#define ENCRYPTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class EnCryptDialog : public QDialog
{
    Q_OBJECT
private:
    bool wasOkPressed;
    QLineEdit	password;
    QLineEdit password2;
    QLineEdit hint;
    QPushButton ok;
    QLabel error;

public:
    explicit EnCryptDialog(QWidget *parent = 0);
    QString getPasswordVerify();
    QString getPassword();
    QString getHint();
    bool okPressed();
    
signals:
    
public slots:

private slots:
    void validateInput();
    void okButtonPressed();
    void cancelButtonPressed();
    
};

#endif // ENCRYPTDIALOG_H

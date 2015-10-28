#ifndef EMAILDIALOG_H
#define EMAILDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>

class EmailDialog : public QDialog
{
    Q_OBJECT
private:
    QLabel          *toLabel;
    QLabel          *bccLabel;
    QLabel          *ccLabel;
    QLabel          *subjectLabel;
    QLabel          *noteLabel;
    QStringList     tokenizeString(QString);

public:
    explicit EmailDialog(QWidget *parent = 0);
    bool sendPressed;
    bool cancelPressed;

    QCheckBox       *ccSelf;
    QLineEdit       *toAddress;
    QLineEdit       *bccAddress;
    QLineEdit       *ccAddress;
    QLineEdit       *subject;

    QPushButton     *sendButton;
    QPushButton     *cancelButton;
    QPlainTextEdit  *note;
    QStringList     getToAddresses();
    QStringList     getCcAddresses();
    QStringList     getBccAddresses();

signals:

public slots:
    void cancelButtonPressed();
    void sendButtonPressed();
    void toAddressChanged();

};

#endif // EMAILDIALOG_H

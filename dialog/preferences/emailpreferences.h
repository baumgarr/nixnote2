#ifndef EMAILPREFERENCES_H
#define EMAILPREFERENCES_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>

class EmailPreferences : public QWidget
{
    Q_OBJECT
private:
    QLabel smtpServerLabel;
    QLabel smtpPortLabel;
    QLabel smtpConnectionTypeLabel;
    QLabel useridLabel;
    QLabel passwordLabel;
    QLabel senderEmailLabel;
    QLabel senderNameLabel;

    QLineEdit smtpServer;
    QLineEdit smtpPort;
    QComboBox smtpConnectionType;
    QLineEdit userid;
    QLineEdit password;
    QLineEdit senderEmail;
    QLineEdit senderName;
    QPushButton *testEmail;

public:
    explicit EmailPreferences(QWidget *parent = 0);
    void saveValues();
signals:

public slots:
    void sendTestEmail();

};

#endif // EMAILPREFERENCES_H

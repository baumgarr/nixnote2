#include "emailpreferences.h"
#include "global.h"

extern Global global;

EmailPreferences::EmailPreferences(QWidget *parent) :
    QWidget(parent)
{

    smtpServerLabel.setText(tr("SMTP Server"));
    smtpPortLabel.setText(tr("Server Port"));
    useridLabel.setText(tr("Userid"));
    passwordLabel.setText(tr("Password"));
    senderNameLabel.setText(tr("Sender Display Name"));
    senderEmailLabel.setText(tr("Sender Email"));
    smtpConnectionTypeLabel.setText(tr("Connection Type"));

    password.setEchoMode(QLineEdit::Password);
    smtpPort.setInputMask("99999");

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    smtpConnectionType.addItem(tr("Plain Text"), "TcpConnection");
    smtpConnectionType.addItem(tr("SSL Connection"), "SslConnection");
    smtpConnectionType.addItem(tr("TLS Connection"), "TlsConnection");

    int row = 0;
    mainLayout->addWidget(&smtpServerLabel, row, 0);
    mainLayout->addWidget(&smtpServer, row++, 1);
    mainLayout->addWidget(&smtpPortLabel, row, 0);
    mainLayout->addWidget(&smtpPort, row++, 1);
    mainLayout->addWidget(&smtpConnectionTypeLabel, row, 0);
    mainLayout->addWidget(&smtpConnectionType, row++, 1);
    mainLayout->addWidget(&useridLabel, row, 0);
    mainLayout->addWidget(&userid, row++, 1);
    mainLayout->addWidget(&passwordLabel, row, 0);
    mainLayout->addWidget(&password, row++, 1);
    mainLayout->addWidget(&senderEmailLabel, row, 0);
    mainLayout->addWidget(&senderEmail, row++, 1);
    mainLayout->addWidget(&senderNameLabel, row, 0);
    mainLayout->addWidget(&senderName, row++, 1);

    // Get the smtp server settings
    global.settings->beginGroup("Email");
    QString s = global.settings->value("smtpServer", "").toString();
    QString p = global.settings->value("smtpPort", "25").toString();
    QString ctype = global.settings->value("smtpConnectionType", "TcpConnection").toString();
    QString u = global.settings->value("userid", "").toString();
    QString pswd = global.settings->value("password", "").toString();
    QString senderE = global.settings->value("senderEmail", "").toString();
    QString senderN = global.settings->value("senderName", "").toString();
    global.settings->endGroup();

    int index = smtpConnectionType.findData(ctype);
    smtpConnectionType.setCurrentIndex(index);

    smtpServer.setText(s);
    smtpPort.setText(p);
    userid.setText(u);
    password.setText(pswd);
    senderEmail.setText(senderE);
    senderName.setText(senderN);
}



void EmailPreferences::saveValues() {
    int index = smtpConnectionType.currentIndex();
    QString ctype = smtpConnectionType.itemData(index).toString();


    global.settings->beginGroup("Email");
    global.settings->setValue("smtpServer", smtpServer.text().trimmed());
    global.settings->setValue("smtpPort", smtpPort.text().trimmed());
    global.settings->setValue("smtpConnectionType", ctype);
    global.settings->setValue("userid", userid.text().trimmed());
    global.settings->setValue("password", password.text().trimmed());
    global.settings->setValue("senderEmail", senderEmail.text().trimmed());
    global.settings->setValue("senderName", senderName.text().trimmed());
    global.settings->endGroup();
}

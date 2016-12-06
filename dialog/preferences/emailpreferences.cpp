#include "global.h"

#include <QMessageBox>

#include "dialog/emaildialog.h"
#include "email/smtpclient.h"
#include "email/mimeinlinefile.h"
#include "email/mimehtml.h"
#include "emailpreferences.h"

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

    testEmail = new QPushButton(tr("Send Test Email"));
    connect(testEmail, SIGNAL(pressed()), this, SLOT(sendTestEmail()));

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
    mainLayout->addWidget(testEmail, row++,0);

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



void EmailPreferences::sendTestEmail() {
    QString server = this->smtpServer.text().trimmed();
    int port = this->smtpPort.text().trimmed().toInt();
    int index = this->smtpConnectionType.currentIndex();
    QString smtpConnectionType = this->smtpConnectionType.itemData(index).toString();
    QString userid = this->userid.text().trimmed();
    QString password = this->password.text().trimmed();
    QString senderEmail = this->senderEmail.text().trimmed();
    QString senderName = this->senderName.text().trimmed();

    if (server == "") {
        QMessageBox::critical(this, tr("Setup Error"),
             tr("You must specify a SMTP server."), QMessageBox::Ok);
        return;
    }
    if (senderEmail == "") {
        QMessageBox::critical(this, tr("Setup Error"),
             tr("You must specify a sender email."), QMessageBox::Ok);
        return;
    }


    SmtpClient::ConnectionType type = SmtpClient::TcpConnection;
    if (smtpConnectionType == "SslConnection")
        type = SmtpClient::SslConnection;
    if (smtpConnectionType == "TlsConnection")
        type = SmtpClient::TlsConnection;

    SmtpClient smtp(server, port, type);
    smtp.setResponseTimeout(-1);

    // We need to set the username (your email address) and password
    // for smtp authentication.
    smtp.setUser(userid);
    smtp.setPassword(password);

    // Now we create a MimeMessage object. This is the email.
    MimeMessage message;

    EmailAddress sender(senderEmail, senderName);
    message.setSender(&sender);

    if (senderName == "")
        senderName = senderEmail;
    EmailAddress *to = new EmailAddress(senderEmail, senderName);
    message.addRecipient(to);


    // Set the subject
    message.setSubject(tr("Test Email From NixNote"));

    // Set the note text
    MimeHtml html;
    html.setHtml(QString(tr("<h1>This is a test email from NixNote.</h1> "))+
                 QString(tr("If you are reading it then your email preferences are are setup properly.<br><img src='cid:image1'/>")));
    MimeInlineFile image1(new QFile(global.fileManager.getProgramDirPath("")+"splash_logo.png"));
    image1.setContentType("image/png");
    image1.setContentId("image1");
    message.addPart(&html);
    message.addPart(&image1);


    // Send the actual message.
    if (!smtp.connectToHost()) {
        QLOG_ERROR()<< "Failed to connect to host!";
        QMessageBox::critical(this, tr("Connection Error"), tr("Unable to connect to host."), QMessageBox::Ok);
        return;
    }

    if (!smtp.login()) {
        QLOG_ERROR() << "Failed to login!";
        QMessageBox::critical(this, tr("Login Error"), tr("Unable to login."), QMessageBox::Ok);
        return;
    }

    if (!smtp.sendMail(message)) {
        QMessageBox::critical(this, tr("Send Error"), tr("Unable to send email."), QMessageBox::Ok);
        QLOG_ERROR() << "Failed to send mail!";
        return;
    }

    smtp.quit();
      QMessageBox::information(this, tr("Message Sent"), tr("Message sent."), QMessageBox::Ok);
}

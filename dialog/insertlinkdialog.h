#ifndef INSERTLINKDIALOG_H
#define INSERTLINKDIALOG_H

#include <QDialog>
#include <QLineEdit>

class InsertLinkDialog : public QDialog
{
    Q_OBJECT
private:
    bool okPressed;
    QLineEdit *url;
    QPushButton *ok;
    QString urlText;
    bool insertHyperlink;

public:
    explicit InsertLinkDialog(bool insert = true, QWidget *parent = 0);
    void accept();
    void reject();
    QString getUrl();
    void setUrl(QString u);

signals:
    
public slots:
    void validateInput();
    bool okButtonPressed();
    
};

#endif // INSERTLINKDIALOG_H

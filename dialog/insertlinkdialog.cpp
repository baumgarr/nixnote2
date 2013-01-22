#include "insertlinkdialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

InsertLinkDialog::InsertLinkDialog(bool insert, QWidget *parent) :
    QDialog(parent)
{
    okPressed = false;
    setWindowTitle(tr("Insert Link"));
    //setWindowIcon(new QIcon(iconPath+"link.png"));
    QGridLayout *grid = new QGridLayout();
    QGridLayout *input = new QGridLayout();
    QGridLayout *button = new QGridLayout();
    setLayout(grid);
    insertHyperlink = insert;


    url = new QLineEdit("");

    input->addWidget(new QLabel(tr("Url")), 1,1);
    input->addWidget(url, 1, 2);
    input->setContentsMargins(10, 10,  -10, -10);
    grid->addLayout(input, 1,1);

    ok = new QPushButton(tr("OK"));
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    ok->setEnabled(false);

    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    button->addWidget(ok, 1, 1);
    button->addWidget(cancel, 1,2);
    grid->addLayout(button, 3, 1);
    connect(url,SIGNAL(textChanged(QString)), this, SLOT(validateInput()));

   // setAttribute(Qt::WA_DeleteOnClose);

}

// Get the password
QString InsertLinkDialog::getUrl() {
    if (urlText.indexOf("://") > 0)
        return urlText;
    if (urlText.trimmed() != "")
        return "http://"+urlText;
    return urlText;
}


// Set the url
void InsertLinkDialog::setUrl(QString u) {
    url->setText(u);
}



// Check if the OK button was pressed
bool InsertLinkDialog::okButtonPressed() {
    return okPressed;
}



// Check that we have a valid URL
void InsertLinkDialog::validateInput() {
    ok->setEnabled(true);
    if (url->text().trimmed() == "" && insertHyperlink)
        ok->setEnabled(false);
}

void InsertLinkDialog::accept() {
    if (ok->isEnabled()) {
        okPressed = true;
        urlText = url->text();
        QDialog::accept();
    }
}

void InsertLinkDialog::reject() {
    okPressed=false;
    QDialog::reject();
}

#include "adduseraccountdialog.h"
#include "global.h"
#include <QSpacerItem>

extern Global global;

AddUserAccountDialog::AddUserAccountDialog(QWidget *parent) :
    QDialog(parent)
{
    okPushed = false;
    okButton = new QPushButton(this);
    cancelButton = new QPushButton(this);
    buttonLayout = new QHBoxLayout();
    mainLayout = new QVBoxLayout();
    labelLayout = new QHBoxLayout();
    okButton->setText(tr("Ok"));
    okButton->setEnabled(false);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    setLayout(mainLayout);

    newAccountLabel = new QLabel(tr("Account Name"));
    newAccountName = new QLineEdit();
    connect(newAccountName, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    labelLayout->addWidget(newAccountLabel);
    labelLayout->addWidget(newAccountName);

    QSpacerItem *spacer1 = new QSpacerItem(10,10,QSizePolicy::Maximum, QSizePolicy::Minimum);
    QSpacerItem *spacer2= new QSpacerItem(10,10,QSizePolicy::Maximum, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->setStretch(0,100);
    buttonLayout->setStretch(3,100);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(spacer2);

    mainLayout->addLayout(labelLayout);
    mainLayout->addLayout(buttonLayout);

    accountNames = global.accountsManager->nameList();

}



AddUserAccountDialog::~AddUserAccountDialog() {
    delete okButton;
    delete cancelButton;
    delete buttonLayout;
    delete mainLayout;
}


void AddUserAccountDialog::cancelClicked() {
    okPushed = false;
    close();
}


void AddUserAccountDialog::okClicked() {
    okPushed = true;
    close();
}


void AddUserAccountDialog::textChanged(QString text) {
    if (text.trimmed() == "")  {
        okButton->setEnabled(false);
        return;
    }
    if (accountNames.contains(text, Qt::CaseInsensitive)) {
        okButton->setEnabled(false);
        return;
    }
    okButton->setEnabled(true);
}

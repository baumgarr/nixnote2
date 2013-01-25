#include "encryptdialog.h"

#include <QGridLayout>

EnCryptDialog::EnCryptDialog(QWidget *parent) :
    QDialog(parent)
{

    wasOkPressed = false;
    setWindowTitle(tr("Encrypt Text"));
    //setWindowIcon(new QIcon(iconPath+"password.png"));
    QGridLayout *grid = new QGridLayout(this);
    QGridLayout *input = new QGridLayout(this);
    QGridLayout *msgGrid = new QGridLayout(this);
    QGridLayout *button = new QGridLayout(this);
    setLayout(grid);


    hint.setText("");
    password.setText("");
    password.setEchoMode(QLineEdit::Password);
    password2.setText("");
    password2.setEchoMode(QLineEdit::Password);


    input->addWidget(new QLabel(tr("Password"), this), 1,1);
    input->addWidget(&password, 1, 2);
    input->addWidget(new QLabel(tr("Verify"), this), 2,1);
    input->addWidget(&password2, 2, 2);
    input->addWidget(new QLabel(tr("Hint"), this), 3,1);
    input->addWidget(&hint, 3, 2);
    input->setContentsMargins(10, 10,  -10, -10);
    grid->addLayout(input, 1,1);

    msgGrid->addWidget(&error, 1, 1);
    grid->addLayout(msgGrid, 2, 1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    ok.setEnabled(false);

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
    button->addWidget(&ok, 1, 1);
    button->addWidget(cancel, 1,2);
    grid->addLayout(button, 3, 1);

    connect(&password, SIGNAL(textChanged(QString)), this, SLOT(validateInput()));
    connect(&password2, SIGNAL(textChanged(QString)), this, SLOT(validateInput()));
    connect(&hint, SIGNAL(textChanged(QString)), this, SLOT(validateInput()));

}

// The OK button was pressed
void EnCryptDialog::okButtonPressed() {
    wasOkPressed = true;
    close();
}

// The CANCEL button was pressed
void EnCryptDialog::cancelButtonPressed() {
    wasOkPressed = false;
    close();
}

// Get the the validated password from the field
QString EnCryptDialog::getPasswordVerify() {
    return password2.text();
}


// Get the password
QString EnCryptDialog::getPassword() {
    return password.text();

}


// Get the password hint
QString EnCryptDialog::getHint() {
    return hint.text();
}

// Check if proper input was input
void EnCryptDialog::validateInput() {
    ok.setEnabled(false);
    error.setText("");
    if (password.text().length()<4) {
        error.setText(tr("Password must be at least 4 characters"));
        return;
    }
    if (password.text() != password2.text()) {
        error.setText(tr("Passwords do not match"));
        return;
    }
    if (hint.text().trimmed() == ("")) {
        error.setText(tr("Hint must be entered"));
        return;
    }

    ok.setEnabled(true);
}

bool EnCryptDialog::okPressed() {
    return wasOkPressed;
}

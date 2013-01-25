#include "insertlatexdialog.h"
#include <QGridLayout>
#include <QLabel>

InsertLatexDialog::InsertLatexDialog(QWidget *parent) :
    QDialog(parent)
{
    wasOkPressed = false;
    setWindowTitle(tr("Insert LaTeX Formula"));
    //setWindowIcon(new QIcon(iconPath+"link.png"));
    QGridLayout *grid = new QGridLayout(this);
    QGridLayout *input = new QGridLayout(this);
    QGridLayout *button = new QGridLayout(this);
    setLayout(grid);


    url.setText("");

    input->addWidget(new QLabel(tr("Formula")), 1,1);
    input->addWidget(&url, 2, 1);
    input->setContentsMargins(10, 10,  -10, -10);
    grid->addLayout(input, 1,1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(accept()));
    ok.setEnabled(false);

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    button->addWidget(&ok, 1, 1);
    button->addWidget(cancel, 1,2);
    grid->addLayout(button, 3, 1);
    connect(&url, SIGNAL(textChanged()), this, SLOT(validateInput()));

    //setAttribute(Qt.WidgetAttribute.WA_DeleteOnClose);
}


// Set the formula
void InsertLatexDialog::setFormula(QString x) {
    url.setText(x);
}


// Get the formula
QString InsertLatexDialog::getFormula() {
    return latexText;
}



// Set the url
void InsertLatexDialog::setUrl(QString u) {
    url.setText(u);
}



// Check if the OK button was pressed
bool InsertLatexDialog::okPressed() {
    return wasOkPressed;
}



// Check that we have a valid URL
void InsertLatexDialog::validateInput() {
    ok.setEnabled(true);
    if (url.toPlainText().trimmed() == "")
        ok.setEnabled(false);
}


void InsertLatexDialog::accept() {
    if (ok.isEnabled()) {
        wasOkPressed = true;
        latexText = url.toPlainText();
        QDialog::accept();
    }
}


void InsertLatexDialog::reject() {
    wasOkPressed=false;
    QDialog::reject();
}

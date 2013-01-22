#include "tabledialog.h"

#include <QString>

TableDialog::TableDialog(QWidget *parent) :
        QDialog(parent)
{
    okPressed = false;
    this->setWindowTitle(QString(tr("Insert Table")));
    //setWindowIcon(new QIcon(iconPath+"table.png"));
    setLayout(&grid);

    unit.addItem(tr("Percent"),true);
    unit.addItem(tr("Pixels"), false);


    width.setText("80");
    widthValidator.setBottom(0);
    widthValidator.setTop(100);
    width.setValidator(&widthValidator);
    connect(&width, SIGNAL(textChanged(QString)), this, SLOT(validateWidth()));

    rows.setMinimum(1);
    rows.setMaximum(30);
    cols.setMinimum(1);
    cols.setMaximum(30);


    connect(&unit, SIGNAL(activated(int)), this, SLOT(unitChanged()));
    input.addWidget(new QLabel(tr("Rows")),1,1);
    input.addWidget(&rows, 1, 2);
    input.addWidget(new QLabel(tr("Columns")), 2, 1);
    input.addWidget(&cols, 2, 2);
    input.addWidget(new QLabel(tr("Width")), 3, 1);
    input.addWidget(&width, 3, 2);
    input.addWidget(new QLabel(tr("Unit")), 4, 1);
    input.addWidget(&unit, 4, 2);
    input.setContentsMargins(10, 10, -10, -10);
    grid.addLayout(&input, 1,1);
    msgGrid.addWidget(&error,1,1);
    grid.addLayout(&msgGrid, 2, 1);

    ok.setText(tr("OK"));
    connect(&ok, SIGNAL(clicked()), this, SLOT(okButtonPressed()));

    cancel.setText(tr("Cancel"));
    connect(&cancel, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));

    button.addWidget(&ok, 1,1);
    button.addWidget(&cancel, 1,2);
    grid.addLayout(&button, 3, 1);
    this->setLayout(&grid);

}


// The OK button was pressed
void TableDialog::okButtonPressed() {
    okPressed = true;
    close();
}


// The CANCEL button was pressed
void TableDialog::cancelButtonPressed() {
    okPressed = false;
    close();
}


// Check if the OK button was pressed
bool TableDialog::isOkPressed() {
    return okPressed;
}



// Check if proper input was input
void TableDialog::validateInput() {
    ok.setEnabled(false);

    ok.setEnabled(true);
}


void TableDialog::validateWidth() {
    if (width.text().trimmed().length() == 0) {
        ok.setEnabled(false);
    } else
        ok.setEnabled(true);
}



void TableDialog::unitChanged() {
    int i = unit.currentIndex();
    if (unit.itemData(i).toBool()) { // if 'percent'
        int w = width.text().toInt();
        if (w > 100)
            width.setText("80");
        widthValidator.setTop(100);
    } else {
        widthValidator.setTop(32767);
    }
}


int TableDialog::getRows() {
    return rows.text().toInt();
}


int TableDialog::getCols() {
    return cols.text().toInt();
}


int TableDialog::getWidth() {
    return width.text().toInt();
}


bool TableDialog::isPercent() {
    int i = unit.currentIndex();
    return unit.itemData(i).toBool();
}

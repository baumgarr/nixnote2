#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>

class TableDialog  : public QDialog
{
    Q_OBJECT
private:
    QGridLayout grid; // = new QGridLayout();
    QGridLayout input; // = new QGridLayout();
    QGridLayout msgGrid; // = new QGridLayout();
    QGridLayout button; // = new QGridLayout();
    bool 			okPressed;
    QSpinBox		rows;
    QSpinBox 		cols;
    QLineEdit       width;
    QPushButton 	ok;
    QPushButton     cancel;
    QLabel          error;
    QIntValidator	widthValidator;
    QComboBox		 unit;


public:
    TableDialog(QWidget *parent);
    bool isOkPressed();
    int getRows();
    int getCols();
    int getWidth();
    bool isPercent();

private slots:
    void validateInput();
    void validateWidth();
    void unitChanged();
    void okButtonPressed();
    void cancelButtonPressed();

};


#endif // TABLEDIALOG_H

#ifndef ENDECRYPTDIALOG_H
#define ENDECRYPTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class EnDecryptDialog : public QDialog
{
    Q_OBJECT
private:

    QPushButton ok;
    QLabel passwordLabel;
    QLabel hintLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout passwordGrid;
    QGridLayout buttonGrid;




public:
    explicit EnDecryptDialog(QWidget *parent = 0);
    QLineEdit	hint;
    QLineEdit	password;
    bool okPressed;


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

};


#endif // ENDECRYPTDIALOG_H

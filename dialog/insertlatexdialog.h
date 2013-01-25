#ifndef INSERTLATEXDIALOG_H
#define INSERTLATEXDIALOG_H

#include <QDialog>
#include <QString>
#include <QTextEdit>
#include <QPushButton>

class InsertLatexDialog : public QDialog
{
    Q_OBJECT
private:
    bool wasOkPressed;
    QTextEdit url;
    QPushButton ok;
    QString latexText;

public:
    explicit InsertLatexDialog(QWidget *parent = 0);
    void setFormula(QString formula);
    QString getFormula();
    void setUrl(QString url);
    bool okPressed();
    void accept();
    void reject();

signals:
    
public slots:

private slots:
    void validateInput();
    
};

#endif // INSERTLATEXDIALOG_H

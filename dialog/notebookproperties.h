#ifndef NOTEBOOKPROPERTIES_H
#define NOTEBOOKPROPERTIES_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>


class NotebookProperties : public QDialog
{
    Q_OBJECT
private:
    QPushButton ok;
    QLabel nameLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout buttonGrid;
    QGridLayout queryGrid;
    QStringList existingNotebooks;
    QString originalName;
    qint32 lid;
    QCheckBox syncBox;

public:
    explicit NotebookProperties(QWidget *parent = 0);
    QLineEdit	name;
    bool okPressed;
    void setLid(qint32 lid);


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

signals:

public slots:
    
};

#endif // NOTEBOOKPROPERTIES_H

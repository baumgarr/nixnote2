#ifndef TAGPROPERTIES_H
#define TAGPROPERTIES_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

class TagProperties : public QDialog
{
    Q_OBJECT

private:
    QPushButton ok;
    QLabel nameLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout buttonGrid;
    QGridLayout queryGrid;
    QStringList existingTags;
    QString originalName;
    qint32 lid;

public:
    explicit TagProperties(QWidget *parent = 0);
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

#endif // TAGPROPERTIES_H

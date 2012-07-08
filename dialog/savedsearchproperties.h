#ifndef SAVEDSEARCHPROPERTIES_H
#define SAVEDSEARCHPROPERTIES_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class SavedSearchProperties : public QDialog
{
    Q_OBJECT
private:

    QPushButton ok;
    QLabel nameLabel;
    QLabel queryLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout queryGrid;
    QGridLayout buttonGrid;
    QStringList existingSearches;
    QString originalName;
    int lid;

public:
    explicit SavedSearchProperties(QWidget *parent = 0);
    QLineEdit	name;
    QLineEdit	query;
    bool okPressed;
    void setLid(int lid);


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

};
#endif // SAVEDSEARCHPROPERTIES_H

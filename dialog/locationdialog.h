#ifndef LOCATIONDIALOG_H
#define LOCATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleValidator>

class LocationDialog : public QDialog
{
    Q_OBJECT
private:
    bool wasOkPressed;
    QLineEdit longitude;
    QLineEdit latitude;
    QLineEdit altitude;
    QPushButton ok;
    QDoubleValidator longlatval;
    QDoubleValidator altitudeval;

public:
    explicit LocationDialog(QWidget *parent = 0);
    bool okPressed();
    QString locationText();
    QString calculateDegrees(double value);
    double getLongitude();
    double getLatitude();
    double getAltitude();
    void setAltitude(double value);
    void setLongitude(double value);
    void setLatitude(double value);
    
signals:
    
public slots:
    void okButtonPressed();
    void cancelButtonPressed();
    
};

#endif // LOCATIONDIALOG_H

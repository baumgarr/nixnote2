#ifndef LOCATIONEDITOR_H
#define LOCATIONEDITOR_H

#include <QPushButton>

class LocationEditor : public QPushButton
{
    Q_OBJECT
private:
    QString activeColor;
    QString defaultText;
    QString inactiveColor;
    double startAltitude;
    double startLongitude;
    double startLatitude;
    qint32 currentLid;

public:
    explicit LocationEditor(QWidget *parent = 0);
    void setGeography(qint32 lid, double longitude, double latitude, double altitude) ;

signals:

public slots:
    void setActiveColor();
    void buttonClicked();

    
};

#endif // LOCATIONEDITOR_H

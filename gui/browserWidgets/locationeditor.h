#ifndef LOCATIONEDITOR_H
#define LOCATIONEDITOR_H

#include <QPushButton>
#include <QToolButton>
#include <QMenu>

class LocationEditor : public QToolButton
{
    Q_OBJECT
private:
    QString activeColor;
    QString defaultText;
    QString inactiveColor;
    double startAltitude;
    double startLongitude;
    double startLatitude;
    QMenu *actionMenu;
    QAction *clearAction;
    QAction *editAction;
    QAction *viewAction;
    qint32 currentLid;

public:
    explicit LocationEditor(QWidget *parent = 0);
    void setGeography(qint32 lid, double longitude, double latitude, double altitude) ;

signals:

public slots:
    void setActiveColor();
    void buttonClicked();
    void clearClicked();
    void viewClicked();

    
};

#endif // LOCATIONEDITOR_H

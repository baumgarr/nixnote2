#include "locationeditor.h"
#include "dialog/locationdialog.h"
#include "sql/notetable.h"

LocationEditor::LocationEditor(QWidget *parent) :
    QPushButton(parent)
{
    QPalette pal;
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    QFont f = font();
    f.setPointSize(8);
    setFont(f);

    inactiveColor = "QPushButton {background-color: transparent; border-radius: 0px; border:none; margin 0px; padding: 0px} ";
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(inactiveColor);

    defaultText = QString(tr("Click to set location..."));
//    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textModified(QString)));
    this->setText(defaultText);
    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    this->setFlat(true);

    hide();
}



void LocationEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


void LocationEditor::buttonClicked() {
    LocationDialog dialog;
    dialog.setLongitude(this->startLongitude);
    dialog.setLatitude(this->startLatitude);
    dialog.setAltitude(this->startAltitude);
    dialog.exec();

    double lon = dialog.getLongitude();
    double lat = dialog.getLatitude();
    double alt = dialog.getAltitude();
    startAltitude = alt;
    startLongitude = lon;
    startLatitude = lat;
    if (dialog.okPressed()) {
        NoteTable ntable;
        if (lon == 0.0 && lat == 0.0) {
            setText(defaultText);
            ntable.resetGeography(currentLid, true);
            return;
        }
        this->setText(dialog.locationText());
        ntable.setGeography(currentLid, lon,lat,alt, true);
    }
}


void LocationEditor::setGeography(qint32 lid, double longitude, double latitude, double altitude) {
    this->startLongitude = longitude;
    this->startLatitude = latitude;
    this->startAltitude = altitude;
    currentLid = lid;

    LocationDialog dialog;
    dialog.setLongitude(this->startLongitude);
    dialog.setLatitude(this->startLatitude);
    dialog.setAltitude(this->startAltitude);
    this->setText(dialog.locationText());
}

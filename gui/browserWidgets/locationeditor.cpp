#include "locationeditor.h"
#include "dialog/locationdialog.h"
#include "sql/notetable.h"
#include <QDesktopServices>

LocationEditor::LocationEditor(QWidget *parent) :
    QToolButton(parent)
{
    QPalette pal;
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    QFont f = font();
    f.setPointSize(8);
    setFont(f);

    inactiveColor = "QToolButton {background-color: transparent; border-radius: 0px; border:none; margin 0px; padding: 4px} ";
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(inactiveColor);

    defaultText = QString(tr("Click to set location..."));
    this->setText(defaultText);
    actionMenu = new QMenu();
    editAction = actionMenu->addAction(tr("Edit..."));
    clearAction = actionMenu->addAction(tr("Clear"));
    viewAction = actionMenu->addAction(tr("View on map"));
    connect(editAction, SIGNAL(triggered()), this, SLOT(buttonClicked()));
    connect(viewAction, SIGNAL(triggered()), this, SLOT(viewClicked()));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearClicked()));
    setAutoRaise(false);
    setMenu(actionMenu);
    this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->setIcon(QIcon(":navigation.png"));

    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));

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


void LocationEditor::setGeography(qint32 lid, double longitude, double latitude, double altitude, QString placeName) {
    this->startLongitude = longitude;
    this->startLatitude = latitude;
    this->startAltitude = altitude;
    currentLid = lid;

    Note n;
    NoteTable ntable;
    ntable.get(n, lid, false,false);
    if (!n.__isset.attributes || !n.attributes.__isset.latitude || !n.attributes.__isset.longitude)
        return;

    if (placeName == "") {
        LocationDialog dialog;
        dialog.setLongitude(this->startLongitude);
        dialog.setLatitude(this->startLatitude);
        dialog.setAltitude(this->startAltitude);
        this->setText(dialog.locationText());
    } else
        this->setText(placeName);
}


void LocationEditor::clearClicked() {
    NoteTable ntable;
    ntable.resetGeography(currentLid, true);
}

void LocationEditor::viewClicked() {
    if (this->text().toLower() == defaultText.toLower())
        return;
    QDesktopServices::openUrl(QUrl("http://maps.google.com/maps?z=6&q=" +QString::number(startLatitude) +","
                                       +QString::number(startLongitude)));
}

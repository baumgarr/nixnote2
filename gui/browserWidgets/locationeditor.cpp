/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "locationeditor.h"
#include "dialog/locationdialog.h"
#include "sql/notetable.h"
#include <QDesktopServices>
#include <QWeakPointer>
#include "global.h"

extern Global global;

LocationEditor::LocationEditor(QWidget *parent) :
    QToolButton(parent)
{
    QPalette pal;
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    this->setFont(global.getGuiFont(font()));

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
    this->reloadIcons();
    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    hide();
}

void LocationEditor::reloadIcons() {
    this->setIcon(global.getIconResource(":navigationIcon"));
}



void LocationEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


void LocationEditor::buttonClicked() {
    const QWeakPointer<LocationDialog> dialogPtr(new LocationDialog(this));
    if (LocationDialog *const dialog = dialogPtr.data()) {
        dialog->setLongitude(this->startLongitude);
        dialog->setLatitude(this->startLatitude);
        dialog->setAltitude(this->startAltitude);
        dialog->exec();
    }

    if (LocationDialog *const dialog = dialogPtr.data()) {
        const double lon = dialog->getLongitude();
        const double lat = dialog->getLatitude();
        const double alt = dialog->getAltitude();
        startAltitude = alt;
        startLongitude = lon;
        startLatitude = lat;
        if (dialog->okPressed()) {
            NoteTable ntable(global.db);
            if (lon == 0.0 && lat == 0.0) {
                setText(defaultText);
                ntable.resetGeography(currentLid, true);
            } else {
                setText(dialog->locationText());
                ntable.setGeography(currentLid, lon,lat,alt, true);
            }
        }
        delete dialog;
    }
}


void LocationEditor::setGeography(qint32 lid, double longitude, double latitude, double altitude, QString placeName) {
    this->startLongitude = longitude;
    this->startLatitude = latitude;
    this->startAltitude = altitude;
    currentLid = lid;

    Note n;
    NoteTable ntable(global.db);
    ntable.get(n, lid, false, false);
    NoteAttributes attributes;
    if (n.attributes.isSet())
        attributes = n.attributes;
    if (!attributes.latitude.isSet() || !attributes.longitude.isSet())
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



void LocationEditor::getGeography(double &longitude, double &latitude, double &altitude, QString &placeName) {
    longitude = this->startLongitude;
    latitude = this->startLatitude;
    altitude = this->startAltitude;
    placeName = text();
}




void LocationEditor::clearClicked() {
    NoteTable ntable(global.db);
    ntable.resetGeography(currentLid, true);
}

void LocationEditor::viewClicked() {
    if (this->text().toLower() == defaultText.toLower())
        return;
    QDesktopServices::openUrl(QUrl("http://maps.google.com/maps?z=6&q=" +QString::number(startLatitude) +","
                                       +QString::number(startLongitude)));
}

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
    void setGeography(qint32 lid, double longitude, double latitude, double altitude, QString placeName) ;
    void getGeography(double &longitude, double &latitude, double &altitude, QString &placeName) ;
    void reloadIcons();

signals:

public slots:
    void setActiveColor();
    void buttonClicked();
    void clearClicked();
    void viewClicked();

    
};

#endif // LOCATIONEDITOR_H

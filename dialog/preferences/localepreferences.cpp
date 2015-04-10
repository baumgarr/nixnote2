/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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


#include "localepreferences.h"
#include "global.h"

#include <QDate>

extern Global global;

LocalePreferences::LocalePreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(mainLayout);
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    dateFormatLabel = new QLabel(tr("Date Format"), this);
    dateFormatLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    dateFormat = new QComboBox(this);
    dateFormat->addItem(tr("MM/dd/yy - ") + date.toString("MM/dd/yy"), MMddyy);
    dateFormat->addItem(tr("MM/dd/yyyy - ")+ date.toString("MM/dd/yyyy"), MMddyyyy);
    dateFormat->addItem(tr("M/dd/yyyy - ")+ date.toString("M/dd/yyyy"), Mddyyyy);
    dateFormat->addItem(tr("M/d/yyyy - ")+ date.toString("M/d/yyyy"), Mdyyyy);
    dateFormat->addItem(tr("dd/MM/yy - ")+ date.toString("dd/MM/yy"), ddMMyy);
    dateFormat->addItem(tr("d/M/yy - ")+ date.toString("d/M/yy"), dMyy);
    dateFormat->addItem(tr("dd/MM/yyyy - ")+ date.toString("dd/MM/yyyy"), ddMMyyyy);
    dateFormat->addItem(tr("d/M/yyyy - ")+ date.toString("d/M/yyyy"), dMyyyy);
    dateFormat->addItem(tr("yyyy-MM-dd - ")+ date.toString("yyyy-MM-dd"), yyyyMMdd);
    dateFormat->addItem(tr("yy-MM-dd - ")+ date.toString("yy-MM-dd"), yyyyMMdd);


    timeFormatLabel = new QLabel(tr("Time Format"), this);
    timeFormatLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    timeFormat = new QComboBox(this);
    timeFormat->addItem(tr("HH:mm:ss - ")+time.toString("HH:mm:ss"), HHmmss);
    timeFormat->addItem(tr("HH:mm:ss a - ")+time.toString("HH:mm:ss a"), HHmmssa);
    timeFormat->addItem(tr("HH:mm - ")+time.toString("HH:mm"), HHmm);
    timeFormat->addItem(tr("HH:mm a - ")+time.toString("HH:mm a"), HHmma);
    timeFormat->addItem(tr("hh:mm:ss - ")+time.toString("hh:mm:ss"), hhmmss);
    timeFormat->addItem(tr("hh:mm:ss a- ")+time.toString("hh:mm:ss a"), hhmmssa);
    timeFormat->addItem(tr("h:mm:ss a - ")+time.toString("h:mm:ss a"), hmmssa);
    timeFormat->addItem(tr("hh:mm - ")+time.toString("HH:mm"), hhmm);
    timeFormat->addItem(tr("hh:mm a - ")+time.toString("hh:mm a"), hhmma);
    timeFormat->addItem(tr("h:mm a - ")+time.toString("h:mm a"), hmma);

    mainLayout->addWidget(dateFormatLabel,0,0);
    mainLayout->addWidget(dateFormat,0,1);
    mainLayout->addWidget(timeFormatLabel,1,0);
    mainLayout->addWidget(timeFormat, 1,1);

    global.settings->beginGroup("Locale");
    int datei = global.settings->value("dateFormat", MMddyy).toInt();
    int timei = global.settings->value("timeFormat", HHmmss).toInt();
    global.settings->endGroup();
    int index = dateFormat->findData(datei);
    dateFormat->setCurrentIndex(index);
    index = timeFormat->findData(timei);
    timeFormat->setCurrentIndex(index);
    this->setFont(global.getGuiFont(font()));
}


LocalePreferences::~LocalePreferences() {
//    delete dateFormat;
//    delete dateFormatLabel;
//    delete timeFormat;
//    delete timeFormatLabel;
//    delete mainLayout;
}



void LocalePreferences::saveValues() {
    int date = getDateFormat();
    int time = getTimeFormat();

    global.settings->beginGroup("Locale");
    global.settings->setValue("dateFormat", date);
    global.settings->setValue("timeFormat", time);
    global.settings->endGroup();

    datefmt = "MM/dd/yy";
    switch (date) {
    case MMddyy:
        datefmt = "MM/dd/yy";
        break;
    case MMddyyyy:
        datefmt = "MM/dd/yyyy";
        break;
    case Mddyyyy:
        datefmt = "M/dd/yyyy";
        break;
    case Mdyyyy:
        datefmt = "M/d/yyyy";
        break;
    case ddMMyy:
        datefmt = "dd/MM/yy";
        break;
    case dMyy:
        datefmt = "d/M/yy";
        break;
    case ddMMyyyy:
        datefmt = "dd/MM/yyyy";
        break;
    case dMyyyy:
        datefmt = "d/M/yyyy";
        break;
    case yyyyMMdd:
        datefmt = "yyyy-MM-dd";
        break;
    case yyMMdd:
        datefmt = "yy-MM-dd";
        break;
    }

    timefmt = "HH:mm:ss";
    switch (time) {
    case HHmmss:
        timefmt = "HH:mm:ss";
        break;
    case HHmmssa:
        timefmt = "HH:MM:SS a";
        break;
    case HHmm:
        timefmt = "HH:mm";
        break;
    case HHmma:
        timefmt = "HH:mm a";
        break;
    case hhmmss:
        timefmt = "hh:mm:ss";
        break;
    case hhmmssa:
        timefmt = "hh:mm:ss a";
        break;
    case hhmm:
        timefmt = "hh:mm";
        break;
    case hhmma:
        timefmt = "hh:mm a";
        break;
    case hmma:
        timefmt = "h:mm a";
        break;
    case hmmssa:
        timefmt = "h:mm:ss a";
        break;
    }
    global.dateFormat = datefmt;
    global.timeFormat = timefmt;
}




int LocalePreferences::getDateFormat() {
    int index = dateFormat->currentIndex();
    return dateFormat->itemData(index).toInt();
}



int LocalePreferences::getTimeFormat() {
    int index = timeFormat->currentIndex();
    return timeFormat->itemData(index).toInt();
}

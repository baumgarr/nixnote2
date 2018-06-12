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



#ifndef LOCALEPREFERENCES_H
#define LOCALEPREFERENCES_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>

class LocalePreferences : public QWidget
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;
    QLabel *dateFormatLabel;
    QComboBox *dateFormat;
    QLabel *timeFormatLabel;
    QComboBox *timeFormat;
    QLabel *translationLabel;
    QComboBox *translationCombo;
    int getDateFormat();
    int getTimeFormat();
    QString getTranslation();

    // see also duplicate in Global - global.cpp
    enum DateFormat {
        MMddyy = 1,
        MMddyyyy = 2,
        Mddyyyy = 3,
        Mdyyyy = 4,
        ddMMyy = 5,
        dMyy = 6,
        ddMMyyyy = 7,
        dMyyyy = 8,
        yyyyMMdd = 9,
        yyMMdd = 10,
        yyMMdd2 = 11
    };
    enum TimeFormat {
        HHmmss = 1,
        HHmmssa = 2,
        HHmm = 3,
        HHmma = 4,
        hhmmss = 5,
        hhmmssa = 6,
        hmmssa = 7,
        hhmm = 8,
        hhmma = 9,
        hmma = 10
    };

public:
    explicit LocalePreferences(QWidget *parent = 0);
    ~LocalePreferences();
    void saveValues();
    QString datefmt;
    QString timefmt;

signals:

public slots:
    
};

#endif // LOCALEPREFERENCES_H

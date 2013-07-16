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
    int getDateFormat();
    int getTimeFormat();
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
        yyMMdd = 10
    };
    enum TimeFormat {
        HHmmss = 1,
        HHMMSSa = 2,
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

#include "datedelegate.h"
#include <QDateTime>

DateDelegate::DateDelegate()
{
}


QString DateDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    if (value.toLongLong() == 0)
        return "";
    QDateTime timestamp;
    timestamp.setTime_t(value.toLongLong()/1000);

    QDate currentDate;
    if (timestamp.date() == QDate::currentDate())
            return "Today " + timestamp.time().toString(Qt::SystemLocaleShortDate);

    return timestamp.toString(Qt::SystemLocaleShortDate);
}

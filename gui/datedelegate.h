#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QStyledItemDelegate>

class DateDelegate : public QStyledItemDelegate
{
public:
    DateDelegate();
    QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // DATEDELEGATE_H

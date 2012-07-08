#ifndef NUMBERDELEGATE_H
#define NUMBERDELEGATE_H

#include <QStyledItemDelegate>

class NumberDelegate : public QStyledItemDelegate
{
public:
    enum Format {
        KBNumber,
        BlankNumber,
        ZeroNumber
    };
    Format fmt;

    NumberDelegate(Format f);
    QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // NUMBERDELEGATE_H

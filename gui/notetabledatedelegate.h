#ifndef NOTETABLEDATEDELEGATE_H
#define NOTETABLEDATEDELEGATE_H

#include <QStyledItemDelegate>

class NoteTableDateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NoteTableDateDelegate(QObject *parent = 0);
    QString displayText(const QVariant &value, const QLocale &locale) const {

signals:

public slots:

};

#endif // NOTETABLEDATEDELEGATE_H

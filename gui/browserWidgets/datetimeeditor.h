#ifndef DATETIMEEDITOR_H
#define DATETIMEEDITOR_H

#include <QDateTimeEdit>
#include <QDateTime>
#include <QWidget>
#include <QPushButton>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

using namespace evernote::edam  ;
using namespace std;


class DateTimeEditor : public QDateTimeEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    Timestamp value;
    qint32 key;
    qint32 lid;

public:
    explicit DateTimeEditor(QWidget *parent = 0);
    void setNote(qint32 lid, Timestamp value, qint32 key);
    
signals:
    
protected:
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);

public slots:

private slots:
    void valueChanged();

    
};

#endif // DATETIMEEDITOR_H

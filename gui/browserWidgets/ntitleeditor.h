#ifndef NTITLEEDITOR_H
#define NTITLEEDITOR_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "evernote/Limits_constants.h"

#include <QtWebKit>
#include <QLineEdit>


using namespace evernote::edam;
using namespace evernote::limits;

class NTitleEditor : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString initialTitle;
    QString priorTitle;
    qint32 currentLid;
    QString defaultTitle;
    QString cleanupTitle(QString text);

public:
    explicit NTitleEditor(QWidget *parent = 0);
    void setTitle(qint32 lid, QString title, QString defaultT);
    void setTitleFromContent(QString s);

public slots:
    void setActiveColor();
    void titleChanged(QString text);

signals:
  void focussed(bool hasFocus);
  void titleChanged();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
    
};

#endif // NTITLEEDITOR_H

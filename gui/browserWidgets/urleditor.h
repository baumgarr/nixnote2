#ifndef URLEDITOR_H
#define URLEDITOR_H

#include <QLineEdit>

class UrlEditor : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString defaultText;
    QString initialText;
    QString priorText;
    qint32 currentLid;

public:
    explicit UrlEditor(QWidget *parent = 0);
    QString getText();

public slots:
    void setActiveColor();
    void textModified(QString text);
    void gainedFocus(bool focus);
    void setUrl(qint32 lid, QString text);

signals:
  void focussed(bool hasFocus);
  void textUpdated();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
};

#endif // URLEDITOR_H

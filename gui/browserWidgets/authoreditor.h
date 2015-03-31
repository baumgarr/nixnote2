#ifndef AUTHOREDITOR_H
#define AUTHOREDITOR_H

#include <QLineEdit>

class AuthorEditor : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString initialText;
    QString priorText;
    qint32 currentLid;

public:
    explicit AuthorEditor(QWidget *parent = 0);
    QString getText();

public slots:
    void setActiveColor();
    void textModified(QString text);
    void setAuthor(qint32 lid, QString text);

signals:
  void focussed(bool hasFocus);
  void textUpdated();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
};

#endif // AUTHOREDITOR_H

#ifndef TAGEDITORNEWTAG_H
#define TAGEDITORNEWTAG_H

#include <QLineEdit>
#include <QCompleter>
#include <QEvent>

class TagEditorNewTag : public QLineEdit
{
    Q_OBJECT
private:
    QString activeColor;
    QString inactiveColor;
    QString defaultText;
    QCompleter *completer;
    QStringList currentTags;
    void loadCompleter();

public:
    explicit TagEditorNewTag(QWidget *parent = 0);
    QString getText();
    void setTags(QStringList s);
    void resetText();
    void addTag(QString s);

public slots:
    void setActiveColor();
    void textModified(QString text);
    void gainedFocus(bool focus);

signals:
  void focussed(bool hasFocus);
  void tabPressed();

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
  bool event(QEvent *event);

};

#endif // TAGEDITORNEWTAG_H

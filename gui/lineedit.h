/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

 #ifndef LINEEDIT_H
 #define LINEEDIT_H

 #include <QLineEdit>

 class QToolButton;

 class LineEdit : public QLineEdit
 {
     Q_OBJECT
 private:
     int filterPosition;
     QString savedText;
     QString defaultText;
     QString activeColor;
     QString inactiveColor;

 public:
     LineEdit(QWidget *parent = 0);
     void updateSelection();
     bool isSet();

 protected:

 private slots:
     void buildSelection();
     void textChanged(QString text);

 private:

 signals:
     void updateSelectionRequested();
 };

 #endif // LIENEDIT_H

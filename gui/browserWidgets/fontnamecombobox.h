#ifndef FONTCOMBOBOX_H
#define FONTCOMBOBOX_H

#include <QComboBox>

class FontNameComboBox : public QComboBox
{
    Q_OBJECT
private:
    bool expanded;
public:
    explicit FontNameComboBox(QWidget *parent = 0);
    bool isExpanded();
protected:
//    void focusInEvent(QFocusEvent *event);
//    void focusOutEvent(QFocusEvent *event);

signals:


public slots:
    void showPopup();
    void hidePopup();


};

#endif // FONTCOMBOBOX_H

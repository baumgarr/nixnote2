#ifndef FONTSIZECOMBOBOX_H
#define FONTSIZECOMBOBOX_H

#include <QWidget>
#include <QComboBox>

class FontSizeComboBox : public QComboBox
{
    Q_OBJECT
private:
    bool expanded;
public:
    explicit FontSizeComboBox(QWidget *parent = 0);
    bool isExpanded();

signals:

public slots:
    void showPopup();
    void hidePopup();


};

#endif // FONTSIZECOMBOBOX_H

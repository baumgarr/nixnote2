#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H

#include <QToolButton>

#define EXPANDBUTTON_1 1
#define EXPANDBUTTON_2 2
#define EXPANDBUTTON_3 3
class ExpandButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ExpandButton(QWidget *parent = 0);
    int currentState;
    
signals:
    void stateChanged(int state);
    
public slots:
    void buttonClicked();
    
};

#endif // EXPANDBUTTON_H

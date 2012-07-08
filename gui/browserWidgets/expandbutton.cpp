#include "expandbutton.h"

ExpandButton::ExpandButton(QWidget *parent) :
    QToolButton(parent)
{

    setIcon(QIcon(":down_arrow_small.png"));
    this->resize(5,5);
    currentState = EXPANDBUTTON_1;

    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}


void ExpandButton::buttonClicked() {
    switch (currentState) {
    case EXPANDBUTTON_1 :
        currentState = EXPANDBUTTON_2;
        emit(stateChanged(currentState));
        break;
    case EXPANDBUTTON_2 :
        currentState = EXPANDBUTTON_3;
        setIcon(QIcon(":up_arrow_small.png"));
        emit(stateChanged(currentState));
        break;
    case EXPANDBUTTON_3 :
        currentState = EXPANDBUTTON_1;
        setIcon(QIcon(":down_arrow_small.png"));
        emit(stateChanged(currentState));
        break;
    }
}

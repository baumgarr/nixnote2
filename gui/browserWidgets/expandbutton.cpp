/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "expandbutton.h"
#include "global.h"

extern Global global;

ExpandButton::ExpandButton(QWidget *parent) :
    QToolButton(parent)
{

    setIcon(global.getIconResource(":down_arrow_small.png"));
    this->resize(5,5);
    currentState = EXPANDBUTTON_1;

    connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ExpandButton::setState(int state) {
    currentState = state;
    switch (currentState) {
    case EXPANDBUTTON_1 :
        setIcon(global.getIconResource(":down_arrow_small.png"));
        emit(stateChanged(currentState));
        break;
    case EXPANDBUTTON_2 :
        setIcon(global.getIconResource(":down_arrow_small.png"));
        emit(stateChanged(currentState));
        break;
    case EXPANDBUTTON_3 :
        setIcon(global.getIconResource(":up_arrow_small.png"));
        emit(stateChanged(currentState));
        break;
    }
}

void ExpandButton::buttonClicked() {
    switch (currentState) {
    case EXPANDBUTTON_1 :
        currentState = EXPANDBUTTON_2;
        break;
    case EXPANDBUTTON_2 :
        currentState = EXPANDBUTTON_3;
        break;
    case EXPANDBUTTON_3 :
        currentState = EXPANDBUTTON_1;
        break;
    }
    setState(currentState);
}

#include "ntableviewheader.h"
#include "global.h"


//************************************************************
//* This overrides QHeaderView and is used so we can have
//* things like custom context menus
//************************************************************
NTableViewHeader::NTableViewHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent)
{

}

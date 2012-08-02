#include "nwebpage.h"
#include <QDebug>
#include "global.h"

//*******************************************************
//* This class inherits everything from QWebPage.  It
//* handles some of the special things we need in
//* QWebPage to be able to edit notes.
//*******************************************************

NWebPage::NWebPage(QWidget *parent) :
    QWebPage(parent)
{
    connect(this,SIGNAL(contentsChanged()), this, SLOT(editAlert()));
}


// This class overrides the one provided by QWebPage.  This shows us
// any javascript console messages that are produced.
void NWebPage::javaScriptConsoleMessage(QString message, int lineNumber, QString sourceID) {
   QLOG_DEBUG() << "Javascript message: " << "Line: " << lineNumber << " Source: " << sourceID <<
           " message: " << message;
}


void NWebPage::editAlert() {
    isDirty = true;
}

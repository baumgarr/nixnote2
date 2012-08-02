#include "nwebview.h"
#include "nwebpage.h"


//****************************************************
//* This inherits everything from QWebView.  It is
//* useful to allow us to setup things that are
//* needed to view & edit notes.
//****************************************************

// Override the constructor so we always use a NWebPage
// rather than a QWebPage
NWebView::NWebView(QWidget *parent) :
    QWebView(parent)
{
    editorPage = new NWebPage();
    setPage(editorPage);
    isDirty = false;
    connect(editorPage, SIGNAL(contentsChanged()), this, SLOT(editAlert()));
    editorPage->setContentEditable(false);
}


void NWebView::editAlert() {
    isDirty =true;
}

#ifndef NWEBVIEW_H
#define NWEBVIEW_H

#include <QWebView>


//****************************************************
//* This inherits everything from QWebView.  It is
//* useful to allow us to setup things that are
//* needed to view & edit notes.
//****************************************************
class NWebView : public QWebView
{
    Q_OBJECT
public:
    explicit NWebView(QWidget *parent = 0);

signals:

public slots:

};

#endif // NWEBVIEW_H

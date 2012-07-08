#ifndef NWEBPAGE_H
#define NWEBPAGE_H

#include <QWebPage>


//*******************************************************
//* This class inherits everything from QWebPage.  It
//* handles some of the special things we need in
//* QWebPage to be able to edit notes.
//*******************************************************
class NWebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit NWebPage(QWidget *parent = 0);  // Constructor

protected:
    void javaScriptConsoleMessage(QString message, int lineNumber, QString sourceID);
signals:

public slots:

};

#endif // NWEBPAGE_H

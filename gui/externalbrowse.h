#ifndef EXTERNALBROWSE_H
#define EXTERNALBROWSE_H

#include <QMdiSubWindow>
#include <gui/nbrowserwindow.h>

class ExternalBrowse : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ExternalBrowse(qint32 lid, QWidget *parent = 0);
    NBrowserWindow *browser;
    
signals:
    
public slots:
    void closeEvent(QCloseEvent *closeEvent);
    void setTitle(QString text);
    
};

#endif // EXTERNALBROWSE_H

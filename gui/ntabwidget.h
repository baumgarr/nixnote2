#ifndef NTABWIDGET_H
#define NTABWIDGET_H

#include "gui/nbrowserwindow.h"

#include <QTabBar>
#include <QStackedWidget>

class NTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NTabWidget();
    QTabBar tabBar;
    QList<NBrowserWindow *> *browserList;
    void addBrowser(NBrowserWindow *v, QString title);
    void setTitle(QString t);
    void setTitle(int index, QString title);
    QStackedWidget stack;
    NBrowserWindow *currentBrowser();


signals:
    void updateSelectionRequested();
    void noteUpdated(int);
    void tagCreated(int);

public slots:
    void closeTab(int index);
    void moveTab(int to, int from);
    void openNote(int lid, bool newWindow);
    void tagCreationSignaled(int lid);

private slots:
    void noteUpdateSignaled(int);

};

#endif // NTABWIDGET_H

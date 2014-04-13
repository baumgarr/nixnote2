#ifndef TRAYMENU_H
#define TRAYMENU_H

#include <QMenu>
#include <QSignalMapper>

class TrayMenu : public QMenu
{
    Q_OBJECT
private:
    QList<QAction*> pinnedActions;
    QMenu *pinnedMenu;
    QSignalMapper *signalMapper;

public:
    explicit TrayMenu(QWidget *parent = 0);
    void setPinnedMenu(QMenu *menu);

signals:
    void openNote(qint32);

public slots:
    void buildPinnedMenu();
    void noteChosen(int note);

};

#endif // TRAYMENU_H

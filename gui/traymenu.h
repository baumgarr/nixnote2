#ifndef TRAYMENU_H
#define TRAYMENU_H

#include <QMenu>
#include <QSignalMapper>

class TrayMenu : public QMenu
{
    Q_OBJECT
private:
    QList<QAction*> actions;
    QMenu *pinnedMenu;
    QMenu *recentlyUpdatedMenu;
    QSignalMapper *signalMapper;
    void buildMenu(QMenu *actionMenu, QList<QPair<qint32, QString> > records);

public:
    enum ActionMenuType {
        PinnedMenu,
        RecentMenu
    };

    explicit TrayMenu(QWidget *parent = 0);
    void setActionMenu(ActionMenuType type, QMenu *menu);

signals:
    void openNote(qint32);

public slots:
    void buildActionMenu();
    void noteChosen(int note);

};

#endif // TRAYMENU_H

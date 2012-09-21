#ifndef NOTEBOOKMENUBUTTON_H
#define NOTEBOOKMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include <QString>
#include <QList>
#include <QAction>
#include <QActionGroup>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>


using namespace evernote::edam  ;
using namespace std;


class NotebookMenuButton : public QPushButton
{
    Q_OBJECT
private:
    QString currentNotebookName;
    QMenu rootMenu;
    QList<QMenu*> stackMenus;
    QList<QAction*> actions;
    int currentAction;
    qint32 currentNoteLid;
    void addNotebookMenuItem(QMenu *menu, QAction *action);
    void addStackMenuItem(QMenu *newMenu);
    QMenu *findStack(Notebook n);

public:
    explicit NotebookMenuButton(QWidget *parent = 0);
    void setCurrentNotebook(qint32 lid, Note n);
    void addNotebook(qint32 lid);

    
signals:
    void notebookChanged();

public slots:
    void loadData();
    void reloadData();
    
private slots:
    void notebookSelected();
    
};

#endif // NOTEBOOKMENUBUTTON_H

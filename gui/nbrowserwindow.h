#ifndef NBROWSERWINDOW_H
#define NBROWSERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QHBoxLayout>

#include "gui/nwebview.h"

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "gui/browserWidgets/ntitleeditor.h"
#include "gui/browserWidgets/notebookmenubutton.h"
#include "gui/browserWidgets/expandbutton.h"
#include "gui/browserWidgets/urleditor.h"
#include "gui/browserWidgets/tageditor.h"
#include "gui/browserWidgets/authoreditor.h"
#include "gui/browserWidgets/dateeditor.h"


using namespace evernote::edam  ;

class NBrowserWindow : public QWidget
{
    Q_OBJECT
private:

public:
    explicit NBrowserWindow(QWidget *parent = 0);
    NWebView editor;
    void setContent(qint32 lid);
    NTitleEditor noteTitle;
    NotebookMenuButton notebookMenu;
    ExpandButton expandButton;
    UrlEditor urlEditor;
    TagEditor tagEditor;
    DateEditor dateEditor;
    AuthorEditor authorEditor;

    QHBoxLayout line2Layout;
    QHBoxLayout line3Layout;
    qint32 lid;

signals:
    void noteUpdated(qint32);
    qint32 tagAdded(qint32);

public slots:
    void changeExpandState(int value);
    void tagRenamed(qint32 lid, QString oldName, QString newName);
    void notebookRenamed(qint32 lid, QString oldName, QString newName);
    void tagDeleted(qint32 lid, QString name);
    void notebookDeleted(qint32 lid, QString name);
    void notebookAdded(qint32 lid);
    void addTagName(qint32 lid);
    void stackRenamed(QString oldName, QString newName);
    void stackDeleted(QString name);
    void stackAdded(QString name);
    void noteSyncUpdate(qint32 lid);

private slots:
    void sendUpdateSignal();
    void newTagAdded(qint32);


};

#endif // NBROWSERWINDOW_H

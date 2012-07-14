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

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
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
    void setContent(int lid, QByteArray c);
    NTitleEditor noteTitle;
    NotebookMenuButton notebookMenu;
    ExpandButton expandButton;
    UrlEditor urlEditor;
    TagEditor tagEditor;
    DateEditor dateEditor;
    AuthorEditor authorEditor;

    QHBoxLayout line2Layout;
    QHBoxLayout line3Layout;
    int lid;

signals:
    void noteUpdated(int);
    int tagAdded(int);

public slots:
    void changeExpandState(int value);
    void tagRenamed(int lid, QString oldName, QString newName);

private slots:
    void sendUpdateSignal();
    void newTagAdded(int);


};

#endif // NBROWSERWINDOW_H

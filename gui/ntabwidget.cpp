#include "ntabwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "html/noteformatter.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

NTabWidget::NTabWidget(SyncRunner *s, NNotebookView *n, NTagView *t)
{
    syncThread = s;
    notebookTreeView = n;
    tagTreeView = t;

    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    tabBar = new QTabBar(this);
    tabBar->setHidden(true);
    tabBar->setMovable(true);
    tabBar->setTabsClosable(true);
    tabBar->setShape(QTabBar::RoundedNorth);
 //   tabBar.setMinimumHeight(20);
 //   tabBar.setMaximumHeight(20);

    browserList = new QList<NBrowserWindow *>();
    vboxlayout.addWidget(tabBar);
    vboxlayout.addWidget(&stack);
    setLayout(&vboxlayout);

    connect(tabBar, SIGNAL(currentChanged(int)),
                 &stack, SLOT(setCurrentIndex(int)));
    connect(tabBar, SIGNAL(tabCloseRequested(int)),
                 this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(tabMoved(int, int)),
                 this, SLOT(moveTab(int, int)));
    this->layout()->setMargin(0);
}

NTabWidget::~NTabWidget() {
    delete browserList;
}

void NTabWidget::addBrowser(NBrowserWindow *v, QString title) {
    tabBar->addTab(title);
    int index = stack.addWidget(v);
    stack.setCurrentIndex(index);
    browserList->append(v);
    tabBar->setCurrentIndex(index);
    tabBar->raise();

    setupConnections(v);

    if (browserList->size() <= 1)
        tabBar->setHidden(true);
    else
        tabBar->setHidden(false);
    return;
}

void NTabWidget::closeTab(int index) {
    tabBar->removeTab(index);
    stack.removeWidget(browserList->at(index));
    browserList->removeAt(index);
    if (browserList->size() <= 1)
        tabBar->setHidden(true);
    else
        tabBar->setHidden(false);
}



void NTabWidget::moveTab(int from, int to) {
    stack.removeWidget(browserList->at(from));
    stack.insertWidget(to, browserList->at(from));
    browserList->move(from, to);

}


NBrowserWindow* NTabWidget::currentBrowser() {
    return this->browserList->at(tabBar->currentIndex());
}


void NTabWidget::openNote(qint32 lid, bool newWindow) {

    // If the lid < 0, then we just clear it & get out
    if (lid < 0) {
        QLOG_DEBUG() << tabBar->currentIndex();
        tabBar->setTabText(tabBar->currentIndex(), QString(""));
        currentBrowser()->setContent(-1);
        return;
    }

    // If we've gotten this far, we have a good note
    Note n;
    NoteTable noteTable;
    noteTable.get(n, lid, true,false);

    NBrowserWindow *view;
    bool found = false;

    // Find out if we already have it open somewhere else
    for (int i=0;i<browserList->size() && !found; i++) {
        if (browserList->at(i)->lid == lid) {
            found = true;
            tabBar->setCurrentIndex(i);
            tabBar->raise();
            stack.setCurrentIndex(i);
            stack.raise();
            return;
        }
    }

    // if we want a new window AND it isn't already open, create a new tab
    if (newWindow && !found) {
        view = new NBrowserWindow();
        addBrowser(view, QString::fromStdString(n.title));
        connect(view, SIGNAL(noteUpdated(qint32)), this, SLOT(noteTitleUpdateSignaled(qint32)));
    } else {
        view = currentBrowser();
        tabBar->setTabText(tabBar->currentIndex(), QString::fromStdString(n.title));
    }

    view->setContent(lid);
}


void NTabWidget::setTitle(QString t) {
    this->setTitle(tabBar->currentIndex(), t);
}

void NTabWidget::setTitle(int index, QString t) {
    tabBar->setTabText(index, t);
}


void NTabWidget::noteUpdateSignaled(qint32 lid) {
    emit(this->noteUpdated(lid));

    Note n;
    NoteTable noteTable;
    noteTable.get(n, lid, false,false);
    for (int i=0;i<browserList->size(); i++) {
        if (browserList->at(i)->lid == lid) {
            setTitle(i, QString::fromStdString(n.title));
            return;
        }
    }
}


void NTabWidget::noteSyncSignaled(qint32 lid) {
    emit(this->noteUpdated(lid));

    Note n;
    NoteTable noteTable;
    noteTable.get(n, lid, false,false);
    for (int i=0;i<browserList->size(); i++) {
        if (browserList->at(i)->lid == lid) {
            setTitle(i, QString::fromStdString(n.title));
            return;
        }
    }
}


void NTabWidget::tagCreationSignaled(qint32 lid) {
    emit(this->tagCreated(lid));
}


void NTabWidget::setupConnections(NBrowserWindow *newBrowser) {
    connect(tagTreeView, SIGNAL(tagRenamed(qint32,QString,QString)), newBrowser, SLOT(tagRenamed(qint32,QString,QString)));
    connect(tagTreeView, SIGNAL(tagDeleted(qint32, QString)), newBrowser, SLOT(tagDeleted(qint32, QString)));
    connect(tagTreeView, SIGNAL(tagAdded(qint32)), newBrowser, SLOT(addTagName(qint32)));

    connect(notebookTreeView, SIGNAL(notebookRenamed(qint32,QString,QString)), newBrowser, SLOT(notebookRenamed(qint32,QString,QString)));
    connect(notebookTreeView, SIGNAL(notebookDeleted(qint32, QString)), newBrowser, SLOT(notebookDeleted(qint32, QString)));
    connect(notebookTreeView, SIGNAL(notebookAdded(qint32)), newBrowser, SLOT(notebookAdded(qint32)));

    connect(notebookTreeView, SIGNAL(stackRenamed(QString,QString)), newBrowser, SLOT(stackRenamed(QString,QString)));
    connect(notebookTreeView, SIGNAL(stackDeleted(QString)), newBrowser, SLOT(stackDeleted(QString)));
    connect(notebookTreeView, SIGNAL(stackAdded(QString)), newBrowser, SLOT(stackAdded(QString)));

    connect(newBrowser, SIGNAL(noteUpdated(qint32)), this, SLOT(noteUpdateSignaled(qint32)));
    connect(newBrowser, SIGNAL(tagAdded(qint32)), this, SLOT(tagCreationSignaled(qint32)));

    connect(syncThread, SIGNAL(noteUpdated(qint32)), this, SLOT(noteSyncSignaled(qint32)));
}

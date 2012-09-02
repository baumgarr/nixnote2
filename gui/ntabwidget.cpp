#include "ntabwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "html/noteformatter.h"
#include "sql/notetable.h"
#include "global.h"

extern Global global;

NTabWidget::NTabWidget()
{
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    tabBar.setHidden(true);
    tabBar.setMovable(true);
    tabBar.setTabsClosable(true);
    tabBar.setShape(QTabBar::RoundedNorth);
 //   tabBar.setMinimumHeight(20);
 //   tabBar.setMaximumHeight(20);

    browserList = new QList<NBrowserWindow *>();
    vboxlayout.addWidget(&tabBar);
    vboxlayout.addWidget(&stack);
    setLayout(&vboxlayout);

    connect(&tabBar, SIGNAL(currentChanged(int)),
                 &stack, SLOT(setCurrentIndex(int)));
    connect(&tabBar, SIGNAL(tabCloseRequested(int)),
                 this, SLOT(closeTab(int)));
    connect(&tabBar, SIGNAL(tabMoved(int, int)),
                 this, SLOT(moveTab(int, int)));
}

NTabWidget::~NTabWidget() {
    delete browserList;
}

void NTabWidget::addBrowser(NBrowserWindow *v, QString title) {
    tabBar.addTab(title);
    int index = stack.addWidget(v);
    stack.setCurrentIndex(index);
    browserList->append(v);
    tabBar.setCurrentIndex(index);
    tabBar.raise();

    connect(v, SIGNAL(noteUpdated(qint32)), this, SLOT(noteUpdateSignaled(qint32)));
    connect(v, SIGNAL(tagAdded(qint32)), this, SLOT(tagCreationSignaled(qint32)));

    if (browserList->size() <= 1)
        tabBar.setHidden(true);
    else
        tabBar.setHidden(false);
    return;
}

void NTabWidget::closeTab(int index) {
    tabBar.removeTab(index);
    stack.removeWidget(browserList->at(index));
    browserList->removeAt(index);
    if (browserList->size() <= 1)
        tabBar.setHidden(true);
    else
        tabBar.setHidden(false);
}



void NTabWidget::moveTab(int from, int to) {
    stack.removeWidget(browserList->at(from));
    stack.insertWidget(to, browserList->at(from));
    browserList->move(from, to);

}


NBrowserWindow* NTabWidget::currentBrowser() {
    return this->browserList->at(tabBar.currentIndex());
}


void NTabWidget::openNote(qint32 lid, bool newWindow) {

    // If the lid < 0, then we just clear it & get out
    if (lid < 0) {
        tabBar.setTabText(tabBar.currentIndex(), QString(""));
        currentBrowser()->setContent(-1, "");
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
            tabBar.setCurrentIndex(i);
            tabBar.raise();
            stack.setCurrentIndex(i);
            stack.raise();
            return;
        }
    }

    // if we want a new window AND it isn't already open, create a new tab
    if (newWindow && !found) {
        view = new NBrowserWindow();
        addBrowser(view, QString::fromStdString(n.title));
    } else {
        view = currentBrowser();
        tabBar.setTabText(tabBar.currentIndex(), QString::fromStdString(n.title));
    }

    view->lid = lid;

    if (!global.cache.contains(lid)) {
        NoteFormatter formatter;
        formatter.setNote(n, false);
        QByteArray content = formatter.rebuildNoteHTML();
        NoteCache *newCache = new NoteCache();
        newCache->noteContent = content;
        global.cache.insert(lid, newCache);
        view->setContent(lid, content);
    } else {
        NoteCache *c = global.cache[lid];
        view->setContent(lid, c->noteContent);
    }  
}


void NTabWidget::setTitle(QString t) {
    this->setTitle(tabBar.currentIndex(), t);
}

void NTabWidget::setTitle(int index, QString t) {
    tabBar.setTabText(index, t);
}


void NTabWidget::noteUpdateSignaled(qint32 lid) {
    emit(this->noteUpdated(lid));
}

void NTabWidget::tagCreationSignaled(qint32 lid) {
    emit(this->tagCreated(lid));
}

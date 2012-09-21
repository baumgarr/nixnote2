#include "nbrowserwindow.h"
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "gui/browserWidgets/urleditor.h"
#include "sql/tagtable.h"
#include "html/noteformatter.h"
#include "global.h"

#include <QVBoxLayout>
#include <QAction>
#include <QMenu>

extern Global global;

NBrowserWindow::NBrowserWindow(QWidget *parent) :
    QWidget(parent)
{

    QHBoxLayout *line1Layout = new QHBoxLayout();
    QVBoxLayout *layout = new QVBoxLayout();   // Note content layout

    // Setup line #1 of the window.  The text & notebook
    layout->addLayout(line1Layout);
    line1Layout->addWidget(&noteTitle);
    line1Layout->addWidget(&notebookMenu);
    line1Layout->addWidget(&expandButton);


    // Add the third layout display (which actually appears on line 2)
    layout->addLayout(&line3Layout);
    line3Layout.addWidget(&dateEditor);

    // Add the second layout display (which actually appears on line 3)
    layout->addLayout(&line2Layout);
    line2Layout.addWidget(&urlEditor,1);
    line2Layout.addWidget(&tagEditor, 3);



    // Setup the last line.  The actual note
    layout->addWidget(&editor);
    setLayout(layout);
    layout->setMargin(0);

    // Setup the signals
    connect(&expandButton, SIGNAL(stateChanged(int)), this, SLOT(changeExpandState(int)));
    connect(&notebookMenu, SIGNAL(notebookChanged()), this, SLOT(sendUpdateSignal()));
    connect(&urlEditor, SIGNAL(textUpdated()), this, SLOT(sendUpdateSignal()));
    connect(&noteTitle, SIGNAL(titleChanged()), this, SLOT(sendUpdateSignal()));
    connect(&dateEditor, SIGNAL(valueChanged()), this, SLOT(sendUpdateSignal()));
    connect(&tagEditor, SIGNAL(tagsUpdated()), this, SLOT(sendUpdateSignal()));
    connect(&tagEditor, SIGNAL(newTagCreated(qint32)), this, SLOT(newTagAdded(qint32)));
    lid = -1;
}


void NBrowserWindow::setContent(qint32 lid) {

    if (lid == -1)
        return;
    this->lid = lid;


    NoteTable noteTable;
    Note n;

    noteTable.get(n, this->lid, false, false);
    QByteArray content;

    if (!global.cache.contains(lid)) {
        NoteFormatter formatter;
        formatter.setNote(n, false);
        content = formatter.rebuildNoteHTML();
        NoteCache *newCache = new NoteCache();
        newCache->noteContent = content;
        global.cache.insert(lid, newCache);
    } else {
        NoteCache *c = global.cache[lid];
        content = c->noteContent;
    }


    noteTitle.setTitle(lid, QString::fromStdString(n.title), QString::fromStdString(n.title));
    dateEditor.setNote(lid, n);
    editor.setContent(content);

    // Set the tags
    tagEditor.clear();
    QStringList names;
    for (unsigned int i=0; i<n.tagNames.size(); i++) {
        names << QString::fromStdString(n.tagNames[i]);
    }
    tagEditor.setTags(names);
    tagEditor.setCurrentLid(lid);

    this->lid = lid;
    notebookMenu.setCurrentNotebook(lid, n);
    if (n.__isset.attributes && n.attributes.__isset.sourceURL)
        urlEditor.setUrl(lid, QString::fromStdString(n.attributes.sourceURL));
    else
        urlEditor.setUrl(lid, "");
}



void NBrowserWindow::changeExpandState(int value) {
    switch (value) {
    case EXPANDBUTTON_1:
        urlEditor.hide();
        tagEditor.hide();
        dateEditor.hide();
        break;
    case EXPANDBUTTON_2:
        urlEditor.show();
        tagEditor.show();
        break;
    case EXPANDBUTTON_3:
        dateEditor.show();
        break;
    }
}


void NBrowserWindow::sendUpdateSignal() {
    emit(this->noteUpdated(lid));
}

void NBrowserWindow::newTagAdded(qint32 lid) {
    emit(tagAdded(lid));
}

void NBrowserWindow::addTagName(qint32 lid) {
    TagTable table;
    Tag t;
    table.get(t, lid);
    tagEditor.addTag(QString::fromStdString(t.name));
}

void NBrowserWindow::tagRenamed(qint32 lid, QString oldName, QString newName) {
    tagEditor.tagRenamed(lid, oldName, newName);
}


void NBrowserWindow::tagDeleted(qint32 lid, QString name) {
    lid = lid;  /* suppress unused */
    tagEditor.removeTag(name);
}

void NBrowserWindow::notebookRenamed(qint32 lid, QString oldName, QString newName) {
    lid = lid;  /* suppress unused */
    oldName = oldName;  /* suppress unused */
    newName = newName;  /* suppress unused */
    notebookMenu.reloadData();
}

void NBrowserWindow::notebookDeleted(qint32 lid, QString name) {
    lid = lid;  /* suppress unused */
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}

void NBrowserWindow::stackRenamed(QString oldName, QString newName) {
    oldName = oldName;  /* suppress unused */
    newName = newName;  /* suppress unused */
    notebookMenu.reloadData();
}

void NBrowserWindow::stackDeleted(QString name) {
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}

void NBrowserWindow::stackAdded(QString name) {
    name=name;  /* suppress unused */
    notebookMenu.reloadData();
}


void NBrowserWindow::notebookAdded(qint32 lid) {
    lid = lid;  /* suppress unused */
    notebookMenu.reloadData();
}


void NBrowserWindow::noteSyncUpdate(qint32 lid) {
    if (lid != this->lid)
        return;
    setContent(lid);

}


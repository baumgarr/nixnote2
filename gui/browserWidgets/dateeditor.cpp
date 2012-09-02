#include "dateeditor.h"
#include "sql/notetable.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

DateEditor::DateEditor(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *vLayout = new QHBoxLayout();
    vLayout->setMargin(4);
    layout->setMargin(0);

    vLayout->addWidget(new QLabel(tr("Created:")));
    vLayout->addWidget(&createdDate);

    vLayout->addStretch(5);
    vLayout->addWidget(new QLabel(tr("Updated:")));
    vLayout->addWidget(&updatedDate);

    vLayout->addStretch(5);
    vLayout->addWidget(new QLabel(tr("Subject:")));
    vLayout->addWidget(&subjectDate);
    vLayout->addStretch(10);

    layout->addLayout(vLayout);
    layout->addWidget(&authorEditor);

    connect(&createdDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&updatedDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&subjectDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&createdDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&updatedDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&subjectDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&authorEditor, SIGNAL(textUpdated()), this, SLOT(emitChangedSignal()));

    setLayout(layout);
    hide();
}

void DateEditor::setNote(qint32 lid, Note n) {
    createdDate.setNote(lid, n.created, NOTE_CREATED_DATE);
    updatedDate.setNote(lid, n.updated, NOTE_UPDATED_DATE);
    if (n.__isset.attributes && n.attributes.__isset.subjectDate && n.attributes.subjectDate > 0)
        subjectDate.setNote(lid, n.attributes.subjectDate, NOTE_ATTRIBUTE_SUBJECT_DATE);
    else
        subjectDate.setNote(lid, n.updated, NOTE_ATTRIBUTE_SUBJECT_DATE);

    if (n.__isset.attributes && n.attributes.__isset.author && n.attributes.author != "")
        authorEditor.setAuthor(lid, QString::fromStdString(n.attributes.author));
    else
        authorEditor.setAuthor(lid, "");
}

void DateEditor::hideEvent(QHideEvent* event) {
    event=event;  // suppress unused
    createdDate.hide();
    subjectDate.hide();
    updatedDate.hide();
    authorEditor.hide();
}

void DateEditor::showEvent(QShowEvent* event) {
    event=event;  // suppress unused
    createdDate.show();
    subjectDate.show();
    updatedDate.show();
    authorEditor.show();
}

void DateEditor::emitChangedSignal() {
    emit(valueChanged());
}

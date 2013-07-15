/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

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
    QHBoxLayout *v2Layout = new QHBoxLayout();
    v2Layout->setMargin(4);
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
    v2Layout->addWidget(&authorEditor);
    v2Layout->addWidget(&locationEditor);
    layout->addLayout(v2Layout);

    connect(&createdDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&updatedDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&subjectDate, SIGNAL(dateChanged(QDate)), this, SLOT(emitChangedSignal()));
    connect(&createdDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&updatedDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&subjectDate, SIGNAL(timeChanged(QTime)), this, SLOT(emitChangedSignal()));
    connect(&authorEditor, SIGNAL(textUpdated()), this, SLOT(emitChangedSignal()));
    connect(&locationEditor, SIGNAL(textUpdated()), this, SLOT(emitChangedSignal()));
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

    if (n.__isset.attributes && n.attributes.__isset.longitude) {
        locationEditor.setGeography(lid, n.attributes.longitude,
        n.attributes.latitude,n.attributes.altitude);
    } else {
        locationEditor.setGeography(lid, 0,0,0);
    }
}

void DateEditor::hideEvent(QHideEvent* event) {
    event=event;  // suppress unused
    createdDate.hide();
    subjectDate.hide();
    updatedDate.hide();
    authorEditor.hide();
    locationEditor.hide();
}

void DateEditor::showEvent(QShowEvent* event) {
    event=event;  // suppress unused
    createdDate.show();
    subjectDate.show();
    updatedDate.show();
    authorEditor.show();
    locationEditor.show();
}

void DateEditor::emitChangedSignal() {
    emit(valueChanged());
}

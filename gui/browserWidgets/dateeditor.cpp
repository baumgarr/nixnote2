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
    connect(&locationEditor, SIGNAL(clicked()), this, SLOT(emitChangedSignal()));
    setLayout(layout);
    hide();
}

void DateEditor::setNote(qint32 lid, Note n) {
    createdDate.setNote(lid, n.created, NOTE_CREATED_DATE);
    updatedDate.setNote(lid, n.updated, NOTE_UPDATED_DATE);
    NoteAttributes attributes;
    if (n.attributes.isSet())
        attributes = n.attributes;
    if (attributes.subjectDate.isSet() && attributes.subjectDate > 0)
        subjectDate.setNote(lid, attributes.subjectDate, NOTE_ATTRIBUTE_SUBJECT_DATE);
    else
        subjectDate.setNote(lid, n.updated, NOTE_ATTRIBUTE_SUBJECT_DATE);

    QString author;
    if (attributes.author.isSet())
        author = attributes.author;
    authorEditor.setAuthor(lid, author.trimmed());

    double longitude=0, altitude=0, latitude=0;
    QString placeName = "";
    if (attributes.longitude.isSet())
        longitude = attributes.longitude;
    if (attributes.latitude.isSet())
        latitude = attributes.latitude;
    if (attributes.altitude.isSet())
        altitude = attributes.altitude;
    if (attributes.placeName.isSet())
        placeName = attributes.placeName;
    locationEditor.setGeography(lid, longitude,latitude, altitude, placeName);
}

void DateEditor::hideEvent(QHideEvent* event) {
    Q_UNUSED(event);  // suppress unused
    createdDate.hide();
    subjectDate.hide();
    updatedDate.hide();
    authorEditor.hide();
    locationEditor.hide();
}

void DateEditor::showEvent(QShowEvent* event) {
    Q_UNUSED(event);  // suppress unused
    createdDate.show();
    subjectDate.show();
    updatedDate.show();
    authorEditor.show();
    locationEditor.show();
}

void DateEditor::emitChangedSignal() {
    emit(valueChanged());
}

void DateEditor::setUpdateDate(qint64 value) {
    this->blockSignals(true);
    updatedDate.setDate(value);
    this->blockSignals(false);
}



void DateEditor::clear() {
    blockSignals(true);
    subjectDate.blockSignals(true);
    subjectDate.setDateTime(QDateTime::currentDateTime());
    subjectDate.blockSignals(false);
    updatedDate.blockSignals(true);
    updatedDate.setDateTime(QDateTime::currentDateTime());
    updatedDate.blockSignals(false);
    createdDate.blockSignals(true);
    createdDate.setDateTime(QDateTime::currentDateTime());
    createdDate.blockSignals(false);
    blockSignals(false);
}

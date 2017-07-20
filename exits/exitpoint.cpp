/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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


#include "exitpoint.h"

#include "global.h"

extern Global global;

ExitPoint::ExitPoint(QObject *parent) : QObject(parent)
{
    this->setExitName("");
    this->setEnabled(false);
    this->setVersion(1);
    this->setScript("");
}



void ExitPoint::setExitName(QString value) {
    exitName = value;
}

QString ExitPoint::getExitName() {
    return exitName;
}


void ExitPoint::setFileName(QString value) {
    fileName = value;
}

QString ExitPoint::getFileName() {
    return fileName;

}

void ExitPoint::setEnabled(bool value) {
    enabled = value;
}

bool ExitPoint::getEnabled() {
    return enabled;
}


void ExitPoint::setVersion(int value) {
    version=value;
}


int ExitPoint::getVersion() {
    return version;
}


QString ExitPoint::getScript() {
    return script;
}


void ExitPoint::setScript(QString value) {
    script = value;
}

void ExitPoint::log(QString value) {
    QLOG_INFO() << value;
}








ExitPoint_NoteEdit::ExitPoint_NoteEdit(ExitPoint *parent) : ExitPoint(parent) {
    exitReady = false;

    title = "";
    title_isSet = false;
    title_isModified = false;

    contents = "";
    contents_isSet = false;
    contents_isModified = false;
    contents_is_dirty = false;

    notebook = new QString("");
    notebook_isSet = false;
    notebook_isModified = false;

    tags.clear();
    tags_isSet = false;
    tags_isModified = false;

    updateDate_isSet = false;
    updateDate_isModified = false;
    subjectDate_isSet = false;
    subjectDate_isModified = false;
    creationDate_isModified = false;
    creationDate_isSet = false;
}


void ExitPoint_NoteEdit::setExitReady() {
    exitReady = true;
}

void ExitPoint_NoteEdit::setTitle(QString t) {
    if (exitReady)
        title_isModified = true;
    title_isSet = true;
    title= t;
}

QString ExitPoint_NoteEdit::getTitle() {
    return title;
}

bool ExitPoint_NoteEdit::isTitleModified() {
    return title_isModified;
}

void ExitPoint_NoteEdit::setContents(QString contents) {
    if (exitReady)
        contents_isModified = true;
    contents_isSet = true;
    this->contents = contents;
}

QString ExitPoint_NoteEdit::getContents() {
    return contents;
}


QString ExitPoint_NoteEdit::getContentsPlainText() {
    if (!contents_isSet)
        return "";
    QTextDocument doc;
    doc.setHtml(contents);
    return doc.toPlainText();
}


void ExitPoint_NoteEdit::setContentsDirty(bool value) {
    contents_is_dirty = value;
}


bool ExitPoint_NoteEdit::isContentsDirty() {
    return contents_is_dirty;
}

bool ExitPoint_NoteEdit::isContentsModified() {
    return contents_isModified;
}

void ExitPoint_NoteEdit::setNotebook(QString notebook) {
    if (exitReady)
        notebook_isModified = true;
    notebook_isSet = true;
    if (this->notebook != NULL)
        delete this->notebook;
    this->notebook = new QString(notebook);
}

QString ExitPoint_NoteEdit::getNotebook() {
    return *notebook;
}

bool ExitPoint_NoteEdit::isNotebookModified() {
    return notebook_isModified;
}

void ExitPoint_NoteEdit::setTags(QStringList tags) {
    this->tags.append(tags);
    if (exitReady)
        tags_isModified = true;
    tags_isSet = true;
}

QStringList ExitPoint_NoteEdit::getTags() {
    return tags;
}

bool ExitPoint_NoteEdit::isTagsModified() {
    return tags_isModified;
}


int ExitPoint_NoteEdit::getTagsSize() {
    return tags.size();
}

void ExitPoint_NoteEdit::insertTag(QString newTag) {
    if (!tags.contains(newTag, Qt::CaseInsensitive))
        tags.append(newTag);
    tags_isModified = true;
}

void ExitPoint_NoteEdit::removeTag(QString oldTag) {
    tags_isModified = true;
    tags.removeAll(oldTag);
}

QString ExitPoint_NoteEdit::getTagAt(int pos) {
    if (tags.size() < pos+1)
        return "";
    return tags.at(pos);
}



void ExitPoint_NoteEdit::setSubjectDate(qint64 msecs) {
    this->subjectDate = QDateTime::fromMSecsSinceEpoch(msecs);
    subjectDate_isSet = true;
    if (exitReady)
        subjectDate_isModified=true;
}

void ExitPoint_NoteEdit::setUpdatedDate(qint64 msecs) {
    this->updateDate = QDateTime::fromMSecsSinceEpoch(msecs);
    updateDate_isSet = true;
    if (exitReady)
        updateDate_isModified=true;
}

void ExitPoint_NoteEdit::setCreationDate(qint64 msecs) {
    this->creationDate = QDateTime::fromMSecsSinceEpoch(msecs);
    creationDate_isSet = true;
    if (exitReady)
        creationDate_isModified=true;
}

QString ExitPoint_NoteEdit::getCreatedDate(QString format) {
    if (creationDate_isSet)
        return creationDate.toString(format);
    else
        return 0;
}


QString ExitPoint_NoteEdit::getUpdatedDate(QString format) {
    if (updateDate_isSet)
        return updateDate.toString(format);
    else
        return 0;
}


QString ExitPoint_NoteEdit::getSubjectDate(QString format) {
    if (subjectDate_isSet)
        return subjectDate.toString(format);
    else
        return 0;
}

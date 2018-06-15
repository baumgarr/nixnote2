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

#ifndef FILTERENGINE_H
#define FILTERENGINE_H

#include <QObject>
#include "filtercriteria.h"

class FilterEngine : public QObject
{
    Q_OBJECT
private:
    void filterFavorite(FilterCriteria *criteria);
    void filterNotebook(FilterCriteria *criteria);
    void filterIndividualNotebook(QString &guid);
    void filterStack(QString& stack);
    void filterTags(FilterCriteria *criteria);
    void filterTrash(FilterCriteria *criteria);
    void filterAttributes(FilterCriteria *criteria);
    void filterSearchString(FilterCriteria *criteria);
    void filterSearchStringAll(QStringList list);
    void splitSearchTerms(QStringList &list, QString search);
    void filterSearchStringNotebookAll(QString string);
//    void filterSearchTodoAll(QStringList list);
    void filterSearchStringTodoAll(QString string);
    void filterSearchStringReminderOrderAll(QString string);
    void filterSearchStringReminderDoneTimeAll(QString string);
    void filterSearchStringReminderDoneTimeAny(QString string);
    void filterSearchStringReminderTimeAll(QString string);
    void filterSearchStringReminderTimeAny(QString string);
    void filterSearchStringTagAll(QString string);
    void filterSearchStringIntitleAll(QString searchStr);
    void filterSearchStringResourceAll(QString string);
    void filterSearchStringCoordinatesAll(QString string, int key);
    void filterSearchStringAuthorAll(QString string);
    void filterSearchStringSourceAll(QString string);
    void filterSearchStringSourceApplicationAll(QString string);
    void filterSearchStringContentClassAll(QString string);
    void filterSearchStringPlaceNameAll(QString string);
    void filterSearchStringResourceRecognitionTypeAll(QString string);
    QDateTime calculateDateTime(QString string);
    void filterSearchStringDateAll(QString string);

    void filterSearchStringAny(QStringList list);
    void filterSearchStringNotebookAny(QString string);
    void filterSearchStringTodoAny(QString string);
    void filterSearchStringReminderOrderAny(QString string);
    void filterSearchStringTagAny(QString string);
    void filterSearchStringIntitleAny(QString string);
    void filterSearchStringResourceAny(QString string);
    void filterSearchStringCoordinatesAny(QString string, int key);
    void filterSearchStringAuthorAny(QString string);
    void filterSearchStringDateAny(QString string);
    void filterSearchStringSourceAny(QString string);
    void filterSearchStringSourceApplicationAny(QString string);
    void filterSearchStringContentClassAny(QString string);
    void filterSearchStringResourceRecognitionTypeAny(QString string);
    bool anyFlagSet;

public:
    explicit FilterEngine(QObject *parent = 0);
    void filter(FilterCriteria *newCriteria=NULL, QList<qint32> *results=NULL);
    bool resourceContains(qint32 resourceLid, QString searchString, QStringList *returnHits);
    
signals:
    
public slots:
    
};

#endif // FILTERENGINE_H

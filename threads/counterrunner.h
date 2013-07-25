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

#ifndef COUNTERRUNNER_H
#define COUNTERRUNNER_H

#include <QThread>
#include "global.h"
#include <QPair>
#include <QList>
#include "sql/databaseconnection.h"

extern Global global;

class CounterRunner : public QObject
{
    Q_OBJECT
private:
    QList<QPair<qint32, qint32>*> *notebookCounts;
    QList<QPair<qint32, qint32>*> *tagCounts;
    qint32 trashCounts;
    DatabaseConnection *db;
    void initialize();
    bool init;

public:
    explicit CounterRunner(QObject *parent = 0);
    bool keepRunning;
    
signals:
    void trashTotals(qint32);
    void notebookTotals(qint32, qint32);
    void tagTotals(qint32, qint32);
    void tagCountComplete();
    
public slots:
    void countAll();
    void countTrash();
    void countNotebooks();
    void countTags();
    
};

#endif // COUNTERTHREAD_H

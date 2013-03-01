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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include "application.h"
#include <QSettings>

#include "logger/qslog.h"
#include "logger/qslogdest.h"
#include "settings/filemanager.h"
#include "settings/startupconfig.h"
#include "filters/filtercriteria.h"
#include "models/notecache.h"
#include "gui/shortcutkeys.h"
#include "settings/accountsmanager.h"

//#include "java/javamachine.h"

#include <string>
#include <QSharedMemory>

//*******************************
//* This class is used to store
//* global values across the
//* program.
//*******************************

#define NOTE_TABLE_LID_POSITION 0
#define NOTE_TABLE_DATE_CREATED_POSITION 1
#define NOTE_TABLE_DATE_UPDATED_POSITION 2
#define NOTE_TABLE_TITLE_POSITION 3
#define NOTE_TABLE_NOTEBOOK_LID_POSITION 4
#define NOTE_TABLE_NOTEBOOK_POSITION 5
#define NOTE_TABLE_TAGS_POSITION 6

#define NOTE_TABLE_AUTHOR_POSITION 7
#define NOTE_TABLE_DATE_SUBJECT_POSITION 8
#define NOTE_TABLE_DATE_DELETED_POSITION 9
#define NOTE_TABLE_SOURCE_POSITION 10
#define NOTE_TABLE_SOURCE_URL_POSITION 11
#define NOTE_TABLE_SOURCE_APPLICATION_POSITION 12
#define NOTE_TABLE_LATITUDE_POSITION 13
#define NOTE_TABLE_LONGITUDE_POSITION 14
#define NOTE_TABLE_ALTITUDE_POSITION 15
#define NOTE_TABLE_HAS_ENCRYPTION_POSITION 16
#define NOTE_TABLE_HAS_TODO_POSITION 17
#define NOTE_TABLE_IS_DIRTY_POSITION 18
#define NOTE_TABLE_SIZE_POSITION 19

#define NOTE_TABLE_COLUMN_COUNT 20


#define MINIMUM_RECOGNITION_WEIGHT 40

using namespace std;

class Global
{
public:
    bool saveInProgress;
    Global();           // Generic constructor
    ~Global();          // destructor

    enum ListViewSetup {
        ListViewWide = 1,
        listViewNarrow = 2
    };

    ListViewSetup listView;

    int argc;           // Initial argument count from the program start
    char** argv;        // List of arguments from the program start
    FileManager fileManager;
    AccountsManager *accountsManager;\
    Application *application;
    unsigned int cryptCounter;
    QString attachmentNameDelimeter;
    string username;
    string password;
    bool connected;
    QSharedMemory *sharedMemory;
    bool confirmDeletes();
    QString tagBehavior();
    QString server;
    QSettings *settings;
    QSettings *globalSettings;
    QClipboard *clipboard;
    ShortcutKeys *shortcutKeys;
    QList<qint32> expungedResources;
    bool showTrayIcon();
    bool closeToTray();
    bool minimizeToTray();
    void setMinimizeToTray(bool value);
    void setCloseToTray(bool value);
    void setColumnPosition(QString col, int position);
    void setColumnWidth(QString col, int width);
    int getColumnPosition(QString col);
    int getColumnWidth(QString col);
    //JavaMachine jvm;

    // Filter criteria.  Used for things like the back & forward buttons
    QList<FilterCriteria*> filterCriteria;
    qint32 filterPosition;

    // Note cache
    QHash<int, NoteCache*> cache;

    void setup(StartupConfig config);  // Setup the global variables
    QString getProgramDirPath();      // Get the path the program is executing from
    QList< QPair<int, QString> > passwordRemember;   // Cache of passwords
    void appendFilter(FilterCriteria *criteria);
};

bool caseInsensitiveLessThan(const QString &s1, const QString &s2);

#endif // GLOBAL_H

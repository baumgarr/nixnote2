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
#include "reminders/remindermanager.h"

#include <string>
#include <QSharedMemory>
#include <QSqlDatabase>

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
#define NOTE_TABLE_REMINDER_ORDER_POSITION 20
#define NOTE_TABLE_REMINDER_TIME_POSITION 21
#define NOTE_TABLE_REMINDER_TIME_DONE_POSITION 22
#define NOTE_TABLE_PINNED_POSITION 23
#define NOTE_TABLE_COLOR_POSITION 24
#define NOTE_TABLE_THUMBNAIL_POSITION 25

#define NOTE_TABLE_COLUMN_COUNT 26

using namespace std;

class Global
{
public:
    Global();           // Generic constructor
    ~Global();          // destructor

    // Possible ways tags & notebook counts may be displayed to the user
    enum CountBehavior {
        CountAll = 1,
        CountNone = 2
    };
    CountBehavior countBehavior;   // How does the user want tags/notebooks to be counted.

    bool disableUploads;           // Should we disable all uploads to Evernote?  Useful for testing.

    // Valid values for the note list appearance.  Should it be a narrow or wide list
    enum ListViewSetup {
        ListViewWide = 1,
        listViewNarrow = 2
    };
    ListViewSetup listView;    // Current desired note list value

    int argc;                  // Initial argument count from the program start
    char** argv;               // List of arguments from the program start
    FileManager fileManager;   // Manage file paths
    AccountsManager *accountsManager;      // Manage user account
    Application *application;              // pointer to this current application
    unsigned int cryptCounter;             // Count of crytpographic entries.  This is incremented each time we encrypt some text.
    QString attachmentNameDelimeter;       // Delimeter between attachment ID & name
    string username;                       // This is probably obsolete
    string password;                       // This is probably obsolete
    bool connected;                        // Are we currently connected to Evernote?
    bool disableIndexing;                  // Stop indexing
    bool pdfPreview;                       // Should we view PDFs inline?
    bool showGoodSyncMessagesInTray;       // Should we show good sync messages in the tray, or just errors?
    QSharedMemory *sharedMemory;           // Shared memory key.  Useful to prevent multiple instances and for cross memory communication
    bool confirmDeletes();                 // Should we confirm deletes?
    QString tagBehavior();                 // Should inactive tags be shown?
    QString server;                        // Evernote server to sync with
    QSettings *settings;                   // Pointer to the nixnote config file.  There is a different one for each account.
    QSettings *globalSettings;             // Pointer to all the config file that is common to all accounts.
    QClipboard *clipboard;                 // System clipboard pointer
    ShortcutKeys *shortcutKeys;            // Keyboard shortcuts defined by the user
    QList<qint32> expungedResources;       // List of expunged resource LIDs
    QFileSystemWatcher resourceWatcher;    // Watcher for file system directories.  New files here will create anote
    bool showTrayIcon();                   // Should we show the tray icon?
    bool closeToTray();                    // Close it to the tray.  We really just hide it.
    bool minimizeToTray();                 // Minimize it to tray rather than the task list.  We really just hide it.
    void setMinimizeToTray(bool value);    // Set if we should minimize it to the tray
    void setCloseToTray(bool value);       // Set if we should close it to the tray
    void setColumnPosition(QString col, int position);    // Save the order of a  note list's column.
    void setColumnWidth(QString col, int width);          // Save the width of a note list column
    int getColumnPosition(QString col);                   // Get the desired position of a note column
    int getColumnWidth(QString col);                      // Get the desired width o a note column.
    int getMinimumRecognitionWeight();                    // Minimum OCR recognition confidence before including it in search results.
    void setSynchronizeAttachments(bool value);           // Should at
    bool synchronizeAttachments();                        // This is probabably obsolete
    qlonglong getLastReminderTime();                      // Get the last time we actually showed a user note reminders.
    void setLastReminderTime(qlonglong value);            // Save the date/time we last showed a user note reminders.
    void setMinimumRecognitionWeight(int weight);         // Set the minimum OCR recgnition confidence before including it in search results.
    QString dateFormat;                                   // Desired display date format
    QString timeFormat;                                   // Desired display time format
    QSqlDatabase *db;                                     // "default" DB connection for the main thread.
    bool javaFound;                                       // Have we found Java?
    QString defaultFont;                                  // Default editor font name
    int defaultFontSize;                                  // Default editor font size
    int defaultGuiFontSize;                               // Default GUI font size
    QString defaultGuiFont;                               // Default GUI font name
    bool startupNewNote;                                  // Were we started with the command to begin a new note?
    bool forceNoStartMimized;                             // Force the system to not start minimized, dispite the user's settings
    bool forceStartMinimized;                             // Force it to start minimized, despiet the user's settings
    bool startMinimized;                                  // Do user prefernces say to start minimized?
    bool syncAndExit;                                     // Should we just start, do a sync, and then quit?
    qint32 startupNote;                                   // Initial note to startup with.

    qint32 minIndexInterval;                              // Minimum interval to check for any unindexed notes.
    qint32 maxIndexInterval;                              // Maximum interval to check for any unindexed notes.
    qint32 indexResourceCountPause;                       // After indexing this many resources we pause to avoid overloading the CPU
    qint32 indexNoteCountPause;                           // After indexing this many notes we pause to avoid overloading the CPU

    // Filter criteria.  Used for things like the back & forward buttons
    QList<FilterCriteria*> filterCriteria;
    qint32 filterPosition;

    QHash<int, NoteCache*> cache;                         // Note cache  used to keep from needing to re-format the same note for a display

    void setup(StartupConfig config);                         // Setup the global variables
    QString getProgramDirPath();                              // Get the path the program is executing from
    QList< QPair<QString, QString> > passwordRemember;        // Cache of passwords
    QHash< QString, QPair <QString, QString> > passwordSafe;  // Saved passwords
    void appendFilter(FilterCriteria *criteria);
    void setupDateTimeFormat();                               // Setup the user's desired date & time format
    QFont getGuiFont(QFont f);                                // Get the user's desired GUI font

    bool nonAsciiSortBug;                                     // Workaround for non-ASCII characters in tag name sorting

    ReminderManager *reminderManager;                         // Used to alert the user when a reminder time has expired


    // These functions deal with the icon themes
    QHash<QString,QString> resourceList;                      // Hashmap of icons used in the current theme
    QPixmap getPixmapResource(QHash<QString, QString> &resourceList, QString key);   // Get a pixmap from the user's (or default) theme
    QPixmap getPixmapResource(QString key);                   // Get a pixmap from the user's (or default) theme
    QIcon getIconResource(QHash<QString, QString> &resourceList, QString key);       // Get an icon from the user's (or default) theme
    QIcon getIconResource(QString key);                       // Get an icon from the user's (or default) theme
    void loadTheme(QHash<QString, QString> &resourceList, QString themeName);   // Load an icon theme into the resourceList
    void loadThemeFile(QFile &file, QString themeName);       // Load a given theme's values from a a file.
    void loadThemeFile(QHash<QString, QString> &resourceList, QFile &file, QString themeName);    // Load a given theme's values from a file
    QStringList getThemeNames();                               // Get a list of all available theme names
    QString getResourceFileName(QHash<QString, QString> &resourceList, QString key);    // Get the actual file path for a given icon theme
    QString getResourcefileName(QString key);                  // Get the actual file path for a given icon theme
    void getThemeNamesFromFile(QFile &file, QStringList &values);  // Get all themes available in a given file
};

bool caseInsensitiveLessThan(const QString &s1, const QString &s2);         // Helper function to sort values case-insensitive.

#endif // GLOBAL_H

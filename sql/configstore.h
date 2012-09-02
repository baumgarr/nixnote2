#ifndef CONFIGSTORE_H
#define CONFIGSTORE_H

#include <QtSql>

//*************************************
//* This table is used to store
//* config values & things about how
//* to run the program.
//*************************************

// Define key types
#define CONFIG_STORE_LID 0   // This is the highest number object in the database
#define CONFIG_STORE_WINDOW_GEOMETRY 1 // The window geometry between runs
#define CONFIG_STORE_WINDOW_STATE 2 // The window state between runs

// Class used to access & update the table
class ConfigStore
{
private:
    void initTable();           // Initialize a new table

public:
    ConfigStore();              // Generic constructor
    void createTable();         // SQL to create the table
    qint32 incrementLidCounter();  // Get the next LID number
    void saveSetting(int key, QByteArray);  // Save a setting
    bool getSetting(QByteArray &value, int key);  // retrieve a setting
};

#endif // CONFIGSTORE_H

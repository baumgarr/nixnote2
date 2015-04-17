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

#ifndef ACCOUNTSMANAGER_H
#define ACCOUNTSMANAGER_H

#include <QObject>
#include <QList>
#include <QtXml>



//********************************************************
//********************************************************
//* This class manages the accounts.conf file.  It
//* stores the oauth key, server URL, consumer key, and
//* the unique ID of each account.
//********************************************************
//********************************************************

class AccountsManager : public QObject
{
    Q_OBJECT
private:
    QDomDocument doc;                                       // XML dom doc to parse out accounts.conf
    QDomNode currentNode;                                   // node to parse out accounts.conf
    void save();                                            // Save accounts.conf file.

public:
    explicit AccountsManager(int id, QObject *parent = 0);   // Constructor
    int currentId;                                           // ID currently in use
    QString configFile;                                      // Configuration (accounts.conf) file
    bool oauthTokenFound();                                  // Do we have a good oauth token?
    QList<int> idList();                                     // List of all IDs
    int addId(int id, QString name, QString oauth="", QString server="");        // Add a new account
    int getNewIdNumber();                                    // Increment the ID number (used for a new account)
    QStringList nameList();                                  // Get a list of account names
    QString getOAuthToken();                                 // Get the oauth token for the current account
    void setOAuthToken(QString token);                       // Set the oauth token for the current account
    void setOAuthToken(int id, QString token);               // set oauth token for a specific account
    void setName(QString name, int id=0);                    // Set the name of a specificaccount
    QString getServer();                                     // Get the URL of the server for the current account
    QString getName();                                       // Get the name of the current account in use
    bool removeId(int id);                                   // Delete an account

signals:
    
public slots:
    
};

#endif // ACCOUNTSMANAGER_H

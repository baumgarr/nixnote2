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

class AccountsManager : public QObject
{
    Q_OBJECT
private:
    QDomDocument doc;
    QDomNode currentNode;
    void save();

public:
    explicit AccountsManager(int id, QObject *parent = 0);
    int currentId;
    QString configFile;
    bool oauthTokenFound();
    QList<int> idList();
    int addId(int id, QString name, QString oauth="", QString server="");
    int getNewIdNumber();
    QStringList nameList();
    QString getOAuthToken();
    void setOAuthToken(QString token);
    void setOAuthToken(int id, QString token);
    void setName(QString name, int id=0);
    QString getServer();
    QString getName();
    bool removeId(int id);

signals:
    
public slots:
    
};

#endif // ACCOUNTSMANAGER_H

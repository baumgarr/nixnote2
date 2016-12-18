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

#ifndef SYNCPREFERENCES_H
#define SYNCPREFERENCES_H

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

class SyncPreferences : public QWidget
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;
    QComboBox *syncInterval;
    QCheckBox *syncOnStartup;
    QCheckBox *syncAutomatically;
    QCheckBox *syncOnShutdown;
    QCheckBox *apiRateRestart;
    QCheckBox *popupOnSyncError;

    QCheckBox *enableProxy;
    QCheckBox *enableSocks5;
    QLineEdit *userId;
    QLineEdit *password;
    QLineEdit *port;
    QLineEdit *host;

    int getSyncInterval();

public:
    explicit SyncPreferences(QWidget *parent = 0);
    ~SyncPreferences();
    void saveValues();
    QCheckBox *enableSyncNotifications;
    QCheckBox *showGoodSyncMessagesInTray;
    
signals:
    
public slots:

private slots:
    void enableSyncStateChange();
    void enableSuccessfulSyncMessagesInTray();
    void proxyCheckboxAltered(int state);
    
};

#endif // SYNCPREFERENCES_H

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

#ifndef POPPLERFACTORY_H
#define POPPLERFACTORY_H

#include <QWebPluginFactory>
#include <QNetworkAccessManager>
#include <poppler-qt4.h>
#include "gui/plugins/popplerviewer.h"
#include <QWebPage>

class QNetworkAccessManager;
class QNetworkReply;

class PluginFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    explicit PluginFactory(QObject *parent = 0);
    QObject *create(const QString &mimeType, const QUrl &url,
                    const QStringList &argumentNames,
                    const QStringList &argumentValues) const;
    QList<QWebPluginFactory::Plugin> plugins() const;

private:
    QNetworkAccessManager *manager;
    QWebPage *editor;
    
signals:
    
public slots:
    
};

#endif // POPPLERFACTORY_H

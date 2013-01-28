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

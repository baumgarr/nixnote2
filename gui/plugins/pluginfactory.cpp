#include "gui/plugins/pluginfactory.h"
#include "gui/plugins/popplerviewer.h"
#include <QNetworkRequest>
#include <QNetworkReply>

PluginFactory::PluginFactory(QObject *parent) :
    QWebPluginFactory(parent)
{
    manager = new QNetworkAccessManager(this);
}

QList<QWebPluginFactory::Plugin> PluginFactory::plugins() const
{
    QWebPluginFactory::MimeType mimeType;
    mimeType.name = "application/pdf";
    mimeType.description = "PDF Viewer";
    mimeType.fileExtensions = QStringList() << "pdf";

    QWebPluginFactory::Plugin plugin;
    plugin.name = "PDF file viewer";
    plugin.description = "A CSV file Web plugin.";
    plugin.mimeTypes = QList<MimeType>() << mimeType;

    return QList<QWebPluginFactory::Plugin>() << plugin;
}


QObject *PluginFactory::create(const QString &mimeType, const QUrl &url,
                            const QStringList &argumentNames,
                            const QStringList &argumentValues) const
{
    if (mimeType == "application/pdf") {
        PopplerViewer *view;
        view = new PopplerViewer(argumentValues[argumentNames.indexOf("type")],
                argumentValues[argumentNames.indexOf("lid")]);

        //QNetworkRequest request(url);
        //QNetworkReply *reply = manager->get(request);
        //connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));

        if (view->doc != NULL)
            return (QObject*)view;
        else
            return 0;
    }
    return 0;
}

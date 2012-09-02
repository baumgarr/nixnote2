#include "thumbnailer.h"
#include <QtWebKit>
#include <QtSql>
#include "global.h"
#include "html/noteformatter.h"


/* Generic constructor. */
Thumbnailer::Thumbnailer()
{
    connect(&page, SIGNAL(loadFinished(bool)),
        this, SLOT(render()));
}


/* Set the note we should generate the thumbnail for */
void Thumbnailer::setNote(qint32 lid, Note n) {
    noteLid = lid;
    NoteFormatter formatter;
    formatter.setNote(n, false);
    formatter.rebuildNoteHTML();
    page.mainFrame()->setHtml(formatter.getPage());
}


/* Do the actual thumbnail once the load has finished */
void Thumbnailer::render()
{
    // Remove the encrypted tags
    QWebElement e = page.mainFrame()->documentElement();
    QWebElementCollection collection = e.findAll("en-crypt");
    foreach (QWebElement paraElement, collection) {
        paraElement.removeFromDocument();
    }

    QString text = page.mainFrame()->toPlainText();
    QSqlQuery sql;
    sql.prepare("Insert into SearchIndex (lid, weight, content) values (:lid, :weight, :content)");
    sql.bindValue(":lid", noteLid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", text);
    sql.exec();
    page.setViewportSize(page.mainFrame()->contentsSize());
    //QImage image(page.viewportSize(), QImage::Format_ARGB32);
    //QPainter painter(&image);

    //page.mainFrame()->render(&painter);
    //painter.end();

    //QImage thumbnail = image.scaled(400, 400);
    //thumbnail.save("thumbnail.png");

    emit finished();
}

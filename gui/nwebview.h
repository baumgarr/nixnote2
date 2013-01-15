#ifndef NWEBVIEW_H
#define NWEBVIEW_H

#include <QWebView>
#include <gui/nwebpage.h>
#include <QShortcut>
#include <QMenu>
#include <QAction>

#include "gui/nbrowserwindow.h"

class NBrowserWindow;

//****************************************************
//* This inherits everything from QWebView.  It is
//* useful to allow us to setup things that are
//* needed to view & edit notes.
//****************************************************
class NWebView : public QWebView
{
    Q_OBJECT
private:
    QAction *setupColorMenuOption(QString color);

public:
    explicit NWebView(NBrowserWindow *parent = 0);
    NBrowserWindow *parent;
    QMenu *contextMenu;
    QMenu *tableMenu;
    QMenu *imageMenu;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteWithoutFormatAction;
    QAction *todoAction;
    QAction *encryptAction;
    QAction *downloadAttachmentAction;
    QAction *downloadImageAction;
    QAction *rotateImageRightAction;
    QAction *rotateImageLeftAction;
    QAction *insertLinkAction;
    QAction *insertLatexAction;
    QAction *insertTableAction;
    QAction *insertTableRowAction;
    QAction *insertTableColumnAction;
    QAction *deleteTableColumnAction;
    QAction *deleteTableRowAction;
    QAction *openAction;
    QAction *insertQuickLinkAction;
    bool isDirty;
    NWebPage *editorPage;
    bool event(QEvent *event);
    void keyPressEvent(QKeyEvent *);
    void downloadAttachment(QNetworkRequest *req);

signals:
    void noteChanged();
    void downloadAttachmentRequested(QNetworkRequest*);

public slots:
    void editAlert();
    void decryptText(QString id, QString text, QString hint);
    void contextMenuEvent(QContextMenuEvent *event);
    void setBackgroundWhite();
    void setBackgroundRed();
    void setBackgroundBlue();
    void setBackgroundGreen();
    void setBackgroundYellow();
    void setBackgroundBlack();
    void setBackgroundPurple();
    void setBackgroundBrown();
    void setBackgroundGrey();
    void setBackgroundOrange();
    void setBackgroundPowderBlue();
    void printNodeName(QString s);

private slots:
    void exposeToJavascript();


};

#endif // NWEBVIEW_H

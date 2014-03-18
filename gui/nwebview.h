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

#ifndef NWEBVIEW_H
#define NWEBVIEW_H

#include <QWebView>
#include <gui/nwebpage.h>
#include <QShortcut>
#include <QMenu>
#include <QNetworkRequest>
#include <QAction>

#include "gui/nbrowserwindow.h"
#include "gui/browserWidgets/ntitleeditor.h"

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
    void setupShortcut(QAction *action, QString text);

public:
    explicit NWebView(NBrowserWindow *parent = 0);
    NBrowserWindow *parent;
    NTitleEditor *titleEditor;
    QMenu *contextMenu;
    QMenu *tableMenu;
    QMenu *imageMenu;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *removeFormattingAction;
    QAction *pasteWithoutFormatAction;
    QAction *todoAction;
    QAction *encryptAction;
    QAction *downloadAttachmentAction();
    QAction *downloadImageAction();
    QAction *rotateImageRightAction;
    QAction *rotateImageLeftAction;
    QAction *insertLinkAction;
    QAction *insertLatexAction;
    QAction *attachFileAction;
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
    void focusLostNotify(QString text);

signals:
    void noteChanged();
    void downloadAttachmentRequested(QNetworkRequest*);
    void htmlEditAlert();

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
    void setTitleEditor(NTitleEditor *editor);
    void focusOutEvent(QFocusEvent *e);

private slots:
    void exposeToJavascript();
    void setDefaultTitle();
    void downloadRequested(QNetworkRequest);


};

#endif // NWEBVIEW_H

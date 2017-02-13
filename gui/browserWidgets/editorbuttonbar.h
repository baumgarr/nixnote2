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




#ifndef EDITORBUTTONBAR_H
#define EDITORBUTTONBAR_H
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QComboBox>
#include <QShortcut>

#include "gui/browserWidgets/colormenu.h"
#include "gui/browserWidgets/fontnamecombobox.h"
#include "gui/browserWidgets/fontsizecombobox.h"


class EditorButtonBar : public QToolBar
{
    Q_OBJECT
public:
    QMenu *contextMenu;
    QAction *undoVisible;
    QAction *redoVisible;
    QAction *cutVisible;
    QAction *copyVisible;
    QAction *pasteVisible;
    QAction *removeFormatVisible;
    QAction *boldVisible;
    QAction *italicVisible;
    QAction *underlineVisible;
    QAction *strikethroughVisible;
    QAction *insertDatetimeVisible;
    QAction *leftJustifyVisible;
    QAction *centerJustifyVisible;
    QAction *fullJustifyVisible;
    QAction *rightJustifyVisible;
    QAction *hlineVisible;
    QAction *shiftRightVisible;
    QAction *shiftLeftVisible;
    QAction *bulletListVisible;
    QAction *numberListVisible;
    QAction *fontVisible;
    QAction *fontSizeVisible;
    QAction *todoVisible;
    QAction *highlightVisible;
    QAction *fontColorVisible;
    QAction *insertTableButtonVisible;
    QAction *spellCheckButtonVisible;
    QAction *htmlEntitiesButtonVisible;
    QAction *subscriptVisible;
    QAction *superscriptVisible;

    QAction *removeFormatButtonAction;
    QShortcut * removeFormatButtonShortcut;
    QAction *undoButtonAction;
    QShortcut *undoButtonShortcut;
    QAction *redoButtonAction;
    QShortcut *redoButtonShortcut;
    QAction *cutButtonAction;
    QShortcut *cutButtonShortcut;
    QAction *copyButtonAction;
    QShortcut *copyButtonShortcut;
    QAction *pasteButtonAction;
    // QShortcut *pasteButtonShortcut;  Captured van NWebView
    QAction *boldButtonAction;
    QShortcut *boldButtonShortcut;
    QAction *italicButtonAction;
    QShortcut *italicButtonShortcut;
    QAction *underlineButtonAction;
    QShortcut *underlineButtonShortcut;
    QAction *strikethroughButtonAction;
    QShortcut *strikethroughButtonShortcut;
    QAction *subscriptButtonAction;
    QShortcut *subscriptButtonShortcut;
    QAction *superscriptButtonAction;
    QShortcut *superscriptButtonShortcut;
    QAction *insertDatetimeButtonAction;
    QShortcut *insertDatetimeButtonShortcut;
    QAction *leftJustifyButtonAction;
    QShortcut *leftJustifyButtonShortcut;
    QAction *centerJustifyButtonAction;
    QShortcut *centerJustifyButtonShortcut;
    QAction *fullJustifyButtonAction;
    QShortcut *fullJustifyButtonShortcut;
    QAction *rightJustifyButtonAction;
    QShortcut *rightJustifyButtonShortcut;
    QAction *hlineButtonAction;
    QShortcut *hlineButtonShortcut;
    QAction *shiftRightButtonAction;
    QShortcut *shiftRightButtonShortcut;
    QAction *shiftLeftButtonAction;
    QShortcut *shiftLeftButtonShortcut;
    QAction *bulletListButtonAction;
    QShortcut *bulletListButtonShortcut;
    QAction *numberListButtonAction;
    QShortcut *numberListButtonShortcut;
    QAction *highlightColorAction;
    QShortcut *highlightColorShortcut;
    QAction *fontColorAction;
    QShortcut *fontColorShortcut;
    QAction *todoButtonAction;
    QShortcut *todoButtonShortcut;
    QAction *fontButtonAction;
    QAction *fontSizeButtonAction;
    QAction *insertTableButtonAction;
    QShortcut *insertTableButtonShortcut;
    QAction *spellCheckButtonAction;
    QShortcut *spellCheckButtonShortcut;
    QAction *htmlEntitiesButtonAction;
    QShortcut *htmlEntitiesButtonShortcut;

    FontNameComboBox *fontNames;
    FontSizeComboBox *fontSizes;

    QToolButton *boldButtonWidget;
    QToolButton *italicButtonWidget;
    QToolButton *underlineButtonWidget;
    QToolButton *insertDatetimeButtonWidget;
    QToolButton *fontColorButtonWidget;
    ColorMenu *fontColorMenuWidget;
    QToolButton *highlightColorButtonWidget;
    ColorMenu *highlightColorMenuWidget;
//    void setupShortcut(QToolButton *toolbutton, QString text);
//    void setupShortcut(QAction *action, QString text);
    void setupShortcut(QShortcut *shortcut, QString text);

    explicit EditorButtonBar(QWidget *parent = 0);
    ~EditorButtonBar();
    void contextMenuEvent(QContextMenuEvent *event);
    void saveVisibleButtons();
    void setupVisibleButtons();
    void reloadIcons();

signals:
    
public slots:
    void toggleUndoButtonVisible();
    void toggleRedoButtonVisible();
    void toggleCutButtonVisible();
    void toggleCopyButtonVisible();
    void togglePasteButtonVisible();
    void toggleBoldButtonVisible();
    void toggleItalicButtonVisible();
    void toggleUnderlineButtonVisible();
    void toggleStrikethroughButtonVisible();
    void toggleSuperscriptButtonVisible();
    void toggleSubscriptButtonVisible();
    void toggleInsertDatetimeVisible();
    void toggleLeftJustifyButtonVisible();
    void toggleCenterJustifyButtonVisible();
    void toggleFullJustifyButtonVisible();
    void toggleRightJustifyButtonVisible();
    void toggleHlineButtonVisible();
    void toggleShiftRightButtonVisible();
    void toggleShiftLeftButtonVisible();
    void toggleBulletListButtonVisible();
    void toggleNumberListButtonVisible();
    void toggleFontButtonVisible();
    void toggleTodoButtonVisible();
    void toggleFontSizeButtonVisible();
    void toggleSpellCheckButtonVisible();
    void toggleFontColorVisible();
    void toggleRemoveFormatVisible();
    void toggleHighlightColorVisible();
    void toggleInsertTableButtonVisible();
    void toggleHtmlEntitiesButtonVisible();
    void loadFontSizeComboBox(QString font);
    void loadFontNames();
    
};

#endif // EDITORBUTTONBAR_H

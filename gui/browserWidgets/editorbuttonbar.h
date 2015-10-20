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
    QAction *rightJustifyVisible;
    QAction *hlineVisible;
    QAction *shiftRightVisible;
    QAction *shiftLeftVisible;
    QAction *buttonListVisible;
    QAction *numberListVisible;
    QAction *fontVisible;
    QAction *fontSizeVisible;
    QAction *todoVisible;
    QAction *highlightVisible;
    QAction *fontColorVisible;
    QAction *insertTableButtonVisible;

    QAction *removeFormatButtonAction;
    QAction *undoButtonAction;
    QAction *redoButtonAction;
    QAction *cutButtonAction;
    QAction *copyButtonAction;
    QAction *pasteButtonAction;
    QAction *boldButtonAction;
    QAction *italicButtonAction;
    QAction *underlineButtonAction;
    QAction *strikethroughButtonAction;
    QAction *insertDatetimeButtonAction;
    QAction *leftJustifyButtonAction;
    QAction *centerJustifyButtonAction;
    QAction *rightJustifyButtonAction;
    QAction *hlineButtonAction;
    QAction *shiftRightButtonAction;
    QAction *shiftLeftButtonAction;
    QAction *bulletListButtonAction;
    QAction *numberListButtonAction;
    QAction *highlightColorAction;
    QAction *fontColorAction;
    QAction *todoButtonAction;
    QAction *fontButtonAction;
    QAction *fontSizeButtonAction;
    QAction *insertTableButtonAction;
    QAction *spellCheckButtonAction;
    QAction *spellCheckButtonVisible;
    QAction *htmlEntitiesButtonAction;
    QAction *htmlEntitiesButtonVisible;
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
    void setupShortcut(QToolButton *action, QString text);
    void setupShortcut(QAction *action, QString text);

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
    void toggleInsertDatetimeVisible();
    void toggleLeftJustifyButtonVisible();
    void toggleCenterJustifyButtonVisible();
    void toggleRightJustifyButtonVisible();
    void toggleHlineButtonVisible();
    void toggleShiftRightButtonVisible();
    void toggleShiftLeftButtonVisible();
    void toggleButtonListButtonVisible();
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

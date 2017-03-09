#ifndef APPEARANCEPREFERENCES_H
#define APPEARANCEPREFERENCES_H

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
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

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>

class AppearancePreferences : public QWidget
{
    Q_OBJECT

private:
    QGridLayout *mainLayout;
    QComboBox *defaultNotebookOnStartup;
    QComboBox *windowThemeChooser;
    QComboBox *defaultFontChooser;
    QComboBox *defaultFontSizeChooser;
    QComboBox *defaultGuiFontSizeChooser;
    QComboBox *defaultGuiFontChooser;
    QCheckBox *autoHideEditorButtonbar;
    QComboBox *traySingleClickAction;
    QComboBox *trayMiddleClickAction;
    QComboBox *trayDoubleClickAction;
    QComboBox *mouseMiddleClickAction;
    QComboBox *mouseDoubleClickAction;
    QComboBox *systemNotifier;
    QCheckBox *autosetUserid;
    QCheckBox *fontPreviewInDialog;
    QLabel *defaultNotebookOnStartupLabel;
    QPushButton *previewButton;
    bool webSettingsChanged;
    QScrollArea *scrollArea;
    void loadFontNames(QComboBox *combo, QString defaultFont);


public:
    explicit AppearancePreferences(QWidget *parent = 0);
    void saveValues();
    QCheckBox *showTrayIcon;
    QCheckBox *showPDFs;
    QCheckBox *showSplashScreen;
    QCheckBox *showMissedReminders;
    QCheckBox *autoStart;
    QCheckBox *startMinimized;
    QCheckBox *minimizeToTray;
    QCheckBox *confirmDeletes;
    QCheckBox *closeToTray;
    QCheckBox *forceWebFonts;
    QCheckBox *dynamicTotals;
    QCheckBox *disableEditingOnStartup;
    QCheckBox *newNoteFocusOnTitle;
    QCheckBox *showNoteListGrid;
    QCheckBox *alternateNoteListColors;

    enum DefaultNotebook {
        UseLastViewedNotebook = 0,
        UseDefaultNotebook = 1,
        UseAllNotebooks = 2
    };

    
signals:
    
public slots:
    void loadFontSizes(QString font);
    void loadGuiFontSizes(QString name);
    void fontSizeChanged(QString name);
    void showTrayIconChanged(bool value);
};

#endif // APPEARANCEPREFERENCES_H

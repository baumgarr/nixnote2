/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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



#ifndef SPELLCHECKDIALOG_H
#define SPELLCHECKDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>

class SpellCheckDialog : public QDialog
{
    Q_OBJECT
private:
    QLabel          *currentWord;
    QLineEdit       *replacementWord;
    QString         misspelledWord;
    QPushButton     *replace;
    QPushButton     *ignore;
    QPushButton     *ignoreAll;
    QPushButton     *addToDictionary;
    QListWidget     *suggestions;
    void            loadLanguages();


public:
    explicit SpellCheckDialog(QString misspelled, QStringList suggestions, QWidget *parent = 0);
    QComboBox       *language;
    bool            replacePressed;
    bool            cancelPressed;
    bool            ignorePressed;
    bool            ignoreAllPressed;
    bool            addToDictionaryPressed;
    bool            changeLanguage;
    QString         replacement;

protected:
   //void closeEvent(QCloseEvent *e);

signals:
    
public slots:
    void validateInput();
    void replacementChosen();
    void replaceButtonPressed();
    void ignoreButtonPressed();
    void ignoreAllButtonPressed();
    void addToDictionaryButtonPressed();
    void cancelButtonPressed();
    void languageChangeRequested(int);
};

#endif // SPELLCHECKDIALOG_H

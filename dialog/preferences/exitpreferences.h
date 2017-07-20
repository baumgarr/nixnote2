/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

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

#ifndef EXITPREFERENCES_H
#define EXITPREFERENCES_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


class ExitPreferences : public QWidget
{
    Q_OBJECT
private:
    QLineEdit   *loadExitFile;
    QPushButton *loadExitButton;
    QLabel      *loadExitFileLabel;
    QComboBox   *loadExitEnabledCombo;
    QLabel      *loadExitEnabledLabel;

    QLineEdit   *saveExitFile;
    QPushButton *saveExitButton;
    QLabel      *saveExitFileLabel;
    QComboBox   *saveExitEnabledCombo;
    QLabel      *saveExitEnabledLabel;
\
public:
    explicit ExitPreferences(QWidget *parent = 0);
    void saveValues();
    void loadValues();

signals:

public slots:
    void loadExitButtonPressed(bool);
    void saveExitButtonPressed(bool);
};

#endif // EXITPREFERENCES_H

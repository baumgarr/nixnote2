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

#ifndef SAVEDSEARCHPROPERTIES_H
#define SAVEDSEARCHPROPERTIES_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class SavedSearchProperties : public QDialog
{
    Q_OBJECT
private:

    QPushButton ok;
    QLabel nameLabel;
    QLabel queryLabel;
    QPushButton cancel;
    QGridLayout grid;
    QGridLayout queryGrid;
    QGridLayout buttonGrid;
    QStringList existingSearches;
    QString originalName;
    qint32 lid;

public:
    explicit SavedSearchProperties(QWidget *parent = 0);
    QLineEdit	name;
    QLineEdit	query;
    bool okPressed;
    void setLid(qint32 lid);


signals:

private slots:
    void okButtonPressed();
    void cancelButtonPressed();
    void validateInput();

};
#endif // SAVEDSEARCHPROPERTIES_H

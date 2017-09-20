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



#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QObject>
#include <QStringList>

// Windows Check
#ifndef _WIN32
#include <hunspell/hunspell.hxx>

class SpellChecker : public QObject
{
    Q_OBJECT
private:
    QStringList dictionaryPath;
    QString findDictionary(QString file);
    Hunspell *hunspell;
    bool error;
    QString errorMsg;

public:
    explicit SpellChecker(QObject *parent = 0);
    void setup(QString programDictionary, QString customDictionary, QString language);
    bool spellCheck(QString word, QStringList &suggestions);
    void addWord(QString dictionary, QString word);
    
signals:
    
public slots:
    
};

#endif // end of windows check

#endif // SPELLCHECKER_H

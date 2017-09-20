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



#include "spellchecker.h"
#include <QFile>
#include <QLocale>
#include <QTextStream>


SpellChecker::SpellChecker(QObject *parent) :
    QObject(parent)
{
    dictionaryPath.append("/usr/share/hunspell/");
    dictionaryPath.append("/usr/share/myspell/");
    dictionaryPath.append("/usr/share/myspell/dicts/");
    dictionaryPath.append("/Library/Spelling/");
    dictionaryPath.append("/opt/openoffice.org/basis3.0/share/dict/ooo/");
    dictionaryPath.append("/opt/openoffice.org2.4/share/dict/ooo/");
    dictionaryPath.append("/usr/lib/openoffice.org2.4/share/dict/ooo");
    dictionaryPath.append("/opt/openoffice.org2.3/share/dict/ooo/");
    dictionaryPath.append("/usr/lib/openoffice.org2.3/share/dict/ooo/");
    dictionaryPath.append("/opt/openoffice.org2.2/share/dict/ooo/");
    dictionaryPath.append("/usr/lib/openoffice.org2.2/share/dict/ooo/");
    dictionaryPath.append("/opt/openoffice.org2.1/share/dict/ooo/");
    dictionaryPath.append("/usr/lib/openoffice.org2.1/share/dict/ooo");
    dictionaryPath.append("/opt/openoffice.org2.0/share/dict/ooo/");
    dictionaryPath.append("/usr/lib/openoffice.org2.0/share/dict/ooo/");

    error = false;
}




QString SpellChecker::findDictionary(QString file) {
    for (int i=0; i<dictionaryPath.size(); i++) {
        QFile f(dictionaryPath[i]+file);
        if (f.exists())
            return dictionaryPath[i]+file;
    }
    return "";
}



void SpellChecker::setup(QString programDictionary, QString customDictionary, QString language) {
    QString locale = QLocale::system().name();
    if (language != "")
        locale = language;
    dictionaryPath.prepend(programDictionary);
    dictionaryPath.prepend(customDictionary);

    QString aff = findDictionary(locale+".aff");
    QString dic = findDictionary(locale+".dic");
    if (dic=="" || aff == "") {
        error = true;
        errorMsg = tr("Unable to find dictionaries.  Is Huntspell installed?");
        return;
    }
    hunspell = new Hunspell(aff.toStdString().c_str(), dic.toStdString().c_str());

    //Start adding custom words
    QFile f(customDictionary+"user.lst");
    if (f.exists()) {
        f.open(QIODevice::ReadOnly);
        QTextStream in(&f);
        while ( !in.atEnd() ) {
            QString line = in.readLine();
            hunspell->add(line.toStdString().c_str());
        }
        f.close();
    }
}


bool SpellChecker::spellCheck(QString word, QStringList &suggestions) {
    int isValid = hunspell->spell(word.toStdString().c_str());
    suggestions.empty();
    if (isValid) {
        return true;
    }
    char **wlst;
    int ns = hunspell->suggest(&wlst,word.toStdString().c_str());
    for (int i=0; i < ns; i++) {
      suggestions.append(QString::fromStdString(wlst[i]));
    }
    return false;
}


void SpellChecker::addWord(QString dictionary, QString word) {
    hunspell->add(word.toStdString().c_str());

    // Append to the end of the user dictionary
    //Start adding custom words
    QFile f(dictionary);
    f.open(QIODevice::Append);
    QTextStream out(&f);
    out << word;
    f.close();
}

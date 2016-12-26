/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2016 Randy Baumgarte

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


// This overrides the virtual plugin HunspellInterface.

#ifndef HUNSPELLPLUGIN_H
#define HUNSPELLPLUGIN_H
#include "hunspellinterface.h"

#// Windows Check
#ifndef _WIN32

class HunspellPlugin : public QObject, HunspellInterface
{
    Q_OBJECT
    Q_INTERFACES(HunspellInterface)
#if QT_VERSION < 0x050000
#else
    Q_PLUGIN_METADATA(IID "org.nixnote.NixNote2.HunspellInterface/2.0");
#endif

private:
    SpellChecker *checker;

public:
    HunspellPlugin();
    void initialize(QString programDictionary, QString userDictionary);
    bool spellCheck(QString word, QStringList &suggestions);
    void addWord(QString dictionary, QString word);
};

#endif // end of windows check

#endif // HUNSPELLPLUGIN_H

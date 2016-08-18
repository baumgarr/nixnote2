TARGET = ../hunspellplugin
CONFIG += plugin
TEMPLATE = lib

SOURCES += hunspellplugin.cpp \
           ../../utilities/spellchecker.cpp


HEADERS += hunspellplugin.h \
           hunspellinterface.h \
           ../../utilities/spellchecker.h

LIBS +=    -lhunspell

QMAKE_LFLAGS += -Wl,-soname,libhunspellplugin.so.1

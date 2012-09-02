#-------------------------------------------------
#
# Project created by QtCreator 2012-08-03T09:03:25
#
#-------------------------------------------------

QT       -= gui

TARGET = encrypt
TEMPLATE = lib

DEFINES += ENCRYPT_LIBRARY

SOURCES += encrypt.cpp

HEADERS += encrypt.h\
        encrypt_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE033A414
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = encrypt.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

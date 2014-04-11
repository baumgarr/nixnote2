TARGET = QEverCloud
TEMPLATE = lib
CONFIG += staticlib

#QT += network widgets webkitwidgets
QT       += widgets core gui webkit sql network xml webkitwidgets


CONFIG += silent
CONFIG -= debug_and_release debug_and_release_target

DESTDIR = bin
MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = ui
RCC_DIR = rcc

HEADERS += \
    include/QEverCloud.h \
    include/QEverCloudOAuth.h \
    oauth.h \
    thrift.h \
    http.h \
    public.h \
    exceptions.h \
    globals.h \
    impl.h \
    thumbnail.h

SOURCES += \
    exceptions.cpp \
    oauth.cpp \
    http.cpp \
    services_nongenerated.cpp


SOURCES += generated/constants.cpp generated/services.cpp generated/types.cpp
HEADERS += generated/constants.h generated/services.h generated/types.h generated/types_impl.h 





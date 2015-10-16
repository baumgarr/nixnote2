TARGET = QEverCloud
TEMPLATE = lib
CONFIG += staticlib

QT += network widgets

use_qwebengine {
QT += webenginewidgets webengine
DEFINES += USE_QT_WEB_ENGINE
}
else {
QT += webkitwidgets webkit
}

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
    thumbnail.h \
    AsyncResult.h \
    Optional.h \
    EverCloudException.h \
    qt4helpers.h \
    EventLoopFinisher.h

SOURCES += \
    exceptions.cpp \
    oauth.cpp \
    http.cpp \
    services_nongenerated.cpp \
    AsyncResult.cpp \
    EventLoopFinisher.cpp

SOURCES += generated/constants.cpp generated/services.cpp generated/types.cpp
HEADERS += generated/constants.h generated/services.h generated/types.h generated/types_impl.h generated/EDAMErrorCode.h





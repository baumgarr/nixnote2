TARGET = ../webcamplugin
CONFIG += plugin
TEMPLATE = lib

SOURCES += webcamplugin.cpp \
           ../../webcam/cvimagewidget.cpp \
           ../../dialog/webcamcapturedialog.cpp
LIBS +=    -lopencv_core -lopencv_highgui -lopencv_imgproc


HEADERS += webcamplugin.h \
           webcaminterface.h \
           ../../webcam/cvimagewidget.h \
          ../../dialog/webcamcapturedialog.h


QMAKE_LFLAGS += -Wl,-soname,libwebcamplugin.so.1


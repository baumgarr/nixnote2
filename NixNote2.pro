#-------------------------------------------------
#
# Project created by QtCreator 2011-11-12T14:00:42
#
#-------------------------------------------------

QT       += core gui webkit sql network xml

TARGET = nixnote
TEMPLATE = app
RESOURCES = NixNote2.qrc

SOURCES += main.cpp\
        nixnote.cpp \
    global.cpp \
    settings/filemanager.cpp \
    gui/nwebpage.cpp \
    gui/nwebview.cpp \
    sql/databaseconnection.cpp \
    logger/qsdebugoutput.cpp \
    logger/qslog.cpp \
    logger/qslogdest.cpp \
    sql/configstore.cpp \
    gui/ntableview.cpp \
    gui/ntableviewheader.cpp \
    evernote/UserStore_types.cpp \
    evernote/UserStore_constants.cpp \
    evernote/UserStore.cpp \
    evernote/Types_types.cpp \
    evernote/Types_constants.cpp \
    evernote/NoteStore_types.cpp \
    evernote/NoteStore_constants.cpp \
    evernote/NoteStore.cpp \
    evernote/Limits_types.cpp \
    evernote/Limits_constants.cpp \
    evernote/Errors_types.cpp \
    evernote/Errors_constants.cpp \
    threads/syncrunner.cpp \
    sql/datastore.cpp \
    sql/usertable.cpp \
    sql/tagtable.cpp \
    sql/searchtable.cpp \
    gui/nsearchview.cpp \
    models/notemodel.cpp \
    gui/nmainmenubar.cpp \
    gui/nsearchviewitem.cpp \
    gui/ntagview.cpp \
    gui/ntagviewitem.cpp \
    gui/widgetpanel.cpp \
    gui/numberdelegate.cpp \
    gui/ntabwidget.cpp \
    gui/nnotebookviewitem.cpp \
    gui/nnotebookview.cpp \
    gui/datedelegate.cpp \
    models/ntreemodel.cpp \
    sql/resourcetable.cpp \
    sql/notetable.cpp \
    sql/notebooktable.cpp \
    filters/notesortfilterproxymodel.cpp \
    html/thumbnailer.cpp \
    html/noteformatter.cpp \
    filters/ensearch.cpp \
    settings/startupconfig.cpp \
    dialog/logindialog.cpp \
    gui/lineedit.cpp \
    gui/nattributetree.cpp \
    filters/filtercriteria.cpp \
    gui/ntrashtree.cpp \
    filters/filterengine.cpp \
    models/notecache.cpp \
    gui/nbrowserwindow.cpp \
    threads/indexrunner.cpp \
    html/tagscanner.cpp \
    java/javamachine.cpp \
    java/messageblock.cpp \
    xml/importdata.cpp \
    sql/notemetadata.cpp \
    sql/sharednotebooktable.cpp \
    gui/browserWidgets/ntitleeditor.cpp \
    gui/browserWidgets/notebookmenubutton.cpp \
    gui/browserWidgets/expandbutton.cpp \
    gui/browserWidgets/urleditor.cpp \
    gui/browserWidgets/tageditor.cpp \
    gui/browserWidgets/tageditornewtag.cpp \
    gui/browserWidgets/tagviewer.cpp \
    gui/flowlayout.cpp \
    gui/browserWidgets/authoreditor.cpp \
    gui/browserWidgets/dateeditor.cpp \
    gui/browserWidgets/datetimeeditor.cpp \
    dialog/savedsearchproperties.cpp \
    gui/treewidgeteditor.cpp \
    dialog/tagproperties.cpp

HEADERS  += nixnote.h \
    global.h \
    settings/filemanager.h \
    gui/nwebpage.h \
    gui/nwebview.h \
    sql/databaseconnection.h \
    logger/qsdebugoutput.h \
    logger/qslogdest.h \
    logger/qslog.h \
    sql/configstore.h \
    gui/ntableview.h \
    gui/ntableviewheader.h \
    evernote/UserStore_types.h \
    evernote/UserStore_constants.h \
    evernote/UserStore.h \
    evernote/Types_types.h \
    evernote/Types_constants.h \
    evernote/NoteStore_types.h \
    evernote/NoteStore_constants.h \
    evernote/NoteStore.h \
    evernote/Limits_types.h \
    evernote/Limits_constants.h \
    evernote/Errors_types.h \
    evernote/Errors_constants.h \
    threads/syncrunner.h \
    sql/datastore.h \
    sql/usertable.h \
    sql/tagtable.h \
    sql/searchtable.h \
    gui/nsearchview.h \
    models/notemodel.h \
    gui/nmainmenubar.h \
    gui/nsearchviewitem.h \
    gui/ntagview.h \
    gui/ntagviewitem.h \
    gui/widgetpanel.h \
    gui/numberdelegate.h \
    gui/ntabwidget.h \
    gui/notetabledatedelegate.h \
    gui/nnotebookviewitem.h \
    gui/nnotebookview.h \
    gui/datedelegate.h \
    models/ntreemodel.h \
    sql/resourcetable.h \
    sql/notetable.h \
    sql/notebooktable.h \
    filters/notesortfilterproxymodel.h \
    html/thumbnailer.h \
    html/noteformatter.h \
    filters/ensearch.h \
    settings/startupconfig.h \
    logindialog.h \
    dialog/logindialog.h \
    gui/lineedit.h \
    gui/nattributetree.h \
    filters/filtercriteria.h \
    gui/ntrashtree.h \
    filters/filterengine.h \
    models/notecache.h \
    gui/nbrowserwindow.h \
    threads/indexrunner.h \
    html/tagscanner.h \
    java/javamachine.h \
    java/messageblock.h \
    xml/importdata.h \
    sql/notemetadata.h \
    sql/sharednotebooktable.h \
    gui/browserWidgets/ntitleeditor.h \
    gui/browserWidgets/notebookmenubutton.h \
    gui/browserWidgets/expandbutton.h \
    gui/browserWidgets/urleditor.h \
    gui/browserWidgets/tageditor.h \
    gui/browserWidgets/tageditornewtag.h \
    gui/browserWidgets/tagviewer.h \
    gui/flowlayout.h \
    gui/browserWidgets/authoreditor.h \
    gui/browserWidgets/dateeditor.h \
    gui/browserWidgets/datetimeeditor.h \
    dialog/savedsearchproperties.h \
    gui/treewidgeteditor.h \
    dialog/tagproperties.h

#LIBS += -L/usr/lib/jvm/java-6-sun/jre/lib/i386/client/ -lthrift -ljvm

    #LIBS +=  -L/usr/lib/jvm/java-6-sun/jre/lib/i386/client -L./lib/ -lthrift -ljvm -lpthread
    LIBS +=  -L/usr/lib/jvm/java-6-openjdk/jre/lib/i386/server -ljvm -lpthread -lthrift
    INCLUDEPATH += /usr/local/include/thrift \
        /usr/lib/jvm/java-6-openjdk/include \
        /usr/lib/jvm/java-6-openjdk/include/linux
    QMAKE_LFLAGS += -Wl,--rpath=./lib/
    QMAKE_LFLAGS_RPATH=


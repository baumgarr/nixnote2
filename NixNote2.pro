#-------------------------------------------------
#
# Project created by QtCreator 2011-11-12T14:00:42
#
#-------------------------------------------------

QT       += core gui webkit sql network xml

TARGET = nixnote
TEMPLATE = app
RESOURCES = NixNote2.qrc


#DESTDIR = ./bin
UI_DIR = .

CONFIG(debug, debug|release) {
        OBJECTS_DIR = build/debug
        MOC_DIR = build/debug
}

CONFIG(release, debug|release) {
        OBJECTS_DIR = build/release
        MOC_DIR = build/release
}

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
#    java/javamachine.cpp \
#    java/messageblock.cpp \
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
    dialog/tagproperties.cpp \
    dialog/notebookproperties.cpp \
    html/enmlformatter.cpp \
    utilities/encrypt.cpp \
    dialog/endecryptdialog.cpp \
    oauth/oauthtokenizer.cpp \
    oauth/oauthwindow.cpp \
    communication/communicationmanager.cpp \
    gui/browserWidgets/colormenu.cpp \
    xml/xmlhighlighter.cpp \
    gui/browserWidgets/toolbarwidgetaction.cpp \
    utilities/mimereference.cpp \
    dialog/accountdialog.cpp \
    gui/shortcutkeys.cpp \
    dialog/insertlinkdialog.cpp \
    dialog/tabledialog.cpp \
    dialog/encryptdialog.cpp \
    dialog/insertlatexdialog.cpp \
    botan/botan_all.cpp \
    gui/plugins/popplerviewer.cpp \
    gui/plugins/pluginfactory.cpp
#    thrift/TApplicationException.cpp  \
#    thrift/Thrift.cpp  \
#    thrift/VirtualProfiling.cpp \
#    thrift/async/TAsyncChannel.cpp \
#    thrift/async/TAsyncProtocolProcessor.cpp \
#    thrift/async/TEvhttpClientChannel.cpp   \
#    thrift/async/TEvhttpServer.cpp \
#    thrift/concurrency/BoostMonitor.cpp \
#    thrift/concurrency/BoostThreadFactory.cpp \
#    thrift/concurrency/Mutex.cpp \
#    thrift/concurrency/ThreadManager.cpp \
#    thrift/concurrency/Util.cpp \
#    thrift/concurrency/BoostMutex.cpp \
#    thrift/concurrency/Monitor.cpp \
#    thrift/concurrency/PosixThreadFactory.cpp \
#    thrift/concurrency/TimerManager.cpp \
#    thrift/processor/PeekProcessor.cpp \
#    thrift/protocol/TBase64Utils.cpp   \
#    thrift/protocol/TDebugProtocol.cpp \
#    thrift/protocol/TDenseProtocol.cpp \
#    thrift/protocol/TJSONProtocol.cpp \
#    thrift/protocol/TBinaryProtocol.tcc  \
#    thrift/protocol/TCompactProtocol.tcc \
#    thrift/protocol/TBufferTransports.cpp \
#    thrift/protocol/THttpServer.cpp  \
#    thrift/protocol/TSocket.cpp \
#    thrift/protocol/TTransportException.cpp \
#    thrift/protocol/TFDTransport.cpp \
#    thrift/protocol/THttpTransport.cpp \
#    thrift/protocol/TSocketPool.cpp \
#    thrift/protocol/TTransportUtils.cpp \
#    thrift/protocol/TFileTransport.cpp \
#    thrift/protocol/TServerSocket.cpp  \
#    thrift/protocol/TSSLServerSocket.cpp \
#    thrift/protocol/TZlibTransport.cpp \
#    thrift/protocol/THttpClient.cpp \
#    thrift/protocol/TSimpleFileTransport.cpp \
#    thrift/protocol/TSSLSocket.cpp



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
#    java/javamachine.h \
#    java/messageblock.h \
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
    dialog/tagproperties.h \
    dialog/notebookproperties.h \
    html/enmlformatter.h \
    utilities/encrypt.h \
    dialog/endecryptdialog.h \
    oauth/oauthtokenizer.h \
    oauth/oauthwindow.h \
    communication/communicationmanager.h \
    gui/browserWidgets/colormenu.h \
    xml/xmlhighlighter.h \
    gui/browserWidgets/toolbarwidgetaction.h \
    utilities/mimereference.h \
    dialog/accountdialog.h \
    gui/shortcutkeys.h \
    dialog/insertlinkdialog.h \
    dialog/tabledialog.h \
    dialog/encryptdialog.h \
    dialog/insertlatexdialog.h \
    botan/botan_all.h \
    gui/plugins/popplerviewer.h \
    gui/plugins/pluginfactory.h
#    thrift/TApplicationException.h  \
#    thrift/Thrift.h \
#    thrift/TLogging.h  \
#    thrift/TProcessor.h \
#    thrift/TReflectionLocal.h \
#    thrift/async/TAsyncBufferProcessor.h \
#    thrift/async/TAsyncChannel.h \
#    thrift/async/TAsyncProcessor.h \
#    thrift/async/TAsyncProtocolProcessor.h \
#    thrift/async/TEvhttpClientChannel.h \
#    thrift/async/TEvhttpServer.h \
#    thrift/concurrency/BoostThreadFactory.h \
#    thrift/concurrency/Monitor.h \
#    thrift/concurrency/PosixThreadFactory.h \
#    thrift/concurrency/TimerManager.h \
#    thrift/concurrency/Exception.h \
#    thrift/concurrency/Mutex.h \
#    thrift/concurrency/Thread.h \
#    thrift/concurrency/Util.h \
#    thrift/concurrency/FunctionRunner.h \
#    thrift/concurrency/PlatformThreadFactory.h \
#    thrift/concurrency/ThreadManager.h \
#    thrift/processor/PeekProcessor.h  \
#    thrift/processor/StatsProcessor.h \
#    thrift/protocol/TBase64Utils.h     \
#    thrift/protocol/TCompactProtocol.h  \
#    thrift/protocol/TDenseProtocol.h  \
#    thrift/protocol/TProtocolException.h \
#    thrift/protocol/TProtocolTap.h \
#    thrift/protocol/TBinaryProtocol.h \
#    thrift/protocol/TDebugProtocol.h \
#    thrift/protocol/TJSONProtocol.h \
#    thrift/protocol/TProtocol.h \
#    thrift/protocol/TVirtualProtocol.h \
#    thrift/transport/TBufferTransports.h \
#    thrift/transport/THttpTransport.h \
#    thrift/transport/TSocket.h \
#    thrift/transport/TTransport.h \
#    thrift/transport/TFDTransport.h \
#    thrift/transport/TServerSocket.h \
#    thrift/transport/TSocketPool.h \
#    thrift/transport/TTransportUtils.h \
#    thrift/transport/TFileTransport.h \
#    thrift/transport/TServerTransport.h \
#    thrift/transport/TSSLServerSocket.h \
#    thrift/transport/TVirtualTransport.h \
#    thrift/transport/THttpClient.h \
#    thrift/transport/TShortReadTransport.h \
#    thrift/transport/TSSLSocket.h \
#    thrift/transport/TZlibTransport.h \
#    thrift/transport/THttpServer.h \
#    thrift/transport/TSimpleFileTransport.h \
#    thrift/transport/TTransportException.h


LIBS += -L./lib -lthrift -lpthread  -L/usr/lib -lpoppler-qt4
#LIBS += -lthrift -lpthread
INCLUDEPATH += /usr/local/include/thrift \
            /usr/include/poppler/qt4

#LIBS +=  -L/usr/lib/jvm/java-6-openjdk/jre/lib/i386/server -ljvm -lpthread -lthrift
        # -L./libencrypt -lencrypt

#INCLUDEPATH += /usr/local/include/thrift \
#        /usr/lib/jvm/java-6-openjdk/include \
#        /usr/lib/jvm/java-6-openjdk/include/linux

#QMAKE_LFLAGS += -Wl,--rpath=./lib/
#QMAKE_LFLAGS_RPATH=

#QMAKE_CXXFLAGS += -isystem=./evernote/

#QMAKE_LFLAGS += -static

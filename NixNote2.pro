#-------------------------------------------------
#
# Project created by QtCreator 2011-11-12T14:00:42
#
#-------------------------------------------------

QT       += core gui webkit sql network xml

TARGET = nixnote
TEMPLATE = app
RESOURCES = NixNote2.qrc

UI_DIR = .

CONFIG(debug, debug|release) {
        OBJECTS_DIR = build/debug
        MOC_DIR = build/debug
}

CONFIG += debug_and_release

TRANSLATIONS = \
    translations/nixnote2_cs_CZ.ts \
    translations/nixnote2_de.ts    \
    translations/nixnote2_ca.ts    \
    translations/nixnote2_da.ts    \
    translations/nixnote2_es.ts    \
    translations/nixnote2_ja.ts    \
    translations/nixnote2_pt.ts    \
    translations/nixnote2_sk.ts    \
    translations/nixnote2_zh_TW.ts \
    translations/nixnote2_fr.ts    \
    translations/nixnote2_pl.ts    \
    translations/nixnote2_ru.ts    \
    translations/nixnote2_zh_CN.ts



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
    utilities/mimereference.cpp \
    dialog/accountdialog.cpp \
    gui/shortcutkeys.cpp \
    dialog/insertlinkdialog.cpp \
    dialog/tabledialog.cpp \
    dialog/encryptdialog.cpp \
    dialog/insertlatexdialog.cpp \
    gui/plugins/popplerviewer.cpp \
    gui/plugins/pluginfactory.cpp \
    gui/findreplace.cpp \
    sql/linkednotebooktable.cpp \
    application.cpp \
    dialog/databasestatus.cpp \
    gui/plugins/popplergraphicsview.cpp \
    threads/counterrunner.cpp \
    gui/nnotebookviewdelegate.cpp \
    gui/ntrashviewdelegate.cpp \
    gui/ntagviewdelegate.cpp \
    watcher/filewatcher.cpp \
    sql/filewatchertable.cpp \
    watcher/filewatchermanager.cpp \
    dialog/watchfolderadd.cpp \
    dialog/watchfolderdialog.cpp \
    dialog/preferences/preferencesdialog.cpp \
    dialog/preferences/debugpreferences.cpp \
    dialog/preferences/syncpreferences.cpp \
    dialog/preferences/appearancepreferences.cpp \
    settings/accountsmanager.cpp \
    dialog/adduseraccountdialog.cpp \
    dialog/accountmaintenancedialog.cpp \
    gui/truefalsedelegate.cpp \
    gui/browserWidgets/editorbuttonbar.cpp \
    communication/communicationerror.cpp \
    dialog/screencapture.cpp \
    gui/imagedelegate.cpp \
    dialog/preferences/searchpreferences.cpp \
    html/attachmenticonbuilder.cpp \
    dialog/locationdialog.cpp \
    gui/browserWidgets/locationeditor.cpp \
    dialog/preferences/localepreferences.cpp \
    gui/reminderorderdelegate.cpp \
    gui/browserWidgets/reminderbutton.cpp \
    dialog/remindersetdialog.cpp \
    reminders/reminderevent.cpp \
    reminders/remindermanager.cpp \
    dialog/notehistoryselect.cpp \
    dialog/closenotebookdialog.cpp \
    webcam/cvimagewidget.cpp \
    dialog/webcamcapturedialog.cpp \
    utilities/spellchecker.cpp \
    dialog/spellcheckdialog.cpp \
    gui/externalbrowse.cpp \
    sql/nsqlquery.cpp \
    dialog/aboutdialog.cpp \
    xml/importenex.cpp \
    xml/exportdata.cpp \
    dialog/logviewer.cpp \
    dialog/htmlentitiesdialog.cpp



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
    utilities/mimereference.h \
    dialog/accountdialog.h \
    gui/shortcutkeys.h \
    dialog/insertlinkdialog.h \
    dialog/tabledialog.h \
    dialog/encryptdialog.h \
    dialog/insertlatexdialog.h \
    gui/plugins/popplerviewer.h \
    gui/plugins/pluginfactory.h \
    gui/findreplace.h \
    sql/linkednotebooktable.h \
    application.h \
    dialog/databasestatus.h \
    gui/plugins/popplergraphicsview.h \
    threads/counterrunner.h \
    gui/nnotebookviewdelegate.h \
    gui/ntrashviewdelegate.h \
    gui/ntagviewdelegate.h \
    watcher/filewatcher.h \
    sql/filewatchertable.h \
    watcher/filewatchermanager.h \
    dialog/watchfolderadd.h \
    dialog/watchfolderdialog.h \
    dialog/preferences/preferencesdialog.h \
    dialog/preferences/debugpreferences.h \
    dialog/preferences/syncpreferences.h \
    dialog/preferences/appearancepreferences.h \
    settings/accountsmanager.h \
    dialog/adduseraccountdialog.h \
    dialog/accountmaintenancedialog.h \
    gui/truefalsedelegate.h \
    gui/browserWidgets/editorbuttonbar.h \
    communication/communicationerror.h \
    dialog/screencapture.h \
    gui/imagedelegate.h \
    dialog/preferences/searchpreferences.h \
    html/attachmenticonbuilder.h \
    dialog/locationdialog.h \
    gui/browserWidgets/locationeditor.h \
    dialog/preferences/localepreferences.h \
    gui/reminderorderdelegate.h \
    gui/browserWidgets/reminderbutton.h \
    dialog/remindersetdialog.h \
    reminders/reminderevent.h \
    reminders/remindermanager.h \
    dialog/notehistoryselect.h \
    dialog/closenotebookdialog.h \
    webcam/cvimagewidget.h \
    dialog/webcamcapturedialog.h \
    utilities/spellchecker.h \
    dialog/spellcheckdialog.h \
    gui/externalbrowse.h \
    sql/nsqlquery.h \
    dialog/aboutdialog.h \
    xml/importenex.h \
    xml/exportdata.h \
    dialog/logviewer.h \
    dialog/htmlentitiesdialog.h


INCLUDEPATH += /usr/local/include/thrift \
            /usr/include/thrift \
            /usr/include/poppler/qt4

#LIBS +=    -Wl,-L./lib -lthrift \
LIBS +=    -lthrift \
           -lopencv_core -lopencv_highgui -lopencv_imgproc \
           -lhunspell \
           -lpthread -L/usr/lib -lpoppler-qt4 -g -rdynamic \
           -Wl,-rpath=/usr/lib/nixnote2

#QMAKE_CXXFLAGS += `dpkg-buildflags --get CFLAGS`
QMAKE_CXXFLAGS +=-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security
#QMAKE_LFLAGS += `dpkg-buildflags --get LDFLAGS`
QMAKE_LFLAGS += -Wl,-Bsymbolic-functions -Wl,-z,relro

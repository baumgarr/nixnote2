#ifndef NBROWSERWINDOW_H
#define NBROWSERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QToolBar>
#include <QTextEdit>
#include <QTimer>

#include "gui/nwebview.h"

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "gui/browserWidgets/ntitleeditor.h"
#include "gui/browserWidgets/notebookmenubutton.h"
#include "gui/browserWidgets/expandbutton.h"
#include "gui/browserWidgets/urleditor.h"
#include "gui/browserWidgets/tageditor.h"
#include "gui/browserWidgets/authoreditor.h"
#include "gui/browserWidgets/dateeditor.h"
#include "gui/browserWidgets/colormenu.h"
#include "gui/nwebview.h"
#include "xml/xmlhighlighter.h"
#include "gui/browserWidgets/toolbarwidgetaction.h"

class ToolbarWidgetAction;

using namespace evernote::edam  ;

class NWebView;
class NBrowserWindow;

class NBrowserWindow : public QWidget
{
    Q_OBJECT
private:
    void loadFontSizeCombobox(QString name);
    void loadFontNames();
    void setupToolBar();
    QTimer *sourceEditorTimer;
    bool insertHyperlink;
    QString currentHyperlink;
    bool insideList;
    bool insideTable;
    bool insideEncryption;
    bool forceTextPaste;
    void editLatex(QString guid);
    QString selectedFileName;
    qint32 selectedFileLid;
    void rotateImage(qreal degrees);
    void updateImageHash(QString newhash);

public:
    explicit NBrowserWindow(QWidget *parent = 0);
    NWebView *editor;
    void setContent(qint32 lid);
    void saveNoteContent();
    NTitleEditor noteTitle;
    NotebookMenuButton notebookMenu;
    ExpandButton expandButton;
    UrlEditor urlEditor;
    TagEditor tagEditor;
    DateEditor dateEditor;
    AuthorEditor authorEditor;

    QToolBar buttonBar;
    ToolbarWidgetAction *undoButtonAction;
    ToolbarWidgetAction *redoButtonAction;
    ToolbarWidgetAction *cutButtonAction;
    ToolbarWidgetAction *pasteButtonAction;
    ToolbarWidgetAction *copyButtonAction;
    ToolbarWidgetAction *boldButtonAction;
    ToolbarWidgetAction *underlineButtonAction;
    ToolbarWidgetAction *italicsButtonAction;
    ToolbarWidgetAction *strikethroughButtonAction;
    ToolbarWidgetAction *leftAlignButtonAction;
    ToolbarWidgetAction *rightAlignButtonAction;
    ToolbarWidgetAction *centerAlignButtonAction;
    ToolbarWidgetAction *hlineButtonAction;
    ToolbarWidgetAction *shiftRightButtonAction;
    ToolbarWidgetAction *shiftLeftButtonAction;
    ToolbarWidgetAction *bulletListButtonAction;
    ToolbarWidgetAction *numberListButtonAction;
    QShortcut *focusNoteShortcut;
    QShortcut *focusTitleShortcut;
    QShortcut *insertDatetimeShortcut;
    QAction *fontColorButtonAction;
    QToolButton *fontColorButton;
    QAction *highlightButtonAction;
    QToolButton *highlightButton;
    ToolbarWidgetAction *spellButtonAction;
    ToolbarWidgetAction *todoButtonAction;
    QComboBox *fontSize;
    QComboBox *fontNames;
    QToolButton *fontColor;
    QToolButton *highlightColor;
    ColorMenu fontColorMenu;
    ColorMenu highlightColorMenu;

    QHBoxLayout line2Layout;
    QHBoxLayout line3Layout;
    qint32 lid;
    void setBackgroundColor(QString);
    void showSource(bool);
    QTextEdit *sourceEdit;
    QString sourceEditHeader;
    XmlHighlighter *highlighter;

    void tabPressed();
    void backtabPressed();
    void clear();
    void setupShortcut(QShortcut *action, QString text);
    void contentChanged();

signals:
    void noteUpdated(qint32);
    qint32 tagAdded(qint32);
    void evernoteLinkClicked(qint32 lid, bool newWindow);

public slots:
    void changeExpandState(int value);
    void tagRenamed(qint32 lid, QString oldName, QString newName);
    void notebookRenamed(qint32 lid, QString oldName, QString newName);
    void tagDeleted(qint32 lid, QString name);
    void notebookDeleted(qint32 lid, QString name);
    void notebookAdded(qint32 lid);
    void addTagName(qint32 lid);
    void stackRenamed(QString oldName, QString newName);
    void stackDeleted(QString name);
    void stackAdded(QString name);
    void noteSyncUpdate(qint32 lid);
    void noteContentUpdated();
    void microFocusChanged();

    void undoButtonPressed();
    void redoButtonPressed();
    void cutButtonPressed();
    void copyButtonPressed();
    void pasteButtonPressed();
    void pasteWithoutFormatButtonPressed();
    void boldButtonPressed();
    void italicsButtonPressed();
    void underlineButtonPressed();
    void strikethroughButtonPressed();
    void alignLeftButtonPressed();
    void alignCenterButtonPressed();
    void alignRightButtonPressed();
    void horizontalLineButtonPressed();
    void shiftLeftButtonPressed();
    void shiftRightButtonPressed();
    void bulletListButtonPressed();
    void numberListButtonPressed();
    void todoButtonPressed();
    void fontSizeSelected(int index);
    void fontNameSelected(int index);
    void fontColorClicked();
    void fontHilightClicked();
    void encryptButtonPressed();
    void insertLinkButtonPressed();
    void insertQuickLinkButtonPressed();
    void insertLatexButtonPressed();
    void insertTableButtonPressed();
    void insertTableRowButtonPressed();
    void insertTableColumnButtonPressed();
    void deleteTableRowButtonPressed();
    void deleteTableColumnButtonPressed();
    void rotateImageLeftButtonPressed();
    void rotateImageRightButtonPressed();
    void linkClicked(const QUrl url);
    void toggleSource();
    void setSource();
    void focusTitle();
    void focusNote();
    void insertDatetime();

    void exposeToJavascript();
    void boldActive();
    void italicsActive();
    void insideEncryptionArea();
    void underlineActive();
    void setInsideList();
    void setInsideTable();
    void setInsideLink(QString link);
    QString fixEncryptionPaste(QString data);
    void imageContextMenu(QString l, QString f);



private slots:
    void sendUpdateSignal();
    void newTagAdded(qint32);
};

#endif // NBROWSERWINDOW_H

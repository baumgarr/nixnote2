#ifndef TAGEDITOR_H
#define TAGEDITOR_H

#include <QWidget>
#include <QLabel>
#include "tageditornewtag.h"
#include "tagviewer.h"
#include <QHBoxLayout>
#include "gui/flowlayout.h"

#define MAX_TAGS 100

class TagEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TagEditor(QWidget *parent = 0);
    void addTag(QString text);
    void setTags(QStringList names);
    void clear();
    void loadTags();
    void setCurrentLid(int l);
    void tagRenamed(int lid, QString oldName, QString newName);

private:
    int currentLid;
    QLabel tagIcon;
    TagEditorNewTag newTag;
    TagViewer tags[MAX_TAGS];
    FlowLayout *layout;
    QStringList tagNames;
    void emptyTags();
    bool checkNewTagEditor();
    bool newEditorHasFocus;

signals:
    void buttonClicked(QString);
    void tagsUpdated();
    void newTagCreated(int lid);
    
public slots:
    void removeTag(QString text);
    void newTagFocusLost(bool focus);
    void newTagTabPressed();

protected:
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);


    
};

#endif // TAGEDITOR_H

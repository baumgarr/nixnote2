#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include <QWidgetAction>
#include <QToolButton>
#include <QPushButton>
#include "gui/nbrowserwindow.h"


class NBrowserWindow;

class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
private:
    NBrowserWindow *browser;

public:
    explicit ToolbarWidgetAction(QWidget *parent = 0);
    QWidget *createWidget(QWidget *parent);

    enum Button {
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Underline,
        Strikethrough,
        Bold,
        Italics,
        AlignRight,
        AlignLeft,
        AlignCenter,
        HorizontalLine,
        BulletList,
        NumberList,
        ShiftLeft,
        ShiftRight,
        SpellCheck,
        Todo
    };

    Button buttonType;
    void setType(Button type);
    QPushButton *button;
    
signals:
    
public slots:
    
};

#endif // TOOLBARWIDGETACTION_H

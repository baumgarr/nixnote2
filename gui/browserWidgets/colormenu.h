#ifndef COLORMENU_H
#define COLORMENU_H

#include <QObject>
#include <QColor>
#include <QMenu>

class ColorMenu : public QObject
{
    Q_OBJECT
private:
    QMenu menu;
    QObject *parent;
    void populateList();
    QColor currentColor;

public:
    explicit ColorMenu(QObject *parent = 0);
    void setDefault(QColor color);
    QColor* getColor();
    QMenu* getMenu();

    
signals:
    
public slots:
    void itemHovered();
    
};

#endif // COLORMENU_H

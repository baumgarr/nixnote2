#ifndef WIDGETPANEL_H
#define WIDGETPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class WidgetPanel : public QWidget
{
    Q_OBJECT
private:

public:
     QVBoxLayout *vboxLayout;

    explicit WidgetPanel(QWidget *parent = 0);
    ~WidgetPanel();
    void addWidget(QWidget *widget);

signals:

public slots:

};

#endif // WIDGETPANEL_H

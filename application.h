#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    bool notify(QObject *receiver_, QEvent *event_);
    
signals:
    
public slots:
    
};

#endif // APPLICATION_H

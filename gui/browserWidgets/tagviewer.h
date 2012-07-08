#ifndef TAGVIEWER_H
#define TAGVIEWER_H

#include <QLabel>
#include <QToolButton>

class TagViewer : public QLabel
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *);
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);

private:
    QToolButton *clearButton;

public:
    explicit TagViewer(QWidget *parent = 0);
    void resize();
    
signals:
    void closeClicked(QString);
    
private slots:
    void updateCloseButton(const QString &text);
    void buttonClicked();
    
};

#endif // TAGVIEWER_H

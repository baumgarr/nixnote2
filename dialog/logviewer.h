#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

class LogViewer : public QDialog
{
    Q_OBJECT
private:
    QTextEdit *viewer;
    QPushButton *okButton;
    QPushButton *saveButton;
    QPushButton *refreshButton;

public:
    explicit LogViewer(QWidget *parent = 0);

signals:

public slots:
    void loadData();
    void saveData();

};

#endif // LOGVIEWER_H

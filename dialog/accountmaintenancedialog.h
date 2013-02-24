#ifndef ACCOUNTMAINTENANCEDIALOG_H
#define ACCOUNTMAINTENANCEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "gui/nmainmenubar.h"

class AccountMaintenanceDialog : public QDialog
{
    Q_OBJECT
private:
    NMainMenuBar *menuBar;
    QPushButton *okButton;
    QPushButton *closeButton;
    QPushButton *addButton;
    QPushButton *renameButton;
    QPushButton *deleteButton;
    QListWidget *nameList;
    QVBoxLayout *buttonLayout1;
    QHBoxLayout *buttonLayout2;
    QHBoxLayout *displayLayout;
    QVBoxLayout *mainLayout;
    void loadData();
    QList<QString> names;
    QList<int> ids;
    bool removeDir(const QString & dirName);
    QWidget *parent;

public:
    explicit AccountMaintenanceDialog(NMainMenuBar *menuBar, QWidget *parent = 0);
    
signals:
    
public slots:
    void addAccount();
    void deleteAccount();
    void renameAccount();
    
};

#endif // ACCOUNTMAINTENANCEDIALOG_H

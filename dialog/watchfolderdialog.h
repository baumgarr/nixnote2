#ifndef WATCHFOLDERDIALOG_H
#define WATCHFOLDERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "watcher/filewatcher.h"
#include <QTableWidget>

class WatchFolderDialog : public QDialog
{
    Q_OBJECT
private:
    void load();
    void addRow(qint32 lid, int row, QString folder, qint32 notebookLid, FileWatcher::ScanType, bool includeSubdirs);

public:
    explicit WatchFolderDialog(QWidget *parent = 0);
    bool okClicked;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QTableWidget *table;
    
signals:
    
public slots:
    
private slots:
    void onClicked();
    void onCancel();
    void tableSelection();
    void addPressed();
    void editPressed();
    void deletePressed();
};

#endif // WATCHFOLDERDIALOG_H

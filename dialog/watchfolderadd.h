#ifndef WATCHFOLDERADD_H
#define WATCHFOLDERADD_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QCheckBox>

class WatchFolderAdd : public QDialog
{
    Q_OBJECT
private:
    QPushButton *okButton;
    QPushButton *cancelButton;

    QPushButton *folderButton;

    QGridLayout *grid;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
    QFileDialog *fileDialog;

public:
    explicit WatchFolderAdd(qint32 lid=0, QWidget *parent = 0);
    bool okClicked;
    QComboBox   *books;
    QLabel      *directory;
    QComboBox   *keep;
    QCheckBox   *subdirs;
    qint32 lid;
    
signals:
    
public slots:

private slots:
    void onClicked();
    void onCancel();
    void itemSelected();
    void folderButtonClicked();
    void folderSelected(QString f);
    
};

#endif // WATCHFOLDERADD_H


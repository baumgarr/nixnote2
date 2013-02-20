#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "dialog/preferences/debugpreferences.h"
#include "dialog/preferences/syncpreferences.h"
#include "dialog/preferences/appearancepreferences.h"

class PreferencesDialog : public QDialog
{
    Q_OBJECT
private:
    QTabWidget *tabs;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    void setupDebugPanel();
    void setupSyncPanel();
    void setupAppearancePanel();
    DebugPreferences *debugPanel;
    SyncPreferences *syncPanel;
    AppearancePreferences *appearancePanel;
    QPushButton *okButton;
    QPushButton *cancelButton;

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();
    bool okButtonPressed;
    
signals:
    
public slots:

private slots:
    void cancelButtonClicked();
    void okButtonClicked();
    
};

#endif // PREFERENCESDIALOG_H

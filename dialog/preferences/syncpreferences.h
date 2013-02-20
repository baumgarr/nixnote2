#ifndef SYNCPREFERENCES_H
#define SYNCPREFERENCES_H

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>

class SyncPreferences : public QWidget
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;
    QComboBox *syncInterval;
    QCheckBox *syncOnStartup;
    QCheckBox *syncAutomatically;
    QCheckBox *syncOnShutdown;
    int getSyncInterval();

public:
    explicit SyncPreferences(QWidget *parent = 0);
    ~SyncPreferences();
    void saveValues();
    QCheckBox *enableSyncNotifications;
    
signals:
    
public slots:

private slots:
    void enableSyncStateChange();
    
};

#endif // SYNCPREFERENCES_H

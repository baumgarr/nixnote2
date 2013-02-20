#ifndef DEBUGPREFERENCES_H
#define DEBUGPREFERENCES_H

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>

class DebugPreferences : public QWidget
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;
    QComboBox *debugLevel;
    QLabel *debugLevelLabel;
    int getMessageLevel();

public:
    explicit DebugPreferences(QWidget *parent = 0);
    ~DebugPreferences();
    QString getDebugLevel();
    void saveValues();
    
signals:
    
public slots:
    
};

#endif // DEBUGPREFERENCES_H

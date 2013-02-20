#ifndef APPEARANCEPREFERENCES_H
#define APPEARANCEPREFERENCES_H

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>

class AppearancePreferences : public QWidget
{
    Q_OBJECT
    enum DefaultNotebook {
        UseDefaultNotebook = 0,
        UseAllNotebooks = 1,
        UseLastViewedNotebook = 2
    };

private:
    QGridLayout *mainLayout;
    QComboBox *defaultNotebookOnStartup;
    QLabel *defaultNotebookOnStartupLabel;

public:
    explicit AppearancePreferences(QWidget *parent = 0);
    void saveValues();
    QCheckBox *showTrayIcon;

    
signals:
    
public slots:
    
};

#endif // APPEARANCEPREFERENCES_H

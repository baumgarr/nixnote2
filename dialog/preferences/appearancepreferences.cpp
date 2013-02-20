#include "appearancepreferences.h"
#include "global.h"

extern Global global;

AppearancePreferences::AppearancePreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);

    showTrayIcon = new QCheckBox(tr("Show tray icon"), this);

    defaultNotebookOnStartupLabel = new QLabel(tr("Notebook to use on startup"),this);
    defaultNotebookOnStartup = new QComboBox();
    defaultNotebookOnStartup->addItem(tr("Default notebook"), UseDefaultNotebook);
    defaultNotebookOnStartup->addItem(tr("All notebooks"), UseAllNotebooks);
    defaultNotebookOnStartup->addItem(tr("Last viewed"), UseLastViewedNotebook);
    defaultNotebookOnStartup->setEnabled(false);

    mainLayout->addWidget(showTrayIcon,0,0);
    mainLayout->addWidget(defaultNotebookOnStartupLabel,1,0);
    mainLayout->addWidget(defaultNotebookOnStartup, 1,1);

    global.settings->beginGroup("Appearance");
    showTrayIcon->setChecked(global.settings->value("showTrayIcon", false).toBool());
    global.settings->endGroup();
}


void AppearancePreferences::saveValues() {
    global.settings->beginGroup("Appearance");
    global.settings->setValue("showTrayIcon", showTrayIcon->isChecked());
    global.settings->endGroup();
}

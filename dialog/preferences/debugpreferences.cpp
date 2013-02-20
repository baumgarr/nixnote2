#include "debugpreferences.h"
#include "logger/qslog.h"
#include "global.h"

extern Global global;

using namespace QsLogging;

DebugPreferences::DebugPreferences(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(mainLayout);

    debugLevelLabel = new QLabel(tr("Message Level"), this);
    debugLevelLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    debugLevel = new QComboBox(this);
    debugLevel->addItem(tr("Trace"), TraceLevel);
    debugLevel->addItem(tr("Debug"), DebugLevel);
    debugLevel->addItem(tr("Info"), InfoLevel);
    debugLevel->addItem(tr("Warnings"), WarnLevel);
    debugLevel->addItem(tr("Errors"), ErrorLevel);
    debugLevel->addItem(tr("Fatal"), FatalLevel);

    global.settings->beginGroup("Debugging");
    int value = global.settings->value("messageLevel", InfoLevel).toInt();
    global.settings->endGroup();
    int index = debugLevel->findData(value);
    debugLevel->setCurrentIndex(index);
    mainLayout->addWidget(debugLevelLabel,0,0);
    mainLayout->addWidget(debugLevel,0,1);

}


DebugPreferences::~DebugPreferences() {
    delete debugLevel;
    delete debugLevelLabel;
    delete mainLayout;
}


int DebugPreferences::getMessageLevel() {
    int index = debugLevel->currentIndex();
    return debugLevel->itemData(index).toInt();
}


void DebugPreferences::saveValues() {
    int value = getMessageLevel();

    global.settings->beginGroup("Debugging");
    global.settings->setValue("messageLevel", value);
    global.settings->endGroup();
}

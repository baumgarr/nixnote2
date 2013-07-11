#include "searchpreferences.h"
#include "global.h"
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>

extern Global global;

SearchPreferences::SearchPreferences(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);
    weight = new QSpinBox(this);
    mainLayout->addWidget(new QLabel(tr("Minimum Image Recognition Weight")), 0,0);
    mainLayout->addWidget(weight,0,1);
    weight->setMinimum(1);
    weight->setMaximum(100);
    weight->setValue(global.getMinimumRecognitionWeight());
    global.settings->endGroup();
}




void SearchPreferences::saveValues() {
    global.setMinimumRecognitionWeight(weight->value());
}

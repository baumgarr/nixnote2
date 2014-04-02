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

    syncAttachments = new QCheckBox(tr("Synchronize Attachments"));
    mainLayout->addWidget(syncAttachments,0,0);
    syncAttachments->setChecked(global.synchronizeAttachments());

    weight = new QSpinBox(this);
    mainLayout->addWidget(new QLabel(tr("Minimum Image Recognition Weight")), 1,0);
    mainLayout->addWidget(weight,1,1);
    weight->setMinimum(1);
    weight->setMaximum(100);
    weight->setValue(global.getMinimumRecognitionWeight());

}




void SearchPreferences::saveValues() {
    global.setMinimumRecognitionWeight(weight->value());
    global.setSynchronizeAttachments(syncAttachments->isChecked());
}

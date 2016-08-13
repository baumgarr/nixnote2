#include "thumbnailpreferences.h"
#include "global.h"

extern Global global;


ThumbnailPreferences::ThumbnailPreferences(QWidget *parent) : QWidget(parent)
{
    minTime = new QSpinBox();
    maxTime = new QSpinBox();
    disableThumbnail = new QCheckBox();
    count = new QSpinBox();

    disableLabel = new QLabel(tr("Disable thumbnail generation"));
    countLabel = new QLabel(tr("Images to generate per interval"));
    minTimeLabel = new QLabel(tr("Minimum scan interval (in seconds)"));
    maxTimeLabel = new QLabel(tr("Maximum scan interval (in seconds)"));

    minTime->setMinimum(1);
    minTime->setMaximum(15);

    maxTime->setMinimum(16);
    maxTime->setMaximum(1200);

    count->setMinimum(1);
    count->setMaximum(99999);

    mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(mainLayout);

    mainLayout->addWidget(disableLabel, 0,0);
    mainLayout->addWidget(disableThumbnail, 0,1);
    mainLayout->addWidget(countLabel, 1,0);
    mainLayout->addWidget(count, 1,1);
    mainLayout->addWidget(minTimeLabel,2,0);
    mainLayout->addWidget(minTime,2,1);
    mainLayout->addWidget(maxTimeLabel,3,0);
    mainLayout->addWidget(maxTime, 3,1);

    global.settings->beginGroup("Thumbnail");
    int min = global.settings->value("minTime", 5).toInt();
    int max = global.settings->value("maxTime", 60).toInt();
    int cnt = global.settings->value("count", 1).toInt();
    bool disabled = global.settings->value("disabled", false).toBool();
    global.settings->endGroup();

    connect(disableThumbnail, SIGNAL(toggled(bool)), this, SLOT(disableToggled(bool)));
    minTime->setValue(min);
    maxTime->setValue(max);
    count->setValue(cnt);
    disableThumbnail->setChecked(disabled);

}


void ThumbnailPreferences::disableToggled(bool value) {
    minTime->setDisabled(value);
    maxTime->setDisabled(value);
    minTimeLabel->setDisabled(value);
    maxTimeLabel->setDisabled(value);
}


void ThumbnailPreferences::saveValues() {

    global.settings->beginGroup("Thumbnail");
    global.settings->setValue("minTime", minTime->value());
    global.settings->setValue("maxTime", maxTime->value());
    global.settings->setValue("count", count->value());
    global.settings->setValue("disabled", disableThumbnail->isChecked());
    global.settings->endGroup();

    global.disableThumbnails = disableThumbnail->isChecked();
    global.minimumThumbnailInterval = minTime->value();
    global.maximumThumbnailInterval = maxTime->value();
    global.batchThumbnailCount = count->value();
}

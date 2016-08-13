#ifndef THUMBNAILPREFERENCES_H
#define THUMBNAILPREFERENCES_H



#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>


class ThumbnailPreferences : public QWidget
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;
    QLabel *minTimeLabel;
    QSpinBox *minTime;
    QLabel *maxTimeLabel;
    QSpinBox *maxTime;
    QLabel *disableLabel;
    QCheckBox *disableThumbnail;
    QSpinBox *count;
    QLabel *countLabel;

public:
    explicit ThumbnailPreferences(QWidget *parent = 0);
    void saveValues();

signals:

public slots:
    void disableToggled(bool value);
};

#endif // THUMBNAILPREFERENCES_H

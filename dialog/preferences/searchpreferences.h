#ifndef SEARCHPREFERENCES_H
#define SEARCHPREFERENCES_H

#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>

class SearchPreferences : public QWidget
{
    Q_OBJECT
private:
    QSpinBox *weight;
    QCheckBox *syncAttachments;

public:
    explicit SearchPreferences(QWidget *parent = 0);
    QString getMinimumSearchWeight();
    void saveValues();
    
signals:
    
public slots:
    
};

#endif // SEARCHPREFERENCES_H

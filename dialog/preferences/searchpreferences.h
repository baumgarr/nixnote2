#ifndef SEARCHPREFERENCES_H
#define SEARCHPREFERENCES_H

#include <QWidget>
#include <QSpinBox>

class SearchPreferences : public QWidget
{
    Q_OBJECT
private:
    QSpinBox *weight;

public:
    explicit SearchPreferences(QWidget *parent = 0);
    QString getMinimumSearchWeight();
    void saveValues();
    
signals:
    
public slots:
    
};

#endif // SEARCHPREFERENCES_H

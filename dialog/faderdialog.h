//***********************************************************************************************
//* This code was shamelessly copied from the Qt Centre forums:
//* http://www.qtcentre.org/threads/56737-Implementing-fade-in-fade-out-for-a-modal-QDialog
//***********************************************************************************************


#ifndef FADERDIALOG_H
#define FADERDIALOG_H

#include <QDialog>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QLabel>

class FaderDialog : public QDialog
{
    Q_OBJECT
private:
    QPropertyAnimation *mpFadeIn;
    QPropertyAnimation *mpFadeOut;
    int mResult;
    QVBoxLayout *layout;
    QLabel *text;

public:
    explicit FaderDialog(QWidget *parent = 0);
    void showEvent(QShowEvent *);
    void done(int result);
    void setText(QString value);

signals:

public slots:
    void onFadeOutFinished();
    void done();

};

#endif // FADERDIALOG_H

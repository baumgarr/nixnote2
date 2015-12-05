//***********************************************************************************************
//* This code was shamelessly copied from the Qt Centre forums:
//* http://www.qtcentre.org/threads/56737-Implementing-fade-in-fade-out-for-a-modal-QDialog
//***********************************************************************************************

#include "faderdialog.h"

FaderDialog::FaderDialog( QWidget * parent ) : QDialog( parent ), mResult( 0 ) {
    setWindowFlags(Qt::SplashScreen);

    layout = new QVBoxLayout();
    text = new QLabel();
    layout->addWidget(text);
    setLayout(layout);

    // Create the fade-in / fade-out animators
    mpFadeIn = new QPropertyAnimation( this, "windowOpacity" );
    mpFadeIn->setDuration( 2000 );
    mpFadeIn->setStartValue( 0.0 );
    mpFadeIn->setEndValue( 1.0 );

    mpFadeOut = new QPropertyAnimation( this, "windowOpacity" );
    mpFadeOut->setDuration( 2000 );
    mpFadeOut->setStartValue( 1.0 );
    mpFadeOut->setEndValue( 0.0 );

    connect( mpFadeIn, SIGNAL(finished()), this, SLOT(done()));
    connect( mpFadeOut, SIGNAL( finished() ), this, SLOT( onFadeOutFinished() ) );
}


void FaderDialog::setText(QString value) {
    this->text->setText(value);
}

void FaderDialog::showEvent( QShowEvent * )
{
    mpFadeIn->start();
}


void FaderDialog::done() {
    this->done(0);
}

void FaderDialog::done( int result )
{
    mResult = result;  // remember the result in a member variable
    mpFadeOut->start();
}

void FaderDialog::onFadeOutFinished()
{
    QDialog::done( mResult );  // now call the real done() slot
}

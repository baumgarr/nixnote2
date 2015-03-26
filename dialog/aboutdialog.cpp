#include "aboutdialog.h"
#include <QWebView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "global.h"

extern Global global;

AboutDialog::AboutDialog(QDialog *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);
    QWebView *page = new QWebView();
    QPushButton *okButton = new QPushButton();
    okButton->setText(tr("OK"));
    mainLayout->addWidget(page);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QSpacerItem *spacer1 = new QSpacerItem(100000,1, QSizePolicy::Maximum);
    QSpacerItem *spacer2 = new QSpacerItem(100000,1, QSizePolicy::Maximum);
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacerItem(spacer2);
    mainLayout->addLayout(buttonLayout);
    this->setLayout(mainLayout);
    QString file = "file://" + global.fileManager.getProgramDirPath("") + "/help/about.html";
    page->load(file);
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    this->resize(600,500);
    this->setFont(global.getGuiFont(font()));
}


#include "databasestatus.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "sql/notetable.h"
#include "sql/resourcetable.h"

DatabaseStatus::DatabaseStatus(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    // Setup window layout, title, & icon
    this->setWindowTitle(tr("Database Status"));
    QGridLayout *textGrid = new QGridLayout();
    setLayout(vBoxLayout);

    NoteTable ntable;
    ResourceTable rtable;
    qint32 totalNotes = ntable.getCount();
    qint32 totalResources = rtable.getCount();
    qint32 unindexedNotes = ntable.getUnindexedCount();
    qint32 unindexedResources = rtable.getUnindexedCount();

    textGrid->addWidget(new QLabel(tr("Total Notes:")),1,1);
    textGrid->addWidget(new QLabel(QString::number(totalNotes)), 1,2);
    textGrid->addWidget(new QLabel(tr("Total Resources:")), 2,1);
    textGrid->addWidget(new QLabel(QString::number(totalResources)), 2, 2);
    textGrid->addWidget(new QLabel(tr("Unindexed Notes:")), 3,1);
    textGrid->addWidget(new QLabel(QString::number(unindexedNotes)),3,2);
    textGrid->addWidget(new QLabel(tr("Unindexed Resources:")), 4,1);
    textGrid->addWidget(new QLabel(QString::number(unindexedResources)),4,2);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    ok = new QPushButton(tr("OK"),this);
    connect(ok, SIGNAL(clicked()), this, SLOT(okPushed()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(ok);
    buttonLayout->addStretch();

    vBoxLayout->addLayout(textGrid);
    vBoxLayout->addLayout(buttonLayout);
}



// OK button pushed, close the window
void DatabaseStatus::okPushed() {
    this->close();
}

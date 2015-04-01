#include "themepreview.h"
#include "global.h"

#include <QGridLayout>
#include <QLabel>

extern Global global;


ThemePreview::ThemePreview(const QString file, QDialog *parent) :
    QDialog(parent)
{
    QLayout *layout = new QGridLayout(this);
    QLabel *label = new QLabel(this);
    this->setLayout(layout);
    layout->addWidget(label);
    label->setPixmap(QPixmap(file));
}

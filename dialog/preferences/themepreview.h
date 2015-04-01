#ifndef THEMEPREVIEW_H
#define THEMEPREVIEW_H

#include <QDialog>
#include <QPixmap>

class ThemePreview : public QDialog
{
    Q_OBJECT
public:
    explicit ThemePreview(const QString file, QDialog *parent = 0);

signals:

public slots:

};

#endif // THEMEPREVIEW_H

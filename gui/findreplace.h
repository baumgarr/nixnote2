#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QWebPage>

class FindReplace : public QWidget
{
    Q_OBJECT


public:
    explicit FindReplace(QWidget *parent = 0);
    void showFind();
    void showFindReplace();
    QLineEdit *findLine;
    QPushButton *closeButton;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QLineEdit *replaceLine;
    QCheckBox *caseSensitive;
    QWebPage::FindFlags getCaseSensitive();
    
signals:
    
public slots:
    void textChanged();
    
};

#endif // FINDREPLACE_H

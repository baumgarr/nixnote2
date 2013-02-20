#ifndef NTABLEVIEWHEADER_H
#define NTABLEVIEWHEADER_H

#include <QTableView>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

class NTableViewHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit NTableViewHeader(Qt::Orientation orientation, QWidget *parent = 0);
    QAction *createdDateAction;
    QAction *changedDateAction;
    QAction *subjectDateAction;
    QAction *tagsAction;
    QAction *titleAction;
    QAction *notebookAction;
    QAction *synchronizedAction;
    QAction *sourceAction;
    QAction *authorAction;
    QAction *urlAction;
    QAction *altitudeAction;
    QAction *longitudeAction;
    QAction *latitudeAction;
    QAction *hasTodoAction;
    QAction *hasEncryptionAction;
    QAction *sizeAction;
    void checkActions();


signals:
    void setColumnVisible(int position, bool visible);

public slots:
    void saveSort(int pos, Qt::SortOrder order);
    void saveHeader();

    void createdDateChecked(bool);
    void changedDateChecked(bool);
    void subjectDateChecked(bool);
    void tagsChecked(bool);
    void titleChecked(bool);
    void notebookChecked(bool);
    void synchronizedChecked(bool);
    void sourceChecked(bool);
    void authorChecked(bool);
    void urlChecked(bool);
    void altitudeChecked(bool);
    void longitudeChecked(bool);
    void latitudeChecked(bool);
    void hasTodoChecked(bool);
    void hasEncryptionChecked(bool);
    void sizeChecked(bool);
};

#endif // NTABLEVIEWHEADER_H

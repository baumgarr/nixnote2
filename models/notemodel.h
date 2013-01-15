#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QSqlTableModel>

class NoteModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit NoteModel(QObject *parent = 0);
    ~NoteModel();
    //int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void createTable();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

signals:

public slots:

};

#endif // NOTEMODEL_H

#ifndef FILTERENGINE_H
#define FILTERENGINE_H

#include <QObject>
#include "filtercriteria.h"

class FilterEngine : public QObject
{
    Q_OBJECT
private:
    void filterNotebook(FilterCriteria *criteria);
    void filterIndividualNotebook(QString &guid);
    void filterStack(QString& stack);
    void filterTags(FilterCriteria *criteria);
    void filterTrash(FilterCriteria *criteria);
    void filterSearchString(FilterCriteria *criteria);
    void filterSearchStringAll(QStringList list);
    void splitSearchTerms(QStringList &list, QString search);
    void filterSearchStringNotebookAll(QString string);
    void filterSearchTodoAll(QStringList list);
    void filterSearchStringTodoAll(QString string);
    bool anyFlagSet;

public:
    explicit FilterEngine(QObject *parent = 0);
    void filter();
    
signals:
    
public slots:
    
};

#endif // FILTERENGINE_H

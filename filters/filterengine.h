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
    void filterAttributes(FilterCriteria *criteria);
    void filterSearchString(FilterCriteria *criteria);
    void filterSearchStringAll(QStringList list);
    void splitSearchTerms(QStringList &list, QString search);
    void filterSearchStringNotebookAll(QString string);
    void filterSearchTodoAll(QStringList list);
    void filterSearchStringTodoAll(QString string);
    void filterSearchStringTagAll(QString string);
    void filterSearchStringIntitleAll(QString string);
    void filterSearchStringResourceAll(QString string);
    void filterSearchStringCoordinatesAll(QString string, int key);
    void filterSearchStringAuthorAll(QString string);
    void filterSearchStringSourceAll(QString string);
    void filterSearchStringSourceApplicationAll(QString string);
    void filterSearchStringContentClassAll(QString string);
    void filterSearchStringPlaceNameAll(QString string);
    void filterSearchStringResourceRecognitionTypeAll(QString string);
    QDateTime calculateDateTime(QString string);
    void filterSearchStringDateAll(QString string);

    void filterSearchStringAny(QStringList list);
    void filterSearchStringNotebookAny(QString string);
    void filterSearchStringTodoAny(QString string);
    void filterSearchStringTagAny(QString string);
    void filterSearchStringIntitleAny(QString string);
    void filterSearchStringResourceAny(QString string);
    void filterSearchStringCoordinatesAny(QString string, int key);
    void filterSearchStringAuthorAny(QString string);
    void filterSearchStringDateAny(QString string);
    void filterSearchStringSourceAny(QString string);
    void filterSearchStringSourceApplicationAny(QString string);
    void filterSearchStringContentClassAny(QString string);
    void filterSearchStringResourceRecognitionTypeAny(QString string);
    bool anyFlagSet;

public:
    explicit FilterEngine(QObject *parent = 0);
    void filter();
    
signals:
    
public slots:
    
};

#endif // FILTERENGINE_H

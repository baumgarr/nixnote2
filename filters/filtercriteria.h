#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include <QObject>

#include "gui/nnotebookviewitem.h"
#include "gui/ntagviewitem.h"
#include "gui/nsearchviewitem.h"

#include <QTreeWidgetItem>
#include <QList>

class FilterCriteria : public QObject
{
    Q_OBJECT
private:
    bool valueSet;
    QTreeWidgetItem *notebook;
    bool notebookIsSet;

    QList<QTreeWidgetItem*> tags;
    bool tagsIsSet;

    NSearchViewItem *savedSearch;
    bool savedSearchIsSet;

    QTreeWidgetItem *attribute;
    bool attributeIsSet;

    QString searchString;
    bool searchStringIsSet;

    bool deletedOnly;
    bool deletedOnlyIsSet;

    qint32 content;
    bool contentIsSet;

    QList<qint32> selectedNotes;
    bool selectedNotesIsSet;

public:
    explicit FilterCriteria(QObject *parent = 0);
    bool isSet();

    QTreeWidgetItem* getNotebook();
    void setNotebook(QTreeWidgetItem &item);
    bool isNotebookSet();
    bool resetNotebook;

    QList<QTreeWidgetItem*> getTags();
    void setTags(QList<QTreeWidgetItem*> &items);
    bool isTagsSet();
    bool resetTags;

    NSearchViewItem* getSavedSearch();
    void setSavedSearch(NSearchViewItem &item);
    bool isSavedSearchSet();
    bool resetSavedSearch;

    QTreeWidgetItem* getAttribute();
    void setAttribute(QTreeWidgetItem &item);
    bool isAttributeSet();
    bool resetAttribute;

    bool getDeletedOnly();
    void setDeletedOnly(bool value);
    bool isDeletedOnlySet();
    bool resetDeletedOnly;

    qint32 getContent();
    void setContent(qint32 content);
    bool isContentSet();
    bool resetContent;

    void getSelectedNotes(QList<qint32> &items);
    void setSelectedNotes(QList<qint32> &items);
    bool isSelectedNotesSet();
    bool resetSelectedNotes;

    QString getSearchString();
    void setSearchString(QString string);
    bool isSearchStringSet();
    bool resetSearchString;

    void duplicate(FilterCriteria &criteria);


signals:

public slots:

};

#endif // FILTERCRITERIA_H

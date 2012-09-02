#include "filtercriteria.h"

FilterCriteria::FilterCriteria(QObject *parent) :
    QObject(parent)
{
    valueSet = false;
    notebook = NULL;
    notebookIsSet = false;
    tags.empty();
    tagsIsSet = false;
    savedSearch = NULL;
    savedSearchIsSet = false;
    searchString = "";
    searchStringIsSet = false;
    attribute = NULL;
    attributeIsSet = false;
    deletedOnly = false;
    deletedOnlyIsSet = false;
    content = -1;
    contentIsSet = false;


    resetNotebook = false;
    resetTags = false;
    resetSavedSearch = false;
    resetAttribute = false;
    resetDeletedOnly = false;
    resetContent = false;
    resetSearchString = false;
    selectedNotesIsSet = false;
}


bool FilterCriteria::isSet() {
    return valueSet;
}


QTreeWidgetItem* FilterCriteria::getNotebook() {
    return notebook;
}

void FilterCriteria::setNotebook(QTreeWidgetItem &item) {
    notebook = &item;
    notebookIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isNotebookSet() {
    return notebookIsSet;
}





QList<QTreeWidgetItem*> FilterCriteria::getTags() {
    return tags;
}

void FilterCriteria::setTags(QList<QTreeWidgetItem*> &items) {
    tags.append(items);
    tagsIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isTagsSet() {
    return tagsIsSet;
}





NSearchViewItem* FilterCriteria::getSavedSearch() {
    return savedSearch;
}

void FilterCriteria::setSavedSearch(NSearchViewItem &item) {
    savedSearch = &item;
    savedSearchIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isSavedSearchSet() {
    return savedSearchIsSet;
}






QTreeWidgetItem* FilterCriteria::getAttribute() {
    return attribute;
}

void FilterCriteria::setAttribute(QTreeWidgetItem &item) {
    attribute = &item;
    attributeIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isAttributeSet() {
    return attributeIsSet;
}




bool FilterCriteria::getDeletedOnly() {
    return deletedOnly;
}

void FilterCriteria::setDeletedOnly(bool value) {
    deletedOnly = value;
    deletedOnlyIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isDeletedOnlySet() {
    return deletedOnlyIsSet;
}



qint32 FilterCriteria::getContent() {
    return content;
}

void FilterCriteria::setContent(qint32 item) {
    content = item;
    contentIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isContentSet() {
    return contentIsSet;
}








void FilterCriteria::getSelectedNotes(QList<qint32> &items) {
    items.clear();
    if (selectedNotesIsSet) {
        for (qint32 i=0; i<selectedNotes.size(); i++) {
            items.append(selectedNotes.at(i));
        }
    }
}

void FilterCriteria::setSelectedNotes(QList<qint32> &items) {
    selectedNotesIsSet = true;
    valueSet = true;
    selectedNotes.clear();
    for (qint32 i=0; i<items.size(); i++) {
        selectedNotes.append(items.at(i));
    }
}

bool FilterCriteria::isSelectedNotesSet() {
    return selectedNotesIsSet;
}




QString FilterCriteria::getSearchString() {
    return searchString;
}

void FilterCriteria::setSearchString(QString string) {
    searchString = string;
    searchStringIsSet = true;
    valueSet = true;
}

bool FilterCriteria::isSearchStringSet() {
    return searchStringIsSet;
}


void FilterCriteria::duplicate(FilterCriteria &newFilter) {
    if (attributeIsSet)
        newFilter.setAttribute(*attribute);

    if (contentIsSet)
        newFilter.setContent(content);

    if (deletedOnlyIsSet)
        newFilter.setDeletedOnly(deletedOnly);

    if (selectedNotesIsSet)
        newFilter.setSelectedNotes(selectedNotes);

    if (tagsIsSet)
        newFilter.setTags(tags);

    if (savedSearch)
        newFilter.setSavedSearch(*savedSearch);

    if (searchStringIsSet)
        newFilter.setSearchString(searchString);

    if (attributeIsSet)
        newFilter.setAttribute(*attribute);

    newFilter.resetNotebook = resetNotebook;
    newFilter.resetTags = resetTags;
    newFilter.resetSavedSearch = resetSavedSearch;
    newFilter.resetAttribute = resetAttribute;
    newFilter.resetDeletedOnly = resetDeletedOnly;
    newFilter.resetContent = resetContent;
    newFilter.resetSearchString = resetSearchString;

}

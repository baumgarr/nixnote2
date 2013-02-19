#include "ntagview.h"
#include "global.h"
#include "ntagviewitem.h"
#include "sql/tagtable.h"
#include "dialog/tagproperties.h"
#include "filters/filtercriteria.h"
#include "gui/ntagviewdelegate.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QByteArray>
#include <QtSql>
#include <QMessageBox>



#define NAME_POSITION 0

extern Global global;

// Constructor
NTagView::NTagView(QWidget *parent) :
    QTreeWidget(parent)
{
    QFont f = this->font();
    f.setPointSize(8);
    this->setFont(f);

    filterPosition = 0;
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
    this->setStyleSheet("QTreeWidget { border: none; background-color:transparent;}");

    // Build the root item
    QIcon icon(":tag.png");
    root = new NTagViewItem(this);
    root->setIcon(NAME_POSITION,icon);
    root->setData(NAME_POSITION, Qt::UserRole, "root");
    root->setData(NAME_POSITION, Qt::DisplayRole, tr("Tags"));
    root->setExpanded(true);

    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    this->rebuildTagTreeNeeded = true;
    this->loadData();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    setAcceptDrops(true);
    setDragEnabled(true);

    global.settings->beginGroup("SaveState");
    hideUnassigned = global.settings->value("hideUnassigned", false).toBool();
    global.settings->endGroup();


    addAction = context.addAction(tr("Create New Tag"));
    addAction->setShortcut(QKeySequence(Qt::Key_Insert));
    addAction->setShortcutContext(Qt::WidgetShortcut);

    addShortcut = new QShortcut(this);
    addShortcut->setKey(QKeySequence(Qt::Key_Insert));
    addShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    deleteAction = context.addAction(tr("Delete"));
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    deleteShortcut = new QShortcut(this);
    deleteShortcut->setKey(QKeySequence(Qt::Key_Delete));
    deleteShortcut->setContext(Qt::WidgetShortcut);

    renameAction = context.addAction(tr("Rename"));
    renameAction->setShortcutContext(Qt::WidgetShortcut);

    renameShortcut = new QShortcut(this);
    renameShortcut->setKey(QKeySequence(Qt::Key_F2));
    renameShortcut->setContext(Qt::WidgetShortcut);

    context.addSeparator();
    hideUnassignedAction = context.addAction(tr("Hide Unassigned"));
    hideUnassignedAction->setCheckable(true);
    hideUnassignedAction->setChecked(hideUnassigned);
    connect(hideUnassignedAction, SIGNAL(triggered()), this, SLOT(hideUnassignedTags()));

    context.addSeparator();
    propertiesAction = context.addAction(tr("Properties"));

    connect(addAction, SIGNAL(triggered()), this, SLOT(addRequested()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteRequested()));
    connect(renameAction, SIGNAL(triggered()), this, SLOT(renameRequested()));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(propertiesRequested()));

    connect(addShortcut, SIGNAL(activated()), this, SLOT(addRequested()));
    connect(deleteShortcut, SIGNAL(activated()), this, SLOT(deleteRequested()));
    connect(renameShortcut, SIGNAL(activated()), this, SLOT(renameRequested()));

    this->setItemDelegate(new NTagViewDelegate());
}


// Destructor
NTagView::~NTagView() {
    delete root;
}


void NTagView::calculateHeight()
{
    int h = 0;

    int topLevelCount = topLevelItemCount();

    for(int i = 0;i < topLevelCount;i++)
    {
        QTreeWidgetItem * item = topLevelItem(i);
        h += calculateHeightRec(item);
        h += item->sizeHint(0).height() + 5;
    }

    if(h != 0)
    {
//        h += header()->sizeHint().height();

        setMinimumHeight(h);
        setMaximumHeight(h);
    }
}

int NTagView::calculateHeightRec(QTreeWidgetItem * item)
{
    if(!item)
        return 0;

    QModelIndex index = indexFromItem(item);

    if(!item->isExpanded())
    {
        return rowHeight(index);
    }

    int h = item->sizeHint(0).height() + 2 + rowHeight(index);
    int childCount = item->childCount();
    for(int i = 0; i<childCount; i++) {
        h += calculateHeightRec(item->child(i));
    }
    return h;
}



// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NTagView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    if (selected)
        selectionModel()->select(item, QItemSelectionModel::Deselect);

    for (int i=0; i<this->selectedItems() .size(); i++) {
        if (this->selectedIndexes().at(i).data(Qt::UserRole) == "root") {
            selectionModel()->select(this->selectedIndexes().at(i), QItemSelectionModel::Deselect);
        }
    }
}



// Load up the data from the database
void NTagView::loadData() {
    QSqlQuery query;
    TagTable tagTable;
    query.exec("Select lid, name, parent_gid from TagModel order by name");
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString parentGid = query.value(2).toString();

        if (!tagTable.isDeleted(lid)) {
            NTagViewItem *newWidget = new NTagViewItem();
            newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
            newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
            this->dataStore.insert(lid, newWidget);
            newWidget->parentGuid = parentGid;
            newWidget->parentLid = tagTable.getLid(parentGid);
            root->addChild(newWidget);
        }
    }
    this->rebuildTree();
}

void NTagView::rebuildTree() {
    if (!this->rebuildTagTreeNeeded)
        return;

    QHashIterator<qint32, NTagViewItem *> i(dataStore);
    TagTable tagTable;

    while (i.hasNext()) {
        i.next();
        NTagViewItem *widget = i.value();
        if (widget != NULL && widget->parentGuid != "") {
            if (widget->parentLid == 0) {
                widget->parentLid = tagTable.getLid(widget->parentGuid);
            }
            NTagViewItem *parent = dataStore[widget->parentLid];
            widget->parent()->removeChild(widget);
            if (parent != NULL) {
                parent->childrenGuids.append(i.key());
                parent->addChild(widget);
            }
        }
    }
    this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
    this->rebuildTagTreeNeeded = false;
    this->resetSize();
}

void NTagView::tagUpdated(qint32 lid, QString name) {
    this->rebuildTagTreeNeeded = true;

    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        TagTable tagTable;
        Tag tag;
        tagTable.get(tag, lid);
        NTagViewItem *newWidget = dataStore.value(lid);
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->parentGuid = QString::fromStdString(tag.parentGuid);
        newWidget->parentLid = tagTable.getLid(tag.parentGuid);
        root->addChild(newWidget);
    } else {
        TagTable tagTable;
        Tag tag;
        tagTable.get(tag, lid);
        NTagViewItem *newWidget = new NTagViewItem();
        newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
        newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
        newWidget->parentGuid = QString::fromStdString(tag.parentGuid);
        newWidget->parentLid = tagTable.getLid(tag.parentGuid);
        this->dataStore.insert(lid, newWidget);
        root->addChild(newWidget);
        if (this->dataStore.count() == 1) {
            this->expandAll();
        }
    }
    resetSize();
    this->sortByColumn(NAME_POSITION);
}


void NTagView::resetSize() {
    calculateHeight();
}


void NTagView::buildSelection() {
    QLOG_TRACE() << "Inside NTagView::buildSelection()";

    QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
    if (selectedItems.size() > 0 && selectedItems[0]->data(0,Qt::UserRole) == "root")
        return;

    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (global.filterPosition+1 < global.filterCriteria.size()) {
        while (global.filterPosition+1 < global.filterCriteria.size())
            global.filterCriteria.removeLast();
    }

    filterPosition++;
    FilterCriteria *newFilter = new FilterCriteria();
    global.filterCriteria.push_back(newFilter);
    global.filterPosition++;

    if (selectedItems.size() > 0) {
        newFilter->setTags(selectedItems);
    }
    newFilter->resetAttribute = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetSavedSearch = true;
    newFilter->resetTags = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving NTagView::buildSelection()";
}


void NTagView::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetTags; i++) {
            selectedItems[i]->setSelected(false);
        }


        for (int i=0; i<criteria->getTags().size() && criteria->isTagsSet(); i++) {
            criteria->getTags()[i]->setSelected(true);
        }
    }
    root->setSelected(false);
    filterPosition = global.filterPosition;

    blockSignals(false);
}

// Add a new tag to the table
void NTagView::addNewTag(qint32 lid) {
    TagTable tagTable;
    Tag newTag;
    tagTable.get(newTag, lid);
    if (newTag.__isset.guid) {
        tagUpdated(lid, QString::fromStdString(newTag.name));
    }
}

// Accept the drag move event if possible
void NTagView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        if (event->answerRect().intersects(childrenRect()))
            event->acceptProposedAction();
        return;
    }
}


// Drag tag event.  Determine if dragging is even possible
void NTagView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-nixnote-note")) {
        event->accept();
        return;
    }
    if (event->source() == this) {
        if (global.tagBehavior() == "HideInactiveCount") {
            event->ignore();
            return;
        }
        event->accept();
        return;
    }
    event->ignore();
}


// Handle what happens when something is dropped onto a tag item
bool NTagView::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) {
    /* suppress unused */
    action=action;
    index=index;

    QLOG_DEBUG() << "Dropping mime: " << data->formats();


    if (data->hasFormat("application/x-nixnote-tag")) {

        QTreeWidgetItem *newChild;

        // If there is no parent, then they are trying to drop to the top level, which isn't permitted
        if (parent == NULL)
            return false;

        QByteArray d = data->data("application/x-nixnote-tag");
        qint32 lid = d.toInt();
        if (lid == 0)
            return false;

        newChild = new QTreeWidgetItem(parent);
        qint32 parentLid = parent->data(NAME_POSITION, Qt::UserRole).toInt();
        Tag tag;
        TagTable tagTable;
        tagTable.get(tag, lid);
        QString guid;
        tagTable.getGuid(guid, parentLid);
        tag.parentGuid = guid.toStdString();
        tagTable.update(tag, true);

        copyTreeItem(currentItem(), newChild);
        currentItem()->setHidden(true);
        this->sortByColumn(NAME_POSITION, Qt::AscendingOrder);
        sortItems(NAME_POSITION, Qt::AscendingOrder);
        return true;
    }
    return false;
}

// Implement of dropEvent so dropMimeData gets called
void NTagView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
}


// Copy an individual item within the tree.  I need to do this because
// Qt doesn't call the dropMimeData on a move, just a copy.
void NTagView::copyTreeItem(QTreeWidgetItem *source, QTreeWidgetItem *target) {
    QLOG_DEBUG() << "In copytreeitem";
    target->setData(NAME_POSITION, Qt::DisplayRole, source->data(NAME_POSITION, Qt::DisplayRole));
    target->setData(NAME_POSITION, Qt::UserRole, source->data(NAME_POSITION, Qt::UserRole));

    for (int i=0; i<source->childCount(); i++) {
        QTreeWidgetItem *newChild = new QTreeWidgetItem(target);
        copyTreeItem(source->child(i), newChild);
        source->child(i)->setHidden(true);
    }
    return;
}


void NTagView::mouseMoveEvent(QMouseEvent *event)
{
    if (currentItem() == NULL)
        return;

    QLOG_DEBUG() << "Mouse Move 1";
    if (!(event->buttons() & Qt::LeftButton))
        return;

    QLOG_DEBUG() << "Mouse Move 2";

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setData("application/x-nixnote-tag", currentItem()->data(NAME_POSITION, Qt::UserRole).toByteArray());
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction | Qt::MoveAction);
}



void NTagView::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.size() == 0) {
        propertiesAction->setEnabled(false);
        deleteAction->setEnabled(false);
        renameAction->setEnabled(false);
    } else {
        propertiesAction->setEnabled(true);
        deleteAction->setEnabled(true);
        renameAction->setEnabled(true);
    }
    context.exec(event->globalPos());
}



void NTagView::addRequested() {
    TagProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    dialog.setLid(0);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    TagTable table;
    NTagViewItem *newWidget = new NTagViewItem();
    QString name = dialog.name.text().trimmed();
    qint32 lid = table.findByName(name);
    newWidget->setData(NAME_POSITION, Qt::DisplayRole, name);
    newWidget->setData(NAME_POSITION, Qt::UserRole, lid);
    this->dataStore.insert(lid, newWidget);
    root->addChild(newWidget);
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);

    // Now add it to the datastore
    dataStore.insert(lid, newWidget);
    emit(tagAdded(lid));
}

void NTagView::propertiesRequested() {
    TagProperties dialog;
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    QString oldName = items[0]->data(NAME_POSITION, Qt::DisplayRole).toString();
    dialog.setLid(lid);

    dialog.exec();
    if (!dialog.okPressed)
        return;

    QString newName = dialog.name.text().trimmed();
    if (newName != oldName) {
        items[0]->setData(NAME_POSITION, Qt::DisplayRole, dialog.name.text().trimmed());

        this->sortItems(NAME_POSITION, Qt::AscendingOrder);
        resetSize();
        this->sortByColumn(NAME_POSITION);
        emit(tagRenamed(lid, oldName, newName));
    }
}

void NTagView::deleteRequested() {
    QList<QTreeWidgetItem*> items = selectedItems();

    qint32 lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    if (global.confirmDeletes()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Are you sure you want to delete this tag?"));
        msgBox.setWindowTitle(tr("Verify Delete"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
            return;
    }
    TagTable table;
    table.deleteTag(lid);
    items[0]->setHidden(true);

    // Now remove it in the datastore
    dataStore.remove(items[0]->data(NAME_POSITION, Qt::UserRole).toInt());
    emit(tagDeleted(lid, items[0]->data(NAME_POSITION, Qt::DisplayRole).toString()));
}

void NTagView::renameRequested() {
    editor = new TreeWidgetEditor(this);
    connect(editor, SIGNAL(editComplete()), this, SLOT(editComplete()));

    QList<QTreeWidgetItem*> items = selectedItems();
    editor->setText(items[0]->text(NAME_POSITION));
    editor->lid = items[0]->data(NAME_POSITION, Qt::UserRole).toInt();
    editor->setTreeWidgetItem(items[0], NAME_POSITION);
    QFontMetrics m(font());
    editor->setMinimumHeight(m.height()+4);
    editor->setMaximumHeight(m.height()+4);
    setItemWidget(items[0], NAME_POSITION, editor);
    editor->setFocus();
}


void NTagView::editComplete() {
    QString text = editor->text().trimmed();
    qint32 lid = editor->lid;
    TagTable table;
    Tag tag;
    table.get(tag, lid);
    QString oldName = QString::fromStdString(tag.name);

    // Check that this tag doesn't already exist
    // if it exists, we go back to the original name
    qint32 check = table.findByName(text);
    if (check != 0) {
        NTagViewItem *item = dataStore[lid];
        item->setData(NAME_POSITION, Qt::DisplayRole, QString::fromStdString(tag.name));
    } else {
        tag.name = text.toStdString();
        table.update(tag, true);
    }
    delete editor;
    this->sortItems(NAME_POSITION, Qt::AscendingOrder);
    resetSize();
    this->sortByColumn(NAME_POSITION);
    emit(tagRenamed(lid, oldName, text));
}



void NTagView::tagExpunged(qint32 lid) {
    // Check if it already exists
    if (this->dataStore.contains(lid)) {
        NTagViewItem *item = this->dataStore.value(lid);
        this->removeItemWidget(item, 0);
        delete item;
    }
    this->resetSize();
}




void NTagView::updateTotals(qint32 lid, qint32 total) {
    if (dataStore.contains(lid)) {
        NTagViewItem *item = dataStore[lid];
        item->count = total;
    }
}


void NTagView::hideUnassignedTags() {
    NTagViewItem *item;
    if (hideUnassignedAction->isChecked())
        hideUnassigned = true;
    else
        hideUnassigned = false;

    // Save this option
    global.settings->beginGroup("SaveState");
    global.settings->setValue("hideUnassigned", hideUnassigned);
    global.settings->endGroup();

    // Unhide everything if they don't want items hidden.
    if (hideUnassigned != true) {
        for (int i=0; i<dataStore.size(); i++) {
            item = dataStore[i];
            if (item != NULL) {
                item->setHidden(false);
            }
        }
        resetSize();
        return;
    }

    // Start hiding unassigned tags
    for (int i=0; i<dataStore.size(); i++) {
        item = dataStore[i];
        if (item != NULL) {
            if (item->count == 0)
                item->setHidden(true);
            else
                item->setHidden(false);
        }
    }

    for (int i=0; i<dataStore.size(); i++) {
        item = dataStore[i];
        if (item != NULL && !item->isHidden()) {
            while(item->parentLid > 0) {
                item->parent()->setHidden(false);
                item = (NTagViewItem*)item->parent();
            }
        }
    }
    resetSize();
}

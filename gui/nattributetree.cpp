/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "nattributetree.h"
#include "global.h"
#include <QHeaderView>
#include <QPainter>

extern Global global;


NAttributeTree::NAttributeTree(QWidget *parent) :
    QTreeWidget(parent)
{
    QFont f = this->font();
    f.setPointSize(global.defaultGuiFontSize);
    this->setFont(f);

    filterPosition = -1;
    // setup options
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setRootIsDecorated(true);
    this->setSortingEnabled(false);
    this->header()->setVisible(false);
    //this->setStyleSheet("QTreeWidget {  border: none; background-color:transparent; }");

    // Build the root item
    root = new QTreeWidgetItem(this);
    QIcon icon(":attributes.png");
    root->setIcon(0,icon);
    root->setData(0, Qt::UserRole, "root");
    root->setData(0, Qt::DisplayRole, tr("Attributes"));
    QFont font = root->font(0);
    font.setBold(true);
    root->setFont(0,font);
    this->setMinimumHeight(1);
    this->addTopLevelItem(root);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(calculateHeight()));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(buildSelection()));

    // Allocate memory for everything needed

    createdRoot = new QTreeWidgetItem(root);
    lastUpdatedRoot = new QTreeWidgetItem(root);
    containsRoot = new QTreeWidgetItem(root);
    sourceRoot = new QTreeWidgetItem(root);

    createdSinceRoot = new QTreeWidgetItem(createdRoot);
    createdSinceToday = new QTreeWidgetItem(createdSinceRoot);
    createdSinceYesterday = new QTreeWidgetItem(createdSinceRoot);
    createdSinceThisWeek = new QTreeWidgetItem(createdSinceRoot);
    createdSinceLastWeek = new QTreeWidgetItem(createdSinceRoot);
    createdSinceThisMonth = new QTreeWidgetItem(createdSinceRoot);
    createdSinceLastMonth = new QTreeWidgetItem(createdSinceRoot);
    createdSinceThisYear = new QTreeWidgetItem(createdSinceRoot);
    createdSinceLastYear = new QTreeWidgetItem(createdSinceRoot);

    createdBeforeRoot = new QTreeWidgetItem(createdRoot);
    createdBeforeToday = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeYesterday = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeThisWeek = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeLastWeek = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeThisMonth = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeLastMonth = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeThisYear = new QTreeWidgetItem(createdBeforeRoot);
    createdBeforeLastYear = new QTreeWidgetItem(createdBeforeRoot);

    lastUpdatedSinceRoot = new QTreeWidgetItem(lastUpdatedRoot);
    lastUpdatedSinceToday = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceYesterday = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceThisWeek = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceLastWeek = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceThisMonth = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceLastMonth = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceThisYear = new QTreeWidgetItem(lastUpdatedSinceRoot);
    lastUpdatedSinceLastYear = new QTreeWidgetItem(lastUpdatedSinceRoot);

    lastUpdatedBeforeRoot = new QTreeWidgetItem(lastUpdatedRoot);
    lastUpdatedBeforeToday = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeYesterday = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeThisWeek = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeLastWeek = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeThisMonth = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeLastMonth = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeThisYear = new QTreeWidgetItem(lastUpdatedBeforeRoot);
    lastUpdatedBeforeLastYear = new QTreeWidgetItem(lastUpdatedBeforeRoot);

    containsImages = new QTreeWidgetItem(containsRoot);
    containsAudio = new QTreeWidgetItem(containsRoot);
    containsInk = new QTreeWidgetItem(containsRoot);
    containsEncryptedText = new QTreeWidgetItem(containsRoot);
    containsToDoItems = new QTreeWidgetItem(containsRoot);
    containsUnfinishedToDoItems = new QTreeWidgetItem(containsRoot);
    containsFinishedToDoItems = new QTreeWidgetItem(containsRoot);
    containsPDFDocument = new QTreeWidgetItem(containsRoot);
    containsAttachment = new QTreeWidgetItem(containsRoot);

    sourceEmailedToEvernote = new QTreeWidgetItem(sourceRoot);
    sourceEmail = new QTreeWidgetItem(sourceRoot);
    sourceWebPage = new QTreeWidgetItem(sourceRoot);
    sourceMobile = new QTreeWidgetItem(sourceRoot);
    sourceAnotherApplication = new QTreeWidgetItem(sourceRoot);


    // Start building selection criteria
    createdRoot->setText(0, tr("Created"));
    createdRoot->setData(0, Qt::UserRole, 0);

    createdSinceRoot->setText(0, tr("Since"));
    createdSinceRoot->setData(0, Qt::UserRole, 0);
    createdRoot->addChild(createdSinceRoot);

    createdSinceToday->setText(0, tr("Today"));
    createdSinceToday->setData(0, Qt::UserRole, CREATED_SINCE_TODAY);
    createdSinceRoot->addChild(createdSinceToday);

    createdSinceYesterday->setText(0, tr("Yesterday"));
    createdSinceYesterday->setData(0, Qt::UserRole, CREATED_SINCE_YESTERDAY);
    createdSinceRoot->addChild(createdSinceYesterday);

    createdSinceThisWeek->setText(0, tr("This week"));
    createdSinceThisWeek->setData(0, Qt::UserRole, CREATED_SINCE_THIS_WEEK);
    createdSinceRoot->addChild(createdSinceThisWeek);

    createdSinceLastWeek->setText(0, tr("Last week"));
    createdSinceLastWeek->setData(0, Qt::UserRole, CREATED_SINCE_LAST_WEEK);
    createdSinceRoot->addChild(createdSinceLastWeek);

    createdSinceThisMonth->setText(0, tr("This Month"));
    createdSinceThisMonth->setData(0, Qt::UserRole, CREATED_SINCE_THIS_MONTH);
    createdSinceRoot->addChild(createdSinceThisMonth);

    createdSinceLastMonth->setText(0, tr("Last Month"));
    createdSinceLastMonth->setData(0, Qt::UserRole, CREATED_SINCE_LAST_MONTH);
    createdSinceRoot->addChild(createdSinceLastMonth);

    createdSinceThisYear->setText(0, tr("This Year"));
    createdSinceThisYear->setData(0, Qt::UserRole, CREATED_SINCE_THIS_YEAR);
    createdSinceRoot->addChild(createdSinceThisYear);

    createdSinceLastYear->setText(0, tr("Last Year"));
    createdSinceLastYear->setData(0, Qt::UserRole, CREATED_SINCE_LAST_YEAR);
    createdSinceRoot->addChild(createdSinceLastYear);


    createdBeforeRoot->setText(0, tr("Before"));
    createdBeforeRoot->setData(0, Qt::UserRole, 0);
    createdRoot->addChild(createdBeforeRoot);

    createdBeforeToday->setText(0, tr("Today"));
    createdBeforeToday->setData(0, Qt::UserRole, CREATED_BEFORE_TODAY);
    createdBeforeRoot->addChild(createdBeforeToday);

    createdBeforeYesterday->setText(0, tr("Yesterday"));
    createdBeforeYesterday->setData(0, Qt::UserRole, CREATED_BEFORE_YESTERDAY);
    createdBeforeRoot->addChild(createdBeforeYesterday);

    createdBeforeThisWeek->setText(0, tr("This week"));
    createdBeforeThisWeek->setData(0, Qt::UserRole, CREATED_BEFORE_THIS_WEEK);
    createdBeforeRoot->addChild(createdBeforeThisWeek);

    createdBeforeLastWeek->setText(0, tr("Last week"));
    createdBeforeLastWeek->setData(0, Qt::UserRole, CREATED_BEFORE_LAST_WEEK);
    createdBeforeRoot->addChild(createdBeforeLastWeek);

    createdBeforeThisMonth->setText(0, tr("This Month"));
    createdBeforeThisMonth->setData(0, Qt::UserRole, CREATED_BEFORE_THIS_MONTH);
    createdBeforeRoot->addChild(createdSinceThisMonth);

    createdBeforeLastMonth->setText(0, tr("Last Month"));
    createdBeforeLastMonth->setData(0, Qt::UserRole, CREATED_BEFORE_LAST_MONTH);
    createdBeforeRoot->addChild(createdBeforeLastMonth);

    createdBeforeThisYear->setText(0, tr("This Year"));
    createdBeforeThisYear->setData(0, Qt::UserRole, CREATED_BEFORE_THIS_YEAR);
    createdBeforeRoot->addChild(createdBeforeThisYear);

    createdBeforeLastYear->setText(0, tr("Last Year"));
    createdBeforeLastYear->setData(0, Qt::UserRole, CREATED_BEFORE_LAST_YEAR);
    createdBeforeRoot->addChild(createdBeforeLastYear);





    //** Start doing the "Last Modified" selection criteria
    lastUpdatedRoot->setText(0, tr("Last Modified"));
    lastUpdatedRoot->setData(0, Qt::UserRole, 0);

    lastUpdatedSinceRoot->setText(0, tr("Since"));
    lastUpdatedSinceRoot->setData(0, Qt::UserRole, 0);
    lastUpdatedRoot->addChild(lastUpdatedSinceRoot);

    lastUpdatedSinceToday->setText(0, tr("Today"));
    lastUpdatedSinceToday->setData(0, Qt::UserRole, MODIFIED_SINCE_TODAY);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceToday);

    lastUpdatedSinceYesterday->setText(0, tr("Yesterday"));
    lastUpdatedSinceYesterday->setData(0, Qt::UserRole, MODIFIED_SINCE_YESTERDAY);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceYesterday);

    lastUpdatedSinceThisWeek->setText(0, tr("This week"));
    lastUpdatedSinceThisWeek->setData(0, Qt::UserRole, MODIFIED_SINCE_THIS_WEEK);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceThisWeek);

    lastUpdatedSinceLastWeek->setText(0, tr("Last week"));
    lastUpdatedSinceLastWeek->setData(0, Qt::UserRole, MODIFIED_SINCE_LAST_WEEK);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceLastWeek);

    lastUpdatedSinceThisMonth->setText(0, tr("This Month"));
    lastUpdatedSinceThisMonth->setData(0, Qt::UserRole, MODIFIED_SINCE_THIS_MONTH);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceThisMonth);

    lastUpdatedSinceLastMonth->setText(0, tr("Last Month"));
    lastUpdatedSinceLastMonth->setData(0, Qt::UserRole, MODIFIED_SINCE_LAST_MONTH);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceLastMonth);

    lastUpdatedSinceThisYear->setText(0, tr("This Year"));
    lastUpdatedSinceThisYear->setData(0, Qt::UserRole, MODIFIED_SINCE_THIS_YEAR);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceThisYear);

    lastUpdatedSinceLastYear->setText(0, tr("Last Year"));
    lastUpdatedSinceLastYear->setData(0, Qt::UserRole, MODIFIED_SINCE_LAST_YEAR);
    lastUpdatedSinceRoot->addChild(lastUpdatedSinceLastYear);


    lastUpdatedBeforeRoot->setText(0, tr("Before"));
    lastUpdatedBeforeRoot->setData(0, Qt::UserRole, 0);
    lastUpdatedRoot->addChild(lastUpdatedBeforeRoot);

    lastUpdatedBeforeToday->setText(0, tr("Today"));
    lastUpdatedBeforeToday->setData(0, Qt::UserRole, MODIFIED_BEFORE_TODAY);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeToday);

    lastUpdatedBeforeYesterday->setText(0, tr("Yesterday"));
    lastUpdatedBeforeYesterday->setData(0, Qt::UserRole, MODIFIED_BEFORE_YESTERDAY);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeYesterday);

    lastUpdatedBeforeThisWeek->setText(0, tr("This week"));
    lastUpdatedBeforeThisWeek->setData(0, Qt::UserRole, MODIFIED_BEFORE_THIS_WEEK);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeThisWeek);

    lastUpdatedBeforeLastWeek->setText(0, tr("Last week"));
    lastUpdatedBeforeLastWeek->setData(0, Qt::UserRole, MODIFIED_BEFORE_LAST_WEEK);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeLastWeek);

    lastUpdatedBeforeThisMonth->setText(0, tr("This Month"));
    lastUpdatedBeforeThisMonth->setData(0, Qt::UserRole, MODIFIED_BEFORE_THIS_MONTH);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeThisMonth);

    lastUpdatedBeforeLastMonth->setText(0, tr("Last Month"));
    lastUpdatedBeforeLastMonth->setData(0, Qt::UserRole, MODIFIED_BEFORE_LAST_MONTH);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeLastMonth);

    lastUpdatedBeforeThisYear->setText(0, tr("This Year"));
    lastUpdatedBeforeThisYear->setData(0, Qt::UserRole, MODIFIED_BEFORE_THIS_YEAR);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeThisYear);

    lastUpdatedBeforeLastYear->setText(0, tr("Last Year"));
    lastUpdatedBeforeLastYear->setData(0, Qt::UserRole, MODIFIED_BEFORE_LAST_YEAR);
    lastUpdatedBeforeRoot->addChild(lastUpdatedBeforeLastYear);



    //*** Contains selection criteria
    this->containsRoot->setText(0, tr("Contains"));
    this->containsRoot->setData(0,Qt::UserRole, 0);

    this->containsImages->setText(0, tr("Images"));
    this->containsImages->setData(0, Qt::UserRole, CONTAINS_IMAGES);
    this->containsRoot->addChild(containsImages);

    this->containsAudio->setText(0, tr("Audio"));
    this->containsAudio->setData(0, Qt::UserRole, CONTAINS_AUDIO);
    this->containsRoot->addChild(containsAudio);

    this->containsInk->setText(0, tr("Ink"));
    this->containsInk->setData(0, Qt::UserRole, CONTAINS_INK);
    this->containsRoot->addChild(containsInk);

    this->containsEncryptedText->setText(0, tr("Encrypted Text"));
    this->containsEncryptedText->setData(0, Qt::UserRole, CONTAINS_ENCRYPTED_TEXT);
    this->containsRoot->addChild(containsEncryptedText);

    this->containsToDoItems->setText(0, tr("To-do items"));
    this->containsToDoItems->setData(0, Qt::UserRole, CONTAINS_TODO_ITEMS);
    this->containsRoot->addChild(containsUnfinishedToDoItems);

    this->containsUnfinishedToDoItems->setText(0, tr("Unfinished to-do items"));
    this->containsUnfinishedToDoItems->setData(0, Qt::UserRole, CONTAINS_UNFINISHED_TODO_ITEMS);
    this->containsRoot->addChild(containsUnfinishedToDoItems);

    this->containsFinishedToDoItems->setText(0, tr("Finished to-do items"));
    this->containsFinishedToDoItems->setData(0, Qt::UserRole, CONTAINS_FINISHED_TODO_ITEMS);
    this->containsRoot->addChild(containsFinishedToDoItems);

    this->containsPDFDocument->setText(0, tr("PDF document"));
    this->containsPDFDocument->setData(0, Qt::UserRole, CONTAINS_PDF_DOCUMENT);
    this->containsRoot->addChild(containsPDFDocument);

    this->containsAttachment->setText(0, tr("Attachment"));
    this->containsAttachment->setData(0, Qt::UserRole, CONTAINS_ATTACHMENT);
    this->containsRoot->addChild(containsAttachment);



    //*** Source selection criteria
    this->sourceRoot->setText(0, tr("Source"));
    this->sourceRoot->setData(0,Qt::UserRole, 0);

    this->sourceEmailedToEvernote->setText(0, tr("Emailed to Evernote"));
    this->sourceEmailedToEvernote->setData(0, Qt::UserRole, SOURCE_EMAILED_TO_EVERNOTE);
    this->sourceRoot->addChild(sourceEmailedToEvernote);

    this->sourceEmail->setText(0, tr("Email"));
    this->sourceEmail->setData(0, Qt::UserRole, SOURCE_EMAIL);
    this->sourceRoot->addChild(sourceEmail);

    this->sourceWebPage->setText(0, tr("Web page"));
    this->sourceWebPage->setData(0, Qt::UserRole, SOURCE_WEB_PAGE);
    this->sourceRoot->addChild(sourceWebPage);

    this->sourceMobile->setText(0, tr("Mobile"));
    this->sourceMobile->setData(0, Qt::UserRole, SOURCE_MOBILE);
    this->sourceRoot->addChild(sourceMobile);

    this->sourceAnotherApplication->setText(0, tr("Another application"));
    this->sourceAnotherApplication->setData(0, Qt::UserRole, SOURCE_ANOTHER_APPLICATION);
    this->sourceRoot->addChild(sourceAnotherApplication);


    // Add the options to the root
    root->addChild(createdRoot);
    root->addChild(lastUpdatedRoot);
    root->addChild(containsRoot);
    root->addChild(sourceRoot);
    root->setExpanded(true);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    this->setFrameShape(QFrame::NoFrame);
    expandedImage = new QImage(":expanded.png");
    collapsedImage = new QImage(":collapsed.png");
}

NAttributeTree::~NAttributeTree() {
    //delete root;
}


void NAttributeTree::calculateHeight()
{
    int h = 0;

    int topLevelCount = topLevelItemCount();

    for(int i = 0;i < topLevelCount;i++)    {
        QTreeWidgetItem * item = topLevelItem(i);
        h += calculateHeightRec(item);
        h += item->sizeHint(0).height() + 5;
    }

    if(h != 0)  {
        setMinimumHeight(h);
        setMaximumHeight(h);
    }
    this->setMaximumWidth(this->sizeHint().width());
}

int NAttributeTree::calculateHeightRec(QTreeWidgetItem * item)
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


void NAttributeTree::resetSize() {
    calculateHeight();
}



//*************************************************************
// This function is called when a user selects something
// within this tree.  It also emits a signal that will
// be picked up by the main nixnote program.
//*************************************************************
void NAttributeTree::buildSelection() {
    QLOG_TRACE() << "Inside NNotebookView::buildSelection()";

    QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
    if (selectedItems.size() > 0 && selectedItems[0]->data(0,Qt::UserRole) == "root") {
        blockSignals(true);
        root->setSelected(false);
        blockSignals(false);
        return;
    }

    for (int i=0; i<selectedItems.size(); i++) {
        QTreeWidgetItem *item = selectedItems[i];
        if (item->data(0, Qt::UserRole) == 0 || item->data(0,Qt::UserRole) == "root") {
            item->setSelected(false);
        }
    }

    selectedItems = this->selectedItems();
    if (selectedItems.size() == 0)
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
        newFilter->setAttribute(*(selectedItems[0]));
    }
    newFilter->resetSavedSearch = true;
    newFilter->resetDeletedOnly = true;
    newFilter->resetNotebook =true;
    newFilter->resetTags = true;
    newFilter->resetAttribute = true;
    newFilter->resetFavorite = true;
    newFilter->resetSearchString = true;

    emit updateSelectionRequested();

    QLOG_TRACE() << "Leaving NNotebookView::buildSelection()";
}


//*************************************************************
// This function is called from the main NixNote class.
// it will reset the items which are selected based upon
// what the user did somewhere else (outside this widget).
//*************************************************************
void NAttributeTree::updateSelection() {
    blockSignals(true);

    FilterCriteria *criteria = global.filterCriteria[global.filterPosition];
    if (global.filterPosition != filterPosition) {
        QList<QTreeWidgetItem*> selectedItems = this->selectedItems();
        for (int i=0; i<selectedItems.size() && criteria->resetAttribute; i++) {
            selectedItems[i]->setSelected(false);
        }


        if (criteria->isAttributeSet()) {
            criteria->getAttribute()->setSelected(true);
        }
    }
    filterPosition = global.filterPosition;

    blockSignals(false);
}


// This alows for the tree item to be toggled.  If the prior item is selected again
// it is deselected.  If it is the root item, we don't permit the selection.
void NAttributeTree::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    if (selected)
        selectionModel()->select(item, QItemSelectionModel::Deselect);

    for (int i=0; i<this->selectedItems() .size(); i++) {
        if (this->selectedIndexes().at(i).data(Qt::UserRole) == "root") {
            if (!root->isExpanded())
                root->setExpanded(true);
            selectionModel()->select(this->selectedIndexes().at(i), QItemSelectionModel::Deselect);
        }
    }
}



QSize NAttributeTree::sizeHint() {
    return QTreeView::sizeHint();
//    QSize sz = QTreeView::sizeHint();

//    if (root->isExpanded())
//        return sz;

//    QFontMetrics fm(root->font(0));;
//    int numWidth = fm.width(tr("Attributes"));

//    sz.setWidth(numWidth+44+root->icon(0).availableSizes().at(0).width());  // Add some extra at the end for totals
//    return sz;
}



void NAttributeTree::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
    if (!index.child(0,0).isValid())
        return;

    painter->save();
    if (isExpanded(index)) {
        int offset = rect.width()-expandedImage->width()-1;
        painter->drawImage(offset, rect.y(),*expandedImage);
    } else {
        int offset = rect.width()-collapsedImage->width()-1;
        painter->drawImage(offset, rect.y(),*collapsedImage);
    }
    painter->restore();
    return;
}


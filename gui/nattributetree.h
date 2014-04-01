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

#ifndef NATTRIBUTETREE_H
#define NATTRIBUTETREE_H

#include <QTreeWidget>
#include <QMouseEvent>

#define CREATED_SINCE_TODAY 1
#define CREATED_SINCE_YESTERDAY 2
#define CREATED_SINCE_THIS_WEEK 3
#define CREATED_SINCE_LAST_WEEK 4
#define CREATED_SINCE_THIS_MONTH 5
#define CREATED_SINCE_LAST_MONTH 6
#define CREATED_SINCE_THIS_YEAR 7
#define CREATED_SINCE_LAST_YEAR 8

#define CREATED_BEFORE_TODAY 101
#define CREATED_BEFORE_YESTERDAY 102
#define CREATED_BEFORE_THIS_WEEK 103
#define CREATED_BEFORE_LAST_WEEK 104
#define CREATED_BEFORE_THIS_MONTH 105
#define CREATED_BEFORE_LAST_MONTH 106
#define CREATED_BEFORE_THIS_YEAR 107
#define CREATED_BEFORE_LAST_YEAR 108

#define MODIFIED_SINCE_TODAY 201
#define MODIFIED_SINCE_YESTERDAY 202
#define MODIFIED_SINCE_THIS_WEEK 203
#define MODIFIED_SINCE_LAST_WEEK 204
#define MODIFIED_SINCE_THIS_MONTH 205
#define MODIFIED_SINCE_LAST_MONTH 206
#define MODIFIED_SINCE_THIS_YEAR 207
#define MODIFIED_SINCE_LAST_YEAR 208

#define MODIFIED_BEFORE_TODAY 301
#define MODIFIED_BEFORE_YESTERDAY 302
#define MODIFIED_BEFORE_THIS_WEEK 303
#define MODIFIED_BEFORE_LAST_WEEK 304
#define MODIFIED_BEFORE_THIS_MONTH 305
#define MODIFIED_BEFORE_LAST_MONTH 306
#define MODIFIED_BEFORE_THIS_YEAR 307
#define MODIFIED_BEFORE_LAST_YEAR 308

#define CONTAINS_IMAGES 401
#define CONTAINS_AUDIO 402
#define CONTAINS_INK 403
#define CONTAINS_ENCRYPTED_TEXT 404
#define CONTAINS_TODO_ITEMS 405
#define CONTAINS_UNFINISHED_TODO_ITEMS 406
#define CONTAINS_FINISHED_TODO_ITEMS 407
#define CONTAINS_PDF_DOCUMENT 408
#define CONTAINS_ATTACHMENT 409

#define SOURCE_EMAILED_TO_EVERNOTE 501
#define SOURCE_EMAIL 502
#define SOURCE_WEB_PAGE 503
#define SOURCE_MOBILE 504
#define SOURCE_ANOTHER_APPLICATION 505













class NAttributeTree : public QTreeWidget
{
    Q_OBJECT
private:

    QTreeWidgetItem *root;
    QTreeWidgetItem *createdRoot;
    QTreeWidgetItem *lastUpdatedRoot;
    QTreeWidgetItem *containsRoot;
    QTreeWidgetItem *sourceRoot;

    QTreeWidgetItem *createdSinceRoot;
    QTreeWidgetItem *createdSinceToday;
    QTreeWidgetItem *createdSinceYesterday;
    QTreeWidgetItem *createdSinceThisWeek;
    QTreeWidgetItem *createdSinceLastWeek;
    QTreeWidgetItem *createdSinceThisMonth;
    QTreeWidgetItem *createdSinceLastMonth;
    QTreeWidgetItem *createdSinceThisYear;
    QTreeWidgetItem *createdSinceLastYear;

    QTreeWidgetItem *createdBeforeRoot;
    QTreeWidgetItem *createdBeforeToday;
    QTreeWidgetItem *createdBeforeYesterday;
    QTreeWidgetItem *createdBeforeThisWeek;
    QTreeWidgetItem *createdBeforeLastWeek;
    QTreeWidgetItem *createdBeforeThisMonth;
    QTreeWidgetItem *createdBeforeLastMonth;
    QTreeWidgetItem *createdBeforeThisYear;
    QTreeWidgetItem *createdBeforeLastYear;

    QTreeWidgetItem *lastUpdatedSinceRoot;
    QTreeWidgetItem *lastUpdatedSinceToday;
    QTreeWidgetItem *lastUpdatedSinceYesterday;
    QTreeWidgetItem *lastUpdatedSinceThisWeek;
    QTreeWidgetItem *lastUpdatedSinceLastWeek;
    QTreeWidgetItem *lastUpdatedSinceThisMonth;
    QTreeWidgetItem *lastUpdatedSinceLastMonth;
    QTreeWidgetItem *lastUpdatedSinceThisYear;
    QTreeWidgetItem *lastUpdatedSinceLastYear;

    QTreeWidgetItem *lastUpdatedBeforeRoot;
    QTreeWidgetItem *lastUpdatedBeforeToday;
    QTreeWidgetItem *lastUpdatedBeforeYesterday;
    QTreeWidgetItem *lastUpdatedBeforeThisWeek;
    QTreeWidgetItem *lastUpdatedBeforeLastWeek;
    QTreeWidgetItem *lastUpdatedBeforeThisMonth;
    QTreeWidgetItem *lastUpdatedBeforeLastMonth;
    QTreeWidgetItem *lastUpdatedBeforeThisYear;
    QTreeWidgetItem *lastUpdatedBeforeLastYear;

    QTreeWidgetItem *containsImages;
    QTreeWidgetItem *containsAudio;
    QTreeWidgetItem *containsInk;
    QTreeWidgetItem *containsEncryptedText;
    QTreeWidgetItem *containsToDoItems;
    QTreeWidgetItem *containsUnfinishedToDoItems;
    QTreeWidgetItem *containsFinishedToDoItems;
    QTreeWidgetItem *containsPDFDocument;
    QTreeWidgetItem *containsAttachment;

    QTreeWidgetItem *sourceEmailedToEvernote;
    QTreeWidgetItem *sourceEmail;
    QTreeWidgetItem *sourceWebPage;
    QTreeWidgetItem *sourceMobile;
    QTreeWidgetItem *sourceAnotherApplication;

    int filterPosition;
    QImage *collapsedImage;
    QImage *expandedImage;


public:
    explicit NAttributeTree(QWidget *parent = 0);
    ~NAttributeTree();
    void updateSelection();
    void resetSize();
    void mousePressEvent(QMouseEvent *event);
    QSize sizeHint();
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

signals:
    void updateSelectionRequested();

public slots:

private slots:
    int calculateHeightRec(QTreeWidgetItem * item);
    void calculateHeight();
    void buildSelection();

};

#endif // NATTRIBUTETREE_H

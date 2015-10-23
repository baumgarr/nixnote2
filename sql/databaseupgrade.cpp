#include "databaseupgrade.h"
#include <QPair>
#include "sql/notetable.h"
#include "sql/notebooktable.h"
#include "sql/searchtable.h"
#include "sql/tagtable.h"
#include "sql/resourcetable.h"
#include "sql/linkednotebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/nsqlquery.h"
#include "global.h"


DatabaseUpgrade::DatabaseUpgrade(QObject *parent) :
    QObject(parent)
{
}


void DatabaseUpgrade::fixSql(bool toQt5) {
    QList<int> fields;

    fields.append(NOTE_ISPINNED);
    fields.append(NOTE_HAS_TODO_COMPLETED);
    fields.append(NOTE_HAS_TODO_UNCOMPLETED);
    fields.append(NOTE_ACTIVE);
    fields.append(NOTE_INDEX_NEEDED);
    fields.append(NOTE_EXPUNGED_FROM_TRASH);
    fields.append(NOTE_THUMBNAIL_NEEDED);
    fields.append(NOTE_ISPINNED);
    fields.append(NOTE_HAS_ENCRYPT);
    fields.append(NOTE_ISDIRTY);
    fields.append(NOTE_HAS_ENCRYPT);
    fields.append(NOTE_HAS_IMAGE);
    fields.append(NOTE_HAS_AUDIO);
    fields.append(NOTE_HAS_INK);
    fields.append(NOTE_HAS_PDF);
    fields.append(NOTE_HAS_ATTACHMENT);
    fields.append(NOTE_ATTRIBUTE_REMINDER_ORDER);

    fields.append(RESOURCE_INDEX_NEEDED);
    fields.append(RESOURCE_INKNOTE);
    fields.append(RESOURCE_CLIENT_WILL_INDEX);
    fields.append(RESOURCE_ACTIVE);
    fields.append(RESOURCE_ATTACHMENT);

    fields.append(NOTEBOOK_IS_DEFAULT);
    fields.append(NOTEBOOK_IS_LOCAL);
    fields.append(NOTEBOOK_ISDIRTY);
    fields.append(NOTEBOOK_IS_CLOSED);
    fields.append(NOTEBOOK_IS_DELETED);
    fields.append(NOTEBOOK_PUBLISHED);

    fields.append(SEARCH_ISDELETED);
    fields.append(SEARCH_ISDIRTY);

    fields.append(TAG_ISDIRTY);
    fields.append(TAG_ISDELETED);

    fields.append(LINKEDNOTEBOOK_IS_DIRTY);

    fields.append(SHAREDNOTEBOOK_ISDIRTY);
    fields.append(SHAREDNOTEBOOK_REQUIRE_LOGIN);
    fields.append(SHAREDNOTEBOOK_MODIFIABLE);
    fields.append(SHAREDNOTEBOOK_ALLOW_PREVIEW);

    NSqlQuery falseQuery(global.db);
    NSqlQuery trueQuery(global.db);
    if (toQt5) {
        falseQuery.prepare("Update Datastore set data=0 where data='false' and key=:key");
        trueQuery.prepare("Update Datastore set data=1 where data='true' and key=:key");
    } else {
        falseQuery.prepare("Update Datastore set data='false' where data=0 and key=:key");
        trueQuery.prepare("Update Datastore set data='true' where data=1 and key=:key");
    }

    for (int i=0; i<fields.size(); i++) {
        falseQuery.bindValue(":key", fields[i]);
        falseQuery.exec();
        trueQuery.bindValue(":key", fields[i]);
        trueQuery.exec();
    }
}

#ifndef USERTABLE_H
#define USERTABLE_H
#include <Q_LONG>
#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

using namespace evernote::edam;

class UserTable
{
private:
    void createTable();

public:
    UserTable();
    void updateUser(AuthenticationResult r);
    void updateSyncState(SyncState s);
    qlonglong getLastSyncDate();
    int getLastSyncNumber();
    void updateLastSyncDate(long date);
    void updateLastSyncNumber(int date);
};

#endif // USER_H

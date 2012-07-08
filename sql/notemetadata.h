#ifndef NOTEMETADATA_H
#define NOTEMETADATA_H

#include <QString>

class NoteMetaData;

class NoteMetaData
{
public:
    NoteMetaData();
    QString getGuid();
    void setGuid(QString g);

    int getColor();
    void setColor(int c);
    void setPinned(bool value);
    bool isPinned();
    void setDirty(bool v);
    bool isDirty();
    void copy(NoteMetaData m);

private:
    QString guid;
    int color;
    bool pinned;
    bool dirty;
};

#endif // NOTEMETADATA_H

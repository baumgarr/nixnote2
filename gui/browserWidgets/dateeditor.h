#ifndef DATEEDITOR_H
#define DATEEDITOR_H

#include <QWidget>
#include <QDateTimeEdit>
#include "gui/browserWidgets/authoreditor.h"
#include "gui/browserWidgets/datetimeeditor.h"

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

using namespace evernote::edam  ;
using namespace std;


class DateEditor : public QWidget
{
    Q_OBJECT
private:
    AuthorEditor authorEditor;
    DateTimeEditor subjectDate;
    DateTimeEditor createdDate;
    DateTimeEditor updatedDate;

public:
    explicit DateEditor(QWidget *parent = 0);
    void setNote(int lid, Note n);
    
signals:
    void valueChanged();

protected:
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);
    
public slots:

private slots:
    void emitChangedSignal();
    
};

#endif // DATEEDITOR_H

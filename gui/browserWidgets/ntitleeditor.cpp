#include "ntitleeditor.h"
#include "sql/notetable.h"

#include <QPalette>

NTitleEditor::NTitleEditor(QWidget *parent) :
    QLineEdit(parent)
{
    // Setup the note title editor
    QPalette pal;
    //pal.setColor(QPalette::Text, QColor(102,153,255));
    pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} QLineEdit:hover {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    this->setStyleSheet(inactiveColor);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(titleChanged(QString)));

}

void NTitleEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


// We now have focus.  Change the appearance
void NTitleEditor::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  emit(focussed(true));
}


// Focus is lost.  Set the colors back and check for text changed
void NTitleEditor::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  setStyleSheet(inactiveColor);
  titleChanged(text());
  emit(focussed(false));
}


// Set the title and values.  defaultT is the default title if the user
// blanks everything else out.  The default text logic should eventually be
// moved to the browser editor since it has the actual text of the note
// we need for the note.
void NTitleEditor::setTitle(int lid, QString title, QString defaultT) {
    priorTitle = title;
    currentLid = lid;
    initialTitle = title;
    defaultTitle = defaultT;
    blockSignals(true);
    setText(title);
    blockSignals(false);
}


// This function gets control whenever the text is edited.
void NTitleEditor::titleChanged(QString text) {
    this->blockSignals(true);

    // Check that we have some type of title.  If not we go with
    // the default
    if (text.trimmed() == "" && !hasFocus())
        setText(defaultTitle);
    else
        setText(text);
    this->blockSignals(false);

    // Now check that the text has actually changed.  We need to do this because
    // we can also get here when focus is lost.
    if (text.trimmed() != initialTitle.trimmed() || priorTitle.trimmed() != text.trimmed()) {
        NoteTable noteTable;
        noteTable.updateTitle(currentLid, text, true);
        emit(titleChanged());
    }

    priorTitle = text;
}


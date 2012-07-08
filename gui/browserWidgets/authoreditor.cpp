#include "authoreditor.h"
#include "global.h"
#include "sql/notetable.h"

AuthorEditor::AuthorEditor(QWidget *parent) :
    QLineEdit(parent)
{
    // Setup the note title editor
    QPalette pal;
    //pal.setColor(QPalette::Text, QColor(102,153,255));
    //pal.setColor(QPalette::Text, QColor(14,28,209));
    pal.setColor(backgroundRole(), QPalette::Base);
    setPalette(pal);

    QFont f = font();
    f.setPointSize(8);
    setFont(f);

    inactiveColor = "QLineEdit {background-color: transparent; border-radius: 0px;} ";
    activeColor = "QLineEdit {border: 1px solid #808080; background-color: white; border-radius: 4px;} ";
    this->setCursor(Qt::PointingHandCursor);
    this->setStyleSheet(inactiveColor);

    defaultText = QString(tr("Click to set Author..."));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textModified(QString)));
    this->textModified(defaultText);

    connect(this, SIGNAL(focussed(bool)), this, SLOT(gainedFocus(bool)));
    hide();
}

void AuthorEditor::setActiveColor() {
    setStyleSheet(activeColor);
}


void AuthorEditor::setAuthor(int lid, QString text) {
    currentLid = lid;
    blockSignals(true);
    if (text.trimmed() == "")
        setText(defaultText);
    else
        setText(text);
    blockSignals(false);
    initialText = text;
    priorText = text;
}


void AuthorEditor::focusInEvent(QFocusEvent *e)
{
  QLineEdit::focusInEvent(e);
  setStyleSheet(activeColor);
  this->setCursor(Qt::ArrowCursor);
  emit(focussed(true));
}

void AuthorEditor::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
  this->setCursor(Qt::PointingHandCursor);
  setStyleSheet(inactiveColor);
  textModified(text());
  emit(focussed(false));
}


void AuthorEditor::textModified(QString text) {
    this->blockSignals(true);

    NoteTable noteTable;
    noteTable.updateAuthor(currentLid, text, true);

    if (text.trimmed() == "" && !hasFocus())
        setText(defaultText);
    else
        setText(text);
    this->blockSignals(false);

    if (text.trimmed() != initialText.trimmed() || priorText.trimmed() != text.trimmed())
        emit(textUpdated());

    priorText = text;
}


void AuthorEditor::gainedFocus(bool focus) {
    if (focus && this->text() == defaultText)
        textModified("");
    if (!focus && this->text().trimmed() == "")
        textModified(defaultText);
}


QString AuthorEditor::getText() {
    if (this->text().trimmed() == defaultText)
        return "";
    else
        return text();
}

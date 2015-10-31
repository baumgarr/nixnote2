/****************************************************************************
**
** Copyright (C) 2006 J-P Nurmi. All rights reserved.
**
** The used XML syntax highlighting principles have been adapted from
** KXESyntaxHighlighter, which is part of KXML Editor 1.1.4,
** (C) 2003 by The KXMLEditor Team (http://kxmleditor.sourceforge.net).
**
** This file may be used under the terms of the GPL Version 2, June 1991.
** For details, see http://www.gnu.org/licenses/gpl.html
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QTextEdit>

class XmlHighlighter : public QSyntaxHighlighter
{
public:
    XmlHighlighter(QObject* parent);
    XmlHighlighter(QTextDocument* parent);
    XmlHighlighter(QTextEdit* parent);
    ~XmlHighlighter();

    enum HighlightType
    {
        SyntaxChar,
        ElementName,
        Comment,
        AttributeName,
        AttributeValue,
        Error,
        Other
    };

    void setHighlightColor(HighlightType type, QColor color, bool foreground = true);
    void setHighlightFormat(HighlightType type, QTextCharFormat format);

protected:
    void highlightBlock(const QString& rstrText);
    int  processDefaultText(int i, const QString& rstrText);

private:
    void init();

    QTextCharFormat fmtSyntaxChar;
    QTextCharFormat fmtElementName;
    QTextCharFormat fmtComment;
    QTextCharFormat fmtAttributeName;
    QTextCharFormat fmtAttributeValue;
    QTextCharFormat fmtError;
    QTextCharFormat fmtOther;

    enum ParsingState
    {
        NoState = 0,
        ExpectElementNameOrSlash,
        ExpectElementName,
        ExpectAttributeOrEndOfElement,
        ExpectEqual,
        ExpectAttributeValue
    };

    enum BlockState
    {
        NoBlock = -1,
        InComment,
        InElement
    };

    ParsingState state;
};

#endif // XMLHIGHLIGHTER_H

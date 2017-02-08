#include "finUiSyntaxHighlighter.h"

finUiSyntaxHighlighter::finUiSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    return;
}

void finUiSyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "\\b(var|return)\\b";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
     }    return;
}

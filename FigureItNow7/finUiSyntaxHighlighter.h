#ifndef FINUISYNTAXHIGHLIGHTER_H
#define FINUISYNTAXHIGHLIGHTER_H

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>


class finUiSyntaxHighlighter : public QSyntaxHighlighter
{
protected:
    QTextCharFormat _baseFormat;

public:
    finUiSyntaxHighlighter(QTextDocument *parent = 0);

protected:
    virtual void highlightBlock(const QString &text);
};

#endif // FINUISYNTAXHIGHLIGHTER_H

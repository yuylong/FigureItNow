#ifndef FINUISYNTAXHIGHLIGHTER_H
#define FINUISYNTAXHIGHLIGHTER_H

#include <QString>
#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>

#include "finErrorCode.h"


class finUiSyntaxHighlighter : public QSyntaxHighlighter
{
protected:
    enum Type {
        FIN_SH_TYPE_DUMMY,

        FIN_SH_TYPE_KEYWORD,
        FIN_SH_TYPE_KEYFUNC,
        FIN_SH_TYPE_OPERATOR,
        FIN_SH_TYPE_BRACKET,
        FIN_SH_TYPE_DECIMAL,

        FIN_SH_TYPE_STRING,
        FIN_SH_TYPE_LINE_COMMENT,
        FIN_SH_TYPE_BLOCK_COMMENT_ON,
        FIN_SH_TYPE_BLOCK_COMMENT_OFF,
    };

    enum State {
        FIN_SH_STAT_DUMMY = -1,
        FIN_SH_STAT_NORMAL = 0,
        FIN_SH_STAT_INCOMMENT,
    };

    QTextCharFormat _baseFormat;
    QMap<Type, QTextCharFormat> _formatList;
    QMap<Type, QRegExp> _regExpList;

public:
    finUiSyntaxHighlighter(QTextDocument *parent = 0);

    const QTextCharFormat &getBaseFormat() const;
    const QTextCharFormat getTypedFormat(Type type) const;
    finErrorCode setBaseFormat(const QTextCharFormat &format);

protected:
    void buildBaseFormat();
    void installFormatList();
    void installRegExpList();

    struct IgnoreItem {
        int _startIdx;
        int _length;
    };
    bool inIgnoreRange(int startidx, int len, const QList<IgnoreItem> &ignorerange) const;

    int searchTypedIndex(Type type, const QString &text, int startpos = 0);
    int findCommentAndString(Type *type, const QString &text, int startpos = 0);
    int handleString(const QString &text, int startpos, QList<IgnoreItem> *ignorerange);
    int handleLineComment(const QString &text, int startpos, QList<IgnoreItem> *ignorerange);
    int handleBlockComment(const QString &text, int startpos, QList<IgnoreItem> *ignorerange);
    finErrorCode handleCommentAndString(const QString &text, QList<IgnoreItem> *ignorerange);

    void handleNormalType(Type type, const QString &text, const QList<IgnoreItem> &ignorerange);
    virtual void highlightBlock(const QString &text);
};

#endif // FINUISYNTAXHIGHLIGHTER_H

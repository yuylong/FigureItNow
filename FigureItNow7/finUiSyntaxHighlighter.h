/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#ifndef FINUISYNTAXHIGHLIGHTER_H
#define FINUISYNTAXHIGHLIGHTER_H

#include <QtGlobal>
#include <QColor>
#include <QString>
#include <QMap>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>

#include "finErrorCode.h"


class finUiSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    enum Type {
        TP_DUMMY,
        TP_BASIC,

        TP_KEYWORD,
        TP_KEYFUNC,
        TP_OPERATOR,
        TP_BRACKET,
        TP_DECIMAL,

        TP_STRING,
        TP_LINE_COMMENT,
        TP_BLOCK_COMMENT_ON,
        TP_BLOCK_COMMENT_OFF,
    };

    struct TextFormatConfig {
        Type type;
        QString fontFamily;
        bool fontFamilyValid;
        qreal fontSize;
        bool fontSizeValid;
        int fontWeight;
        bool fontWeightValid;
        bool fontItalic;
        bool fontItalicValid;
        QColor foregroundColor;
        bool fgColorValid;
        QColor backgroundColor;
        bool bgColorValid;
    };

protected:
    enum State {
        ST_DUMMY = -1,
        ST_NORMAL = 0,
        ST_INCOMMENT,
    };

    static const QTextCharFormat &globalBaseFormat();

    QList<TextFormatConfig> _formatConfig;
    QMap<Type, QTextCharFormat> _formatList;
    QMap<Type, QRegExp> _regExpList;

public:
    finUiSyntaxHighlighter(QTextDocument *parent = 0);

    QTextCharFormat getBaseFormat() const;
    QTextCharFormat getTypedFormat(Type type) const;

protected:
    void loadDefaultConfig();

    static QTextCharFormat convertConfigToFormat(TextFormatConfig config, const QTextCharFormat &basefmt);
    int findBasicConfigIndex() const;
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

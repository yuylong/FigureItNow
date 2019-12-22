/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finUiSyntaxHighlighter.h"

#include <QTextDocument>


static finUiSyntaxHighlighter::TextFormatConfig _defTextFormatConfig[] = {
    {
        .type            = finUiSyntaxHighlighter::TP_BASIC,
        .fontFamily      = QString("Consolas"),
        .fontFamilyValid = true,
        .fontSize        = 10.0,
        .fontSizeValid   = true,
        .fontWeight      = QFont::Normal,
        .fontWeightValid = true,
        .fontItalic      = false,
        .fontItalicValid = true,
        .foregroundColor = Qt::black,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = true,
    }, {
        .type            = finUiSyntaxHighlighter::TP_KEYWORD,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = QFont::Bold,
        .fontWeightValid = true,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::blue,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_KEYFUNC,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::darkYellow,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_OPERATOR,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::darkBlue,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_BRACKET,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::darkMagenta,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_DECIMAL,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::red,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_STRING,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::darkGreen,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_LINE_COMMENT,
        .fontFamily      = QString(""),
        .fontFamilyValid = false,
        .fontSize        = 0.0,
        .fontSizeValid   = false,
        .fontWeight      = 0,
        .fontWeightValid = false,
        .fontItalic      = false,
        .fontItalicValid = false,
        .foregroundColor = Qt::darkGray,
        .fgColorValid    = true,
        .backgroundColor = Qt::transparent,
        .bgColorValid    = false,
    }, {
        .type            = finUiSyntaxHighlighter::TP_DUMMY,
    }
};

const QTextCharFormat &finUiSyntaxHighlighter::globalBaseFormat()
{
    static QTextCharFormat *basefmt = NULL;

    if ( basefmt != NULL )
        return *basefmt;

    basefmt = new QTextCharFormat();
    basefmt->setFontFamily(QString("Consolas"));
    basefmt->setFontPointSize(10.0);
    basefmt->setFontWeight(QFont::Normal);
    basefmt->setFontItalic(false);

    basefmt->setBackground(Qt::transparent);
    basefmt->setForeground(Qt::black);

    return *basefmt;
}

finUiSyntaxHighlighter::finUiSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    this->loadDefaultConfig();
    parent->setDefaultFont(globalBaseFormat().font());

    this->installFormatList();
    this->installRegExpList();
    return;
}

QTextCharFormat finUiSyntaxHighlighter::convertConfigToFormat(TextFormatConfig config, const QTextCharFormat &basefmt)
{
    QTextCharFormat retfmt = basefmt;

    if (config.fontFamilyValid)
        retfmt.setFontFamily(config.fontFamily);
    if (config.fontSizeValid)
        retfmt.setFontPointSize(config.fontSize);
    if (config.fontWeightValid)
        retfmt.setFontWeight(config.fontWeight);
    if (config.fontItalicValid)
        retfmt.setFontItalic(config.fontItalic);
    if (config.fgColorValid)
        retfmt.setForeground(config.foregroundColor);
    if (config.bgColorValid)
        retfmt.setBackground(config.backgroundColor);

    return retfmt;
}

void finUiSyntaxHighlighter::loadDefaultConfig()
{
    this->_formatConfig.clear();
    for (int i = 0; _defTextFormatConfig[i].type != finUiSyntaxHighlighter::TP_DUMMY; i++) {
        this->_formatConfig.append(_defTextFormatConfig[i]);
    }
}

int finUiSyntaxHighlighter::findBasicConfigIndex() const
{
    for (int i = 0; i < this->_formatConfig.count(); i++) {
        if (this->_formatConfig[i].type == TP_BASIC)
            return i;
    }
    return -1;
}

void finUiSyntaxHighlighter::installFormatList()
{
    int basefmtcfgidx = this->findBasicConfigIndex();
    QTextCharFormat basefmt;
    if (basefmtcfgidx >= 0)
        basefmt = convertConfigToFormat(this->_formatConfig[basefmtcfgidx], globalBaseFormat());
    else
        basefmt = globalBaseFormat();

    this->_formatList.clear();
    this->_formatList.insert(TP_BASIC, basefmt);

    TextFormatConfig curcfg;
    QTextCharFormat curfmt;
    foreach (curcfg, this->_formatConfig) {
        if (curcfg.type == TP_BASIC)
            continue;
        if (this->_formatList.contains(curcfg.type))
            continue;

        curfmt = convertConfigToFormat(curcfg, basefmt);
        this->_formatList.insert(curcfg.type, curfmt);
        if (curcfg.type == TP_LINE_COMMENT) {
            this->_formatList.insert(TP_BLOCK_COMMENT_ON, curfmt);
            this->_formatList.insert(TP_BLOCK_COMMENT_OFF, curfmt);
        }
    }
}

void finUiSyntaxHighlighter::installRegExpList()
{
    QRegExp regexp;

    // Keyword
    regexp = QRegExp(QString("\\b(if|elif|else|for|while|continue|break|return|exit|goto|var)\\b"));
    this->_regExpList.insert(TP_KEYWORD, regexp);

    // Key functions
    regexp = QRegExp(QString("\\b(dot|line|polyline|rect|polygon|circle|ellipse|axis|clear_fig|"
                                 "draw_(pinned_)?(text|image)|(read|write)_(fig|graph)_config|"
                                 "plot_(function|polar|parametric|equation)|named_color|"
                                 "sin|cos|tan|tg|cot|ctg|ln|log|PI|E)\\b"));
    this->_regExpList.insert(TP_KEYFUNC, regexp);

    // Operators
    regexp = QRegExp(QString("(\\+|\\-|\\-|\\*|\\/|\\^|\\=|\\!|\\&|\\||\\<|\\>|\\$)"));
    this->_regExpList.insert(TP_OPERATOR, regexp);

    // Brackets
    regexp = QRegExp(QString("(\\(|\\)|\\[|\\]|\\{|\\})"));
    this->_regExpList.insert(TP_BRACKET, regexp);

    // Decimal
    regexp = QRegExp(QString("\\b[0-9]+(\\.[0-9]+)?([e|E][\\-\\+]?[0-9]+)?\\b"));
    this->_regExpList.insert(TP_DECIMAL, regexp);

    // String
    regexp = QRegExp(QString("\\\""));
    this->_regExpList.insert(TP_STRING, regexp);

    // Comment
    regexp = QRegExp(QString("\\/\\/"));
    this->_regExpList.insert(TP_LINE_COMMENT, regexp);
    regexp = QRegExp(QString("\\/\\*"));
    this->_regExpList.insert(TP_BLOCK_COMMENT_ON, regexp);
    regexp = QRegExp(QString("\\*\\/"));
    this->_regExpList.insert(TP_BLOCK_COMMENT_OFF, regexp);
}

QTextCharFormat finUiSyntaxHighlighter::getBaseFormat() const
{
    return this->_formatList.value(TP_BASIC, globalBaseFormat());
}

QTextCharFormat finUiSyntaxHighlighter::getTypedFormat(Type type) const
{
    return this->_formatList.value(type, this->getBaseFormat());
}

bool finUiSyntaxHighlighter::inIgnoreRange(int startidx, int len,
                                           const QList<IgnoreItem> &ignorerange) const
{
    if ( ignorerange.empty() )
        return false;

    int endidx = startidx + len;
    for ( int i = 0; i < ignorerange.count(); i++ ) {
        const IgnoreItem &curitem = ignorerange.at(i);

        if ( startidx < curitem._startIdx + curitem._length &&
             curitem._startIdx < endidx )
            return true;
    }
    return false;
}

int finUiSyntaxHighlighter::searchTypedIndex(Type type, const QString &text, int startpos)
{
    if ( !this->_regExpList.contains(type) )
        return -1;

    QRegExp expression = this->_regExpList.value(type);
    return  text.indexOf(expression, startpos);
}

int finUiSyntaxHighlighter::findCommentAndString(Type *type, const QString &text, int startpos)
{
    if ( type == nullptr )
        return -1;
    int minidx = -1, curidx;

    curidx = this->searchTypedIndex(TP_STRING, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = TP_STRING;
    }

    curidx = this->searchTypedIndex(TP_LINE_COMMENT, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = TP_LINE_COMMENT;
    }

    curidx = this->searchTypedIndex(TP_BLOCK_COMMENT_ON, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = TP_BLOCK_COMMENT_ON;
    }
    return minidx;
}

int finUiSyntaxHighlighter::handleString(const QString &text, int startpos, QList<IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;
    if ( !this->_regExpList.contains(TP_STRING) )
        return startpos + 1;

    QRegExp expression = this->_regExpList.value(TP_STRING);
    int lastidx = text.indexOf(expression, startpos + 1);
    int length;
    if ( lastidx < 0 ) {
        length = text.length() - startpos;
    } else {
        lastidx += expression.matchedLength();
        length = lastidx - startpos;
    }

    QTextCharFormat format = this->getTypedFormat(TP_STRING);
    setFormat(startpos, length, format);
    this->setCurrentBlockState(ST_NORMAL);

    IgnoreItem newitem;
    newitem._startIdx = startpos;
    newitem._length = length;
    ignorerange->append(newitem);
    return lastidx;
}

int finUiSyntaxHighlighter::handleLineComment(const QString &text, int startpos, QList<IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;

    int length = text.length() - startpos;
    QTextCharFormat format = this->getTypedFormat(TP_LINE_COMMENT);
    setFormat(startpos, length, format);
    this->setCurrentBlockState(ST_NORMAL);

    finUiSyntaxHighlighter::IgnoreItem newitem;
    newitem._startIdx = startpos;
    newitem._length = length;
    ignorerange->append(newitem);
    return -1;
}

int finUiSyntaxHighlighter::handleBlockComment(const QString &text, int startpos, QList<IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;
    if ( !this->_regExpList.contains(TP_BLOCK_COMMENT_OFF) )
        return startpos + 2;

    QRegExp expression = this->_regExpList.value(TP_BLOCK_COMMENT_OFF);
    finUiSyntaxHighlighter::IgnoreItem newitem;
    newitem._startIdx = startpos;

    int fromidx = startpos;
    if ( this->currentBlockState() != ST_INCOMMENT )
        fromidx += 2;

    int lastidx = text.indexOf(expression, fromidx);
    int length;
    if ( lastidx < 0 ) {
        this->setCurrentBlockState(ST_INCOMMENT);
        length = text.length() - startpos;
    } else {
        this->setCurrentBlockState(ST_NORMAL);
        lastidx += expression.matchedLength();
        length = lastidx - startpos;
    }

    QTextCharFormat format = this->getTypedFormat(TP_BLOCK_COMMENT_ON);
    setFormat(startpos, length, format);

    newitem._length = length;
    ignorerange->append(newitem);
    return lastidx;
}

finErrorCode finUiSyntaxHighlighter::handleCommentAndString(const QString &text,
                                                            QList<IgnoreItem> *ignorerange)
{
    Type curtype = TP_DUMMY;
    int index = 0;
    if ( this->previousBlockState() == ST_INCOMMENT) {
        curtype = TP_BLOCK_COMMENT_ON;
        this->setCurrentBlockState(ST_INCOMMENT);
    } else {
        index = this->findCommentAndString(&curtype, text);
        this->setCurrentBlockState(ST_NORMAL);
    }

    while ( index >= 0 ) {
        switch ( curtype ) {
          case TP_STRING:
            index = this->handleString(text, index, ignorerange);
            break;

          case TP_LINE_COMMENT:
            index = this->handleLineComment(text, index, ignorerange);
            break;

          case TP_BLOCK_COMMENT_ON:
          case TP_BLOCK_COMMENT_OFF:
            index = this->handleBlockComment(text, index, ignorerange);
            break;

          default:
            index = -1;
            break;
        }
        if (index < 0 || index >= text.length())
            break;

        index = this->findCommentAndString(&curtype, text, index);
    }
    return finErrorKits::EC_SUCCESS;
}

void finUiSyntaxHighlighter::handleNormalType(finUiSyntaxHighlighter::Type type, const QString &text,
                                              const QList<finUiSyntaxHighlighter::IgnoreItem> &ignorerange)
{
    if ( !this->_regExpList.contains(type) )
        return;

    QTextCharFormat format = this->getTypedFormat(type);
    QRegExp expression = this->_regExpList.value(type);

    int index = text.indexOf(expression);
    while ( index >= 0 ) {
        int length = expression.matchedLength();
        if ( !this->inIgnoreRange(index, length, ignorerange) )
            this->setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}

void finUiSyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat baseformat = this->getBaseFormat();
    this->setFormat(0, text.length(), baseformat);

    QList<finUiSyntaxHighlighter::IgnoreItem> ignorerange;
    this->handleCommentAndString(text, &ignorerange);

    this->handleNormalType(TP_KEYWORD, text, ignorerange);
    this->handleNormalType(TP_KEYFUNC, text, ignorerange);
    this->handleNormalType(TP_OPERATOR, text, ignorerange);
    this->handleNormalType(TP_BRACKET, text, ignorerange);
    this->handleNormalType(TP_DECIMAL, text, ignorerange);
}

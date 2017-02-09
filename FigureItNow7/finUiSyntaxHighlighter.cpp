#include "finUiSyntaxHighlighter.h"


finUiSyntaxHighlighter::finUiSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    this->installFormatList();
    this->installRegExpList();
    return;
}

void finUiSyntaxHighlighter::installFormatList()
{
    QTextCharFormat curfmt;

    // Keyword
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::blue);
    curfmt.setFontWeight(QFont::Bold);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYWORD, curfmt);

    // Key Functions
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::darkYellow);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYFUNC, curfmt);

    // Operators
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::darkBlue);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_OPERATOR, curfmt);

    // Brackets
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::darkMagenta);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BRACKET, curfmt);

    // Decimal
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::red);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_DECIMAL, curfmt);

    // String
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::darkGreen);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING, curfmt);

    // Comment
    curfmt = this->_baseFormat;
    curfmt.setForeground(Qt::darkGray);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT, curfmt);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON, curfmt);
    this->_formatList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_OFF, curfmt);
}

void finUiSyntaxHighlighter::installRegExpList()
{
    QRegExp regexp;

    // Keyword
    regexp = QRegExp(QString("\\b(if|elif|else|for|while|continue|break|return|exit|goto|var)\\b"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYWORD, regexp);

    // Key functions
    regexp = QRegExp(QString("\\b(dot|line|polyline|rect|polygon|circle|ellipse|axis|clear_fig|"
                                 "draw_(pinned_)?(text|image)|(read|write)_(fig|graph)_config|"
                                 "plot_(function|polar|parametric|equation)|named_color|"
                                 "sin|cos|tan|tg|cot|ctg|ln|log|PI|E)\\b"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYFUNC, regexp);

    // Operators
    regexp = QRegExp(QString("(\\+|\\-|\\-|\\*|\\/|\\^|\\=|\\!|\\&|\\||\\<|\\>|\\$)"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_OPERATOR, regexp);

    // Brackets
    regexp = QRegExp(QString("(\\(|\\)|\\[|\\]|\\{|\\})"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BRACKET, regexp);

    // Decimal
    regexp = QRegExp(QString("\\b[0-9]+(\\.[0-9]+)?([e|E][\\-\\+]?[0-9]+)?\\b"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_DECIMAL, regexp);

    // String
    regexp = QRegExp(QString("\\\""));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING, regexp);

    // Comment
    regexp = QRegExp(QString("\\/\\/"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT, regexp);
    regexp = QRegExp(QString("\\/\\*"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON, regexp);
    regexp = QRegExp(QString("\\*\\/"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_OFF, regexp);
}

const QTextCharFormat &finUiSyntaxHighlighter::getBaseFormat() const
{
    return this->_baseFormat;
}

const QTextCharFormat finUiSyntaxHighlighter::getTypedFormat(finUiSyntaxHighlighter::Type type) const
{
    return this->_formatList.value(type, this->_baseFormat);
}

finErrorCode finUiSyntaxHighlighter::setBaseFormat(const QTextCharFormat &format)
{
    this->_baseFormat = format;
    this->installFormatList();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finUiSyntaxHighlighter::inIgnoreRange(int startidx, int len,
                                           const QList<finUiSyntaxHighlighter::IgnoreItem> &ignorerange) const
{
    if ( ignorerange.empty() )
        return false;

    int endidx = startidx + len;
    for ( int i = 0; i < ignorerange.count(); i++ ) {
        const finUiSyntaxHighlighter::IgnoreItem &curitem = ignorerange.at(i);

        if ( startidx < curitem._startIdx + curitem._length &&
             curitem._startIdx < endidx )
            return true;
    }
    return false;
}

int finUiSyntaxHighlighter::searchTypedIndex(finUiSyntaxHighlighter::Type type, const QString &text, int startpos)
{
    if ( !this->_regExpList.contains(type) )
        return -1;

    QRegExp expression = this->_regExpList.value(type);
    return  text.indexOf(expression, startpos);
}

int finUiSyntaxHighlighter::findCommentAndString(finUiSyntaxHighlighter::Type *type,
                                                 const QString &text, int startpos)
{
    if ( type == NULL )
        return -1;
    int minidx = -1, curidx;

    curidx = this->searchTypedIndex(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = finUiSyntaxHighlighter::FIN_SH_TYPE_STRING;
    }

    curidx = this->searchTypedIndex(finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT;
    }

    curidx = this->searchTypedIndex(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON, text, startpos);
    if ( curidx >= 0 && (minidx < 0 || curidx < minidx) ) {
        minidx = curidx;
        *type = finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON;
    }
    return minidx;
}

int finUiSyntaxHighlighter::handleString(const QString &text, int startpos,
                                         QList<finUiSyntaxHighlighter::IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;
    if ( !this->_regExpList.contains(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING) )
        return startpos + 1;

    QRegExp expression = this->_regExpList.value(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING);
    int lastidx = text.indexOf(expression, startpos + 1);
    int length;
    if ( lastidx < 0 ) {
        length = text.length() - startpos;
    } else {
        lastidx += expression.matchedLength();
        length = lastidx - startpos;
    }

    QTextCharFormat format = this->_formatList.value(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING,
                                                     this->_baseFormat);
    setFormat(startpos, length, format);
    this->setCurrentBlockState(FIN_SH_STAT_NORMAL);

    finUiSyntaxHighlighter::IgnoreItem newitem;
    newitem._startIdx = startpos;
    newitem._length = length;
    ignorerange->append(newitem);
    return lastidx;
}

int finUiSyntaxHighlighter::handleLineComment(const QString &text, int startpos,
                                              QList<finUiSyntaxHighlighter::IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;

    int length = text.length() - startpos;
    QTextCharFormat format = this->_formatList.value(finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT,
                                                     this->_baseFormat);
    setFormat(startpos, length, format);
    this->setCurrentBlockState(FIN_SH_STAT_NORMAL);

    finUiSyntaxHighlighter::IgnoreItem newitem;
    newitem._startIdx = startpos;
    newitem._length = length;
    ignorerange->append(newitem);
    return -1;
}

int finUiSyntaxHighlighter::handleBlockComment(const QString &text, int startpos,
                                               QList<finUiSyntaxHighlighter::IgnoreItem> *ignorerange)
{
    if ( startpos < 0 || startpos >= text.length() )
        return -1;
    if ( !this->_regExpList.contains(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_OFF) )
        return startpos + 2;

    QRegExp expression = this->_regExpList.value(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_OFF);
    finUiSyntaxHighlighter::IgnoreItem newitem;
    newitem._startIdx = startpos;

    int fromidx = startpos;
    if ( this->currentBlockState() != FIN_SH_STAT_INCOMMENT )
        fromidx += 2;

    int lastidx = text.indexOf(expression, fromidx);
    int length;
    if ( lastidx < 0 ) {
        this->setCurrentBlockState(FIN_SH_STAT_INCOMMENT);
        length = text.length() - startpos;
    } else {
        this->setCurrentBlockState(FIN_SH_STAT_NORMAL);
        lastidx += expression.matchedLength();
        length = lastidx - startpos;
    }
\
    QTextCharFormat format = this->_formatList.value(finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON,
                                                     this->_baseFormat);
    setFormat(startpos, length, format);

    newitem._length = length;
    ignorerange->append(newitem);
    return lastidx;
}

finErrorCode finUiSyntaxHighlighter::handleCommentAndString(const QString &text,
                                                            QList<finUiSyntaxHighlighter::IgnoreItem> *ignorerange)
{
    finUiSyntaxHighlighter::Type curtype = finUiSyntaxHighlighter::FIN_SH_TYPE_DUMMY;
    int index = 0;
    if ( this->previousBlockState() == FIN_SH_STAT_INCOMMENT) {
        curtype = finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON;
        this->setCurrentBlockState(FIN_SH_STAT_INCOMMENT);
    } else {
        index = this->findCommentAndString(&curtype, text);
        this->setCurrentBlockState(FIN_SH_STAT_NORMAL);
    }

    while ( index >= 0 ) {
        switch ( curtype ) {
          case finUiSyntaxHighlighter::FIN_SH_TYPE_STRING:
            index = this->handleString(text, index, ignorerange);
            break;

          case finUiSyntaxHighlighter::FIN_SH_TYPE_LINE_COMMENT:
            index = this->handleLineComment(text, index, ignorerange);
            break;

          case finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_ON:
          case finUiSyntaxHighlighter::FIN_SH_TYPE_BLOCK_COMMENT_OFF:
            index = this->handleBlockComment(text, index, ignorerange);
            break;

          default:
            index = -1;
            break;
        }
        if ( index < 0 || index >= text.length() )
            break;

        index = this->findCommentAndString(&curtype, text, index);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finUiSyntaxHighlighter::handleNormalType(finUiSyntaxHighlighter::Type type, const QString &text,
                                              const QList<finUiSyntaxHighlighter::IgnoreItem> &ignorerange)
{
    if ( !this->_regExpList.contains(type) )
        return;

    QTextCharFormat format = this->_formatList.value(type, this->_baseFormat);
    QRegExp expression = this->_regExpList.value(type);

    int index = text.indexOf(expression);
    while ( index >= 0 ) {
        int length = expression.matchedLength();
        if ( !this->inIgnoreRange(index, length, ignorerange) )
            setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}

void finUiSyntaxHighlighter::highlightBlock(const QString &text)
{
    QList<finUiSyntaxHighlighter::IgnoreItem> ignorerange;
    this->handleCommentAndString(text, &ignorerange);

    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYWORD, text, ignorerange);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYFUNC, text, ignorerange);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_OPERATOR, text, ignorerange);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_BRACKET, text, ignorerange);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_DECIMAL, text, ignorerange);
}

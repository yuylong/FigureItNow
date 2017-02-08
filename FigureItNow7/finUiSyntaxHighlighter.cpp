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
    curfmt.setForeground(Qt::darkCyan);
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
    regexp = QRegExp(QString("\\b(dot|line|axis)\\b"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYFUNC, regexp);

    // Operators
    regexp = QRegExp(QString("(\\+|\\+\\+|\\-|\\-\\-|\\*|\\/)"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_OPERATOR, regexp);

    // Brackets
    regexp = QRegExp(QString("(\\(|\\)|\\[|\\]|\\{|\\})"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_BRACKET, regexp);

    // Decimal
    regexp = QRegExp(QString("\\b[0-9]+(\\.[0-9]+)?(e(\\-|\\+)?[0-9]+)?\\b"));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_DECIMAL, regexp);

    // String
    regexp = QRegExp(QString("\".*\""));
    this->_regExpList.insert(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING, regexp);
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

void finUiSyntaxHighlighter::handleNormalType(finUiSyntaxHighlighter::Type type, const QString &text)
{
    if ( !this->_regExpList.contains(type) )
        return;

    QTextCharFormat format = this->_formatList.value(type, this->_baseFormat);
    QRegExp expression = this->_regExpList.value(type);

    int index = text.indexOf(expression);
    while ( index >= 0 ) {
        int length = expression.matchedLength();
        setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}

void finUiSyntaxHighlighter::highlightBlock(const QString &text)
{

    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYWORD, text);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_KEYFUNC, text);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_OPERATOR, text);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_BRACKET, text);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_DECIMAL, text);
    this->handleNormalType(finUiSyntaxHighlighter::FIN_SH_TYPE_STRING, text);
}

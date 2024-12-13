/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */

#include "finLexReader.h"

#include <QtLogging>
#include <QStringBuilder>


finLexReader::finLexReader()
    : _inputStr()
{
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    this->_lastNodeType = finLexNode::TP_DUMMY;
    this->_nextReadOrder = ORD_NUMBER_FIRST;
}

finLexReader::finLexReader(const QString &inputstr)
    : _inputStr(inputstr)
{
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    this->_lastNodeType = finLexNode::TP_DUMMY;
    this->_nextReadOrder = ORD_NUMBER_FIRST;
}

QString
finLexReader::getString() const
{
    return this->_inputStr;
}

unsigned long
finLexReader::getCurrentPosition() const
{
    return this->_posIdx;
}

qsizetype
finLexReader::getStringLength() const
{
    return this->_inputStr.length();
}

bool
finLexReader::isReachBottom() const
{
    return this->_posIdx >= this->getStringLength();
}

void
finLexReader::setString(const QString &instr)
{
    if ( !this->_inputStr.isEmpty() && this->_posIdx != 0 ) {
        throw finException(finErrorCode::EC_STATE_ERROR,
                           "Cannot change string for a lex reader in processing, resetPosition must be called first.");
    }

    this->_inputStr.clear();
    this->_inputStr = instr;
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
}

void
finLexReader::resetPosition()
{
    if ( this->_inputStr.isEmpty() ) {
        qWarning() << "Reset the position to a lex reader that contains empty input string." << Qt::endl;
    }
    if ( this->_posIdx == 0 ) {
        qWarning() << "Reset the position to an already reset lex reader." << Qt::endl;
    }

    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
}

finErrorCode
finLexReader::getNextLexNode(finLexNode *retnode)
{   
    finErrorCode ret;
    int typeordernum;
    const finLexNodeType *typeorder;

    // Check the current state of string inside the LexReader.
    if ( retnode == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( this->_inputStr.isEmpty() )
        return finErrorKits::EC_STATE_ERROR;

    // Move the position pointer to the next non-blank char of the string.
    ret = this->moveToNextNonblank();
    if ( ret == finErrorKits::EC_REACH_BOTTOM )
        return ret;

    ret = this->getLexTypeOrder(&typeorder, &typeordernum);
    if ( finErrorKits::isErrorResult(ret) )
        return ret;

    for ( int i = 0; i < typeordernum; i++ ) {
        ret = this->tryGetTypedNode(retnode, typeorder[i]);
        if ( ret == finErrorKits::EC_SUCCESS )
            return ret;
    }
    return ret;
}

unsigned long
finLexReader::scriptLength() const
{
    return static_cast<unsigned long>(this->_inputStr.length());
}

QChar
finLexReader::getScriptCharAt(unsigned long pos) const
{
    if ( pos >= this->scriptLength() )
        return QChar::Null;

    return this->_inputStr.at(static_cast<int>(pos));
}

QChar
finLexReader::getScriptChar() const
{
    return this->getScriptCharAt(this->_posIdx);
}

QString
finLexReader::getScriptSubAt(unsigned long pos, unsigned long len) const
{
    unsigned long strlength = this->scriptLength();
    if ( pos >= strlength )
        return QString("");

    unsigned long endpos = pos + len;
    if ( endpos >= strlength )
        endpos = strlength;

    return this->_inputStr.mid(static_cast<int>(pos), static_cast<int>(endpos - pos));
}

QString
finLexReader::getScriptSub(unsigned long len) const
{
    return getScriptSubAt(this->_posIdx, len);
}

finErrorCode
finLexReader::moveReadPos()
{
    this->_posIdx++;

    unsigned long strlength = this->scriptLength();
    if ( this->_posIdx >= strlength ) {
        this->_posIdx = strlength;
        return finErrorKits::EC_REACH_BOTTOM;
    }

    if ( this->getScriptChar() == QChar::LineFeed ) {
        this->_curRow++;
        this->_curCol = 0;
    } else {
        this->_curCol++;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finLexReader::moveReadPosWith(unsigned long detpos)
{
    finErrorCode errcode = finErrorKits::EC_SUCCESS;

    for ( unsigned long i = 0; i < detpos; i++ ) {
        errcode = this->moveReadPos();
        if ( finErrorKits::isErrorResult(errcode) || errcode == finErrorKits::EC_REACH_BOTTOM )
            break;
    }
    return errcode;
}

finErrorCode
finLexReader::moveToNextNonblank()
{
    finErrorCode errcode = finErrorKits::EC_SUCCESS;

    while ( this->getScriptChar().isSpace() ) {
        errcode = this->moveReadPos();
        if ( finErrorKits::isErrorResult(errcode) || errcode == finErrorKits::EC_REACH_BOTTOM )
            break;
    }

    return errcode;
}

void
finLexReader::buildLexNode(finLexNode *retnode, finLexNodeType type, unsigned long endpos)
{
    if ( endpos <= this->_posIdx ) {
        throw finException(finErrorKits::EC_INVALID_PARAM,
                           QString("LexReader build LexNode with wrong pos at %1.").arg(endpos));
    }

    unsigned long detpos = endpos - this->_posIdx;
    retnode->setType(type);
    retnode->setString(this->getScriptSub(detpos));
    retnode->setRow(this->_curRow);
    retnode->setColumn(this->_curCol);

    this->moveReadPosWith(detpos);
}

finErrorCode
finLexReader::getLexTypeOrder(const finLexNodeType **typeorder, int *typenum)
{
    return this->getLexTypeOrder(this->_nextReadOrder, typeorder, typenum);
}

finErrorCode finLexReader::getLexTypeOrder(finLexReader::finLexReaderOrder order,
                                           const finLexNodeType **typeorder, int *typenum)
{
    static const int _MaxTypeNum = 5;
    static const struct _infinLexTypeOrder {
        int _typeNum;
        finLexNodeType _typeOrder[_MaxTypeNum];
    } _typeOrderConst[] = {
        { 5, { finLexNode::TP_NOTE, finLexNode::TP_STRING,
               finLexNode::TP_VARIABLE, finLexNode::TP_DECIMAL,
               finLexNode::TP_OPERATOR }
        },
        { 5, { finLexNode::TP_NOTE, finLexNode::TP_STRING,
               finLexNode::TP_VARIABLE, finLexNode::TP_OPERATOR,
               finLexNode::TP_DECIMAL }
        }
    };

    if ( typeorder == nullptr && typenum == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    switch ( order ) {
      case ORD_NUMBER_FIRST:
        if ( typeorder != nullptr )
            *typeorder = &_typeOrderConst[0]._typeOrder[0];
        if ( typenum != nullptr )
            *typenum = _typeOrderConst[0]._typeNum;
        break;

      case ORD_OPERATOR_FIRST:
        if ( typeorder != nullptr )
            *typeorder = &_typeOrderConst[1]._typeOrder[0];
        if ( typenum != nullptr )
            *typenum = _typeOrderConst[1]._typeNum;
        break;

#if 0
      default:
        if ( typeorder != nullptr )
            *typeorder = _typeOrderConst[0]._typeOrder;
        if ( typenum != nullptr )
            *typenum = _typeOrderConst[0]._typeNum;
        return finErrorKits::EC_NORMAL_WARN;
        //break;
#endif
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexReader::tryGetTypedNode(finLexNode *retnode, finLexNodeType lextype)
{
    switch (lextype) {
      case finLexNode::TP_NOTE:
        return this->tryGetNote(retnode);
        //break;

      case finLexNode::TP_STRING:
        return this->tryGetString(retnode);
        //break;

      case finLexNode::TP_VARIABLE:
      case finLexNode::TP_KEYWORD:
        return this->tryGetVariable(retnode);
        //break;

      case finLexNode::TP_DECIMAL:
        return this->tryGetNumber(retnode);
        //break;

      case finLexNode::TP_OPERATOR:
        return this->tryGetOperator(retnode);
        //break;

      default:
        return finErrorKits::EC_INVALID_PARAM;
        //break;
    }
    //return finErrorKits::EC_UNKNOWN_ERROR;
}

finErrorCode finLexReader::tryGetNote(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = this->scriptLength();

    if ( trypos + 1 >= strlength || this->getScriptCharAt(trypos) != QChar('/') )
        return finErrorKits::EC_NOT_FOUND;

    trypos++;
    if ( this->getScriptCharAt(trypos) == QChar('/') ) {
        trypos++;
        while ( trypos < strlength ) {
            QChar curch = this->getScriptCharAt(trypos);
            QChar::Category curchcat = curch.category();

            if ( curchcat == QChar::Separator_Line || curchcat == QChar::Separator_Paragraph ||
                 curch == QChar::LineFeed || curch == QChar::CarriageReturn ||
                 curch == QChar::ParagraphSeparator || curch == QChar::LineSeparator )
                break;
            trypos++;
        }
    } else if ( this->getScriptCharAt(trypos) == QChar('*') ) {
        trypos++;
        bool hasendmark = false;
        QChar prechar = this->getScriptCharAt(trypos++);
        QChar curchar;

        while ( trypos < strlength ) {
            curchar = this->getScriptCharAt(trypos++);
            if ( prechar == QChar('*') && curchar == QChar('/') ) {
                hasendmark = true;
                break;
            }
            prechar = curchar;
        }
        if ( !hasendmark )
            return finErrorKits::EC_NOT_FOUND;
    } else {
        return finErrorKits::EC_NOT_FOUND;
    }

    this->buildLexNode(retnode, finLexNode::TP_NOTE, trypos);
    return finErrorKits::EC_SUCCESS;
}

bool finLexReader::isVariableStartChar(const QChar &ch)
{
    if ( ch.isLetter() || ch == QChar('_') )
        return true;
    else
        return false;
}

bool finLexReader::isVariableChar(const QChar &ch)
{
    if ( ch.isLetter() || ch.isDigit() || ch == QChar('_') )
        return true;
    else
        return false;
}

finErrorCode finLexReader::tryGetVariable(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = this->scriptLength();

    // Check the first char in the string, it must be a letter.
    if ( trypos >= strlength || !isVariableStartChar(this->getScriptCharAt(trypos)) )
        return finErrorKits::EC_NOT_FOUND;
    trypos++;

    // Check the rest char in the variable, it must be a letter or a digital.
    while ( trypos < strlength ) {
        if ( !isVariableChar(this->getScriptCharAt(trypos)) )
            break;

        trypos++;
    }

    this->buildLexNode(retnode, finLexNode::TP_VARIABLE, trypos);
    this->tryRecogKeyword(retnode);

    this->_lastNodeType = retnode->getType();
    this->_nextReadOrder = ORD_OPERATOR_FIRST;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexReader::tryRecogKeyword(finLexNode *retnode)
{
    static QString kwlist[] = {
        QString("if"), QString("elif"), QString("else"),
        QString("for"), QString("while"),
        QString("continue"), QString("break"),
        QString("return"), QString("exit"), QString("goto"),
        QString("var")
    };
    int kwlistcnt = sizeof (kwlist) / sizeof (QString);

    for ( int i = 0; i < kwlistcnt; i++ ) {
        if ( QString::compare(retnode->getString(), kwlist[i]) == 0 ) {
            retnode->setType(finLexNode::TP_KEYWORD);
            return finErrorKits::EC_SUCCESS;
        }
    }
    return finErrorKits::EC_NORMAL_WARN;
}

finErrorCode finLexReader::tryGetNumber(finLexNode *retnode)
{
    // Define a state machine to recognize the number.
    // All the number are treated as double or float, we do not differntiate the fixed or float point in
    // "Figure it Now".
    enum finLexInNumST {
        _IN_NUMST_INIT,
        _IN_NUMST_INTEG1,
        _IN_NUMST_INTEG,
        _IN_NUMST_FLOAT1,
        _IN_NUMST_FLOAT,
        _IN_NUMST_EX_INIT,
        _IN_NUMST_EX_INTEG1,
        _IN_NUMST_EX_INTEG
    } curstate = _IN_NUMST_INIT;
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = this->scriptLength();
    double basenum = 0.0, basestep = 0;
    int expnum = 0;
    bool basesign = true, expsign = true;

    while (trypos < strlength) {
        QChar curchar = this->getScriptCharAt(trypos);
        bool isfinish = false;

        switch (curstate) {
        case _IN_NUMST_INIT:
            if ( curchar == QChar('-') ) {
                basesign = false;
                curstate = _IN_NUMST_INTEG1;
            } else if ( curchar == QChar('+') ) {
                basesign = true;
                curstate = _IN_NUMST_INTEG1;
            } else if ( curchar.isDigit() ) {
                basenum = static_cast<double>(curchar.digitValue());
                curstate = _IN_NUMST_INTEG;
            } else {
                return finErrorKits::EC_NOT_FOUND;
            }
            break;

        case _IN_NUMST_INTEG1:
            if ( curchar.isDigit() ) {
                basenum = static_cast<double>(curchar.digitValue());
                curstate = _IN_NUMST_INTEG;
            } else {
                return finErrorKits::EC_NOT_FOUND;
            }
            break;

        case _IN_NUMST_INTEG:
            if (curchar == QChar('.'))
                curstate = _IN_NUMST_FLOAT1;
            else if ( curchar.toLower() == QChar('e') )
                curstate = _IN_NUMST_EX_INIT;
            else if ( curchar.isDigit() )
                basenum = basenum * 10.0 + static_cast<double>(curchar.digitValue());
            else if ( curchar.isLetter() )
                return finErrorKits::EC_NOT_FOUND;
            else
                isfinish = true;
            break;

        case _IN_NUMST_FLOAT1:
            if ( curchar.isDigit() ) {
                basestep = 0.1;
                basenum += basestep * static_cast<double>(curchar.digitValue());
                curstate = _IN_NUMST_FLOAT;
            } else {
                return finErrorKits::EC_NOT_FOUND;
            }
            break;

        case _IN_NUMST_FLOAT:
            if ( curchar.toLower() == QChar('e') )
                curstate = _IN_NUMST_EX_INIT;
            else if ( curchar.isDigit() )
                basenum += (basestep /= 10.0) * static_cast<double>(curchar.digitValue());
            else if ( curchar.isLetter() )
                return finErrorKits::EC_NOT_FOUND;
            else
                isfinish = true;

            break;

        case _IN_NUMST_EX_INIT:
            if ( curchar == QChar('-') ) {
                expsign = false;
                curstate = _IN_NUMST_EX_INTEG1;
            } else if ( curchar == QChar('+') ) {
                expsign = true;
                curstate = _IN_NUMST_EX_INTEG1;
            } else if ( curchar.isDigit() ) {
                expnum = curchar.digitValue();
                curstate = _IN_NUMST_EX_INTEG;
            } else {
                return finErrorKits::EC_NOT_FOUND;
            }
            break;

        case _IN_NUMST_EX_INTEG1:
            if ( curchar.isDigit() ) {
                expnum = curchar.digitValue();
                curstate = _IN_NUMST_EX_INTEG;
            } else {
                return finErrorKits::EC_NOT_FOUND;
            }
            break;

        case _IN_NUMST_EX_INTEG:
            if ( curchar.isDigit() )
                expnum = expnum * 10 + curchar.digitValue();
            else if ( curchar.isLetter() || curchar == QChar('.') )
                return finErrorKits::EC_NOT_FOUND;
            else
                isfinish = true;
            break;
        }

        if ( isfinish )
            break;
        trypos++;
    }

    if ( curstate == _IN_NUMST_INIT ||
         curstate == _IN_NUMST_INTEG1 || curstate == _IN_NUMST_FLOAT1 ||
         curstate == _IN_NUMST_EX_INIT || curstate == _IN_NUMST_EX_INTEG1 )
        return finErrorKits::EC_NOT_FOUND;

    this->buildLexNode(retnode, finLexNode::TP_DECIMAL, trypos);

    basenum = (basesign ? basenum : -basenum);
    basestep = (expsign ? 10.0 : 0.1);
    for ( /*expnum*/; expnum > 0; expnum-- ) {
        basenum *= basestep;
    }
    retnode->setFloatValue(basenum);

    this->_lastNodeType = finLexNode::TP_DECIMAL;
    this->_nextReadOrder = ORD_OPERATOR_FIRST;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexReader::tryGetString(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = this->scriptLength();

    if ( trypos >= strlength || this->getScriptCharAt(trypos) != QChar('\"'))
        return finErrorKits::EC_NOT_FOUND;

    QString retstr("");  // Create an empty (not null) string to store the result string.
    unsigned long grabstart = ++trypos;

    while ( trypos < strlength ) {
        QChar curch = this->getScriptCharAt(trypos);

        // A string constant ends with another double-quote or a newline.
        if ( curch == QChar('\"') ||
             curch.category() == QChar::Separator_Line || curch.category() == QChar::Separator_Paragraph ) {
            if ( trypos > grabstart )
                retstr += this->getScriptSubAt(grabstart, trypos - grabstart);
            trypos++;
            break;
        }

        if ( curch == QChar('\\') ) {
            if ( trypos > grabstart )
                retstr += this->getScriptSubAt(grabstart, trypos - grabstart);

            // Read one more char if the transformer char is found at current location.
            curch = this->_inputStr.at(static_cast<int>(++trypos));

            if ( curch == QChar('n') ) {
                retstr += QChar::LineFeed;
                retstr += QChar::CarriageReturn;
            } else if ( curch == QChar('t') ) {
                retstr += QChar::Tabulation;
            } else if ( curch == QChar('\\') || curch == QChar('\"') ) {
                retstr += curch;
            } else {
                return finErrorKits::EC_READ_ERROR;
            }

            grabstart = trypos + 1;
        }

        trypos++;
    }

    this->buildLexNode(retnode, finLexNode::TP_STRING, trypos);
    retnode->setStringValue(retstr);

    this->_lastNodeType = finLexNode::TP_STRING;
    this->_nextReadOrder = ORD_OPERATOR_FIRST;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexReader::tryGetOperator(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = this->scriptLength();

    if ( trypos >= strlength )
        return finErrorKits::EC_NOT_FOUND;

    QChar curchar = this->getScriptCharAt(trypos);
    finLexOperatorType optype = finLexNode::OP_DUMMY;
    trypos++;

    QChar nxtchar;
#define READ_NEXT_CHAR  do {                         \
        nxtchar = QChar('\0');                       \
        if ( trypos < strlength )                    \
            nxtchar = this->getScriptCharAt(trypos); \
    } while (0)

    if ( curchar == QChar('+') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('+') ) {
            optype = finLexNode::OP_ACCUMLT;
            this->_nextReadOrder = ORD_OPERATOR_FIRST;
            trypos++;
        } else {
            optype = finLexNode::OP_ADD;
            this->_nextReadOrder = ORD_NUMBER_FIRST;
        }
    } else if ( curchar == QChar('-') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('-') ) {
            optype = finLexNode::OP_DESCEND;
            this->_nextReadOrder = ORD_OPERATOR_FIRST;
            trypos++;
        } else {
            optype = finLexNode::OP_SUB;
            this->_nextReadOrder = ORD_NUMBER_FIRST;
        }
    } else if ( curchar == QChar('*') ) {
        optype = finLexNode::OP_MUL;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('/') ) {
        optype = finLexNode::OP_DIV;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('%') ) {
        optype = finLexNode::OP_MOD;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('^') ) {
        optype = finLexNode::OP_POWER;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('=') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::OP_EQUAL;
            trypos++;
        } else {
            optype = finLexNode::OP_LET;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('>') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::OP_GRT_EQ;
            trypos++;
        } else {
            optype = finLexNode::OP_GRT;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('<') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::OP_LES_EQ;
            trypos++;
        } else if ( nxtchar == QChar('>') ) {
            optype = finLexNode::OP_NONEQUAL;
            trypos++;
        } else {
            optype = finLexNode::OP_LES;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('!') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::OP_NONEQUAL;
            trypos++;
        } else {
            // Style 'num!' (Factorial) goes first, because the syntax reader can change it to LOGIC-NOT.
            optype = finLexNode::OP_FACTORI;
            //optype = finLexNode::OP_LOGIC_NOT;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('~') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('~') ) {
            optype = finLexNode::OP_LOGIC_NOT;
            trypos++;
        } else {
            optype = finLexNode::OP_BIT_NOT;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('&') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('&') ) {
            optype = finLexNode::OP_LOGIC_AND;
            trypos++;
        } else {
            optype = finLexNode::OP_BIT_AND;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('|') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('|') ) {
            optype = finLexNode::OP_LOGIC_OR;
            trypos++;
        } else {
            optype = finLexNode::OP_BIT_OR;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('$') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('$') ) {
            optype = finLexNode::OP_LOGIC_XOR;
            trypos++;
        } else {
            optype = finLexNode::OP_BIT_XOR;
        }
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('(') ) {
        optype = finLexNode::OP_L_RND_BRCKT;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(')') ) {
        optype = finLexNode::OP_R_RND_BRCKT;
        this->_nextReadOrder = ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar('[') ) {
        optype = finLexNode::OP_L_SQR_BRCKT;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(']') ) {
        optype = finLexNode::OP_R_SQR_BRCKT;
        this->_nextReadOrder = ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar('{') ) {
        optype = finLexNode::OP_L_FLW_BRCKT;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('}') ) {
        optype = finLexNode::OP_R_FLW_BRCKT;
        this->_nextReadOrder = ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar(';') ) {
        optype = finLexNode::OP_SPLIT;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(',') ) {
        optype = finLexNode::OP_COMMA;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(':') ) {
        optype = finLexNode::OP_COLON;
        this->_nextReadOrder = ORD_NUMBER_FIRST;
    } else {
        return finErrorKits::EC_NOT_FOUND;
    }
#undef READ_NEXT_CHAR

    if ( optype == finLexNode::OP_DUMMY )
        return finErrorKits::EC_NOT_FOUND;

    this->buildLexNode(retnode, finLexNode::TP_OPERATOR, trypos);
    retnode->setOperator(optype);

    this->_lastNodeType = finLexNode::TP_OPERATOR;
    return finErrorKits::EC_SUCCESS;
}

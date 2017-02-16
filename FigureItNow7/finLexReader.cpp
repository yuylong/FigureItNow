/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finLexReader.h"

#include <QStringBuilder>


finLexReader::finLexReader()
    : _inputStr()
{
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    this->_lastNodeType = finLexNode::FIN_LN_TYPE_DUMMY;
    this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
}

finLexReader::finLexReader(const QString &inputstr)
    : _inputStr(inputstr)
{
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    this->_lastNodeType = finLexNode::FIN_LN_TYPE_DUMMY;
    this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
}

QString finLexReader::getString() const
{
    return this->_inputStr;
}

unsigned long finLexReader::getCurrentPosition() const
{
    return this->_posIdx;
}

finErrorCode finLexReader::setString(const QString &instr)
{
    if ( !this->_inputStr.isEmpty() && this->_posIdx != 0 )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_inputStr.clear();
    this->_inputStr = instr;
    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}


finErrorCode finLexReader::resetPosition()
{
    if ( this->_inputStr.isEmpty() )
        return finErrorCodeKits::FIN_EC_UNREADY_WARN;
    if ( this->_posIdx == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_posIdx = 0;
    this->_curRow = 0;
    this->_curCol = 0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::getNextLexNode(finLexNode *retnode)
{   
    finErrorCode ret;
    int typeordernum;
    const finLexNodeType *typeorder;

    // Check the current state of string inside the LexReader.
    if ( retnode == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( this->_inputStr.isEmpty() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    // Move the position pointer to the next non-blank char of the string.
    ret = this->moveToNextNonblank();
    if ( ret == finErrorCodeKits::FIN_EC_REACH_BOTTOM )
        return ret;

    ret = this->getLexTypeOrder(&typeorder, &typeordernum);
    if ( finErrorCodeKits::isErrorResult(ret) )
        return ret;

    for ( int i = 0; i < typeordernum; i++ ) {
        ret = this->tryGetTypedNode(retnode, typeorder[i]);
        if ( ret == finErrorCodeKits::FIN_EC_SUCCESS )
            return ret;
    }
    return ret;
}

finErrorCode finLexReader::moveReadPos()
{
    this->_posIdx++;

    unsigned long strlength = (unsigned long)this->_inputStr.length();
    if ( this->_posIdx >= strlength ) {
        this->_posIdx = strlength;
        return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    }

    if ( this->_inputStr.at(this->_posIdx) == QChar::QChar::Separator_Line) {
        this->_curRow++;
        this->_curCol = 0;
    } else {
        this->_curCol++;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::moveToNextNonblank()
{
    unsigned long strlength = (unsigned long)this->_inputStr.length();

    while ( this->_posIdx < strlength && this->_inputStr.at(this->_posIdx).isSpace() )
        this->_posIdx++;

    if ( this->_posIdx >= strlength )
        return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::getLexTypeOrder(const finLexNodeType **typeorder, int *typenum)
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
        { 5, { finLexNode::FIN_LN_TYPE_NOTE, finLexNode::FIN_LN_TYPE_STRING,
               finLexNode::FIN_LN_TYPE_VARIABLE, finLexNode::FIN_LN_TYPE_DECIMAL,
               finLexNode::FIN_LN_TYPE_OPERATOR }
        },
        { 5, { finLexNode::FIN_LN_TYPE_NOTE, finLexNode::FIN_LN_TYPE_STRING,
               finLexNode::FIN_LN_TYPE_VARIABLE, finLexNode::FIN_LN_TYPE_OPERATOR,
               finLexNode::FIN_LN_TYPE_DECIMAL }
        }
    };

    if ( typeorder == NULL && typenum == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    switch ( order ) {
      case FIN_LR_ORD_NUMBER_FIRST:
        if ( typeorder != NULL )
            *typeorder = &_typeOrderConst[0]._typeOrder[0];
        if ( typenum != NULL )
            *typenum = _typeOrderConst[0]._typeNum;
        break;

      case FIN_LR_ORD_OPERATOR_FIRST:
        if ( typeorder != NULL )
            *typeorder = &_typeOrderConst[1]._typeOrder[0];
        if ( typenum != NULL )
            *typenum = _typeOrderConst[1]._typeNum;
        break;

      default:
        if ( typeorder != NULL )
            *typeorder = _typeOrderConst[0]._typeOrder;
        if ( typenum != NULL )
            *typenum = _typeOrderConst[0]._typeNum;
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
        break;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::tryGetTypedNode(finLexNode *retnode, finLexNodeType lextype)
{
    switch (lextype) {
      case finLexNode::FIN_LN_TYPE_NOTE:
        return this->tryGetNote(retnode);
        break;

      case finLexNode::FIN_LN_TYPE_STRING:
        return this->tryGetString(retnode);
        break;

      case finLexNode::FIN_LN_TYPE_VARIABLE:
      case finLexNode::FIN_LN_TYPE_KEYWORD:
        return this->tryGetVariable(retnode);
        break;

      case finLexNode::FIN_LN_TYPE_DECIMAL:
        return this->tryGetNumber(retnode);
        break;

      case finLexNode::FIN_LN_TYPE_OPERATOR:
        return this->tryGetOperator(retnode);
        break;

      default:
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        break;
    }
    //return finErrorCodeKits::FIN_EC_UNKNOWN_ERROR;
}

finErrorCode finLexReader::tryGetNote(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = (unsigned long)this->_inputStr.length();

    if ( trypos + 1 >= strlength || this->_inputStr.at(trypos) != QChar('/') )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    trypos++;
    if ( this->_inputStr.at(trypos) == QChar('/') ) {
        trypos++;
        while ( trypos < strlength ) {
            QChar curch = this->_inputStr.at(trypos);
            QChar::Category curchcat = curch.category();

            if ( curchcat == QChar::Separator_Line || curchcat == QChar::Separator_Paragraph ||
                 curch == QChar::LineFeed || curch == QChar::CarriageReturn ||
                 curch == QChar::ParagraphSeparator || curch == QChar::LineSeparator )
                break;
            trypos++;
        }
    } else if ( this->_inputStr.at(trypos) == QChar('*') ) {
        trypos++;
        bool hasendmark = false;
        QChar prechar = this->_inputStr.at(trypos++);
        QChar curchar;

        while ( trypos < strlength ) {
            curchar = this->_inputStr.at(trypos++);
            if ( prechar == QChar('*') && curchar == QChar('/') ) {
                hasendmark = true;
                break;
            }
            prechar = curchar;
        }
        if ( !hasendmark )
            return finErrorCodeKits::FIN_EC_NOT_FOUND;
    } else {
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }

    retnode->setType(finLexNode::FIN_LN_TYPE_NOTE);
    retnode->setString(this->_inputStr.mid(this->_posIdx, trypos - this->_posIdx));
    this->_posIdx = trypos;

    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    unsigned long strlength = (unsigned long)this->_inputStr.length();

    // Check the first char in the string, it must be a letter.
    if ( trypos >= strlength || !this->isVariableStartChar(this->_inputStr.at(trypos)) )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    trypos++;

    // Check the rest char in the variable, it must be a letter or a digital.
    while ( trypos < strlength ) {
        if ( !this->isVariableChar(this->_inputStr.at(trypos)) )
            break;

        trypos++;
    }

    retnode->setType(finLexNode::FIN_LN_TYPE_VARIABLE);
    retnode->setString(this->_inputStr.mid(this->_posIdx, trypos - this->_posIdx));
    this->_posIdx = trypos;

    this->tryRecogKeyword(retnode);

    this->_lastNodeType = retnode->getType();
    this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
            retnode->setType(finLexNode::FIN_LN_TYPE_KEYWORD);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }
    }
    return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finLexReader::tryGetNumber(finLexNode *retnode)
{
    // Define a state machine to recognize the number.
    // All the number are treated as double or float, we do not differntiate the fixed or float point in
    // "Figure it Now".
    enum finLexInNumST {
        FIN_LEX_IN_NUM_INIT,
        FIN_LEX_IN_NUM_INTEG1,
        FIN_LEX_IN_NUM_INTEG,
        FIN_LEX_IN_NUM_FLOAT1,
        FIN_LEX_IN_NUM_FLOAT,
        FIN_LEX_IN_NUM_EX_INIT,
        FIN_LEX_IN_NUM_EX_INTEG1,
        FIN_LEX_IN_NUM_EX_INTEG
    } curstate = FIN_LEX_IN_NUM_INIT;
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = (unsigned long)this->_inputStr.length();
    double basenum = 0.0, basestep = 0;
    int expnum = 0;
    bool basesign = true, expsign = true;

    while (trypos < strlength) {
        QChar curchar = this->_inputStr.at(trypos);
        bool isfinish = false;

        switch (curstate) {
        case FIN_LEX_IN_NUM_INIT:
            if ( curchar == QChar('-') ) {
                basesign = false;
                curstate = FIN_LEX_IN_NUM_INTEG1;
            } else if ( curchar == QChar('+') ) {
                basesign = true;
                curstate = FIN_LEX_IN_NUM_INTEG1;
            } else if ( curchar.isDigit() ) {
                basenum = (double)curchar.digitValue();
                curstate = FIN_LEX_IN_NUM_INTEG;
            } else {
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            }
            break;

        case FIN_LEX_IN_NUM_INTEG1:
            if ( curchar.isDigit() ) {
                basenum = (double)curchar.digitValue();
                curstate = FIN_LEX_IN_NUM_INTEG;
            } else {
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            }
            break;

        case FIN_LEX_IN_NUM_INTEG:
            if (curchar == QChar('.'))
                curstate = FIN_LEX_IN_NUM_FLOAT1;
            else if ( curchar.toLower() == QChar('e') )
                curstate = FIN_LEX_IN_NUM_EX_INIT;
            else if ( curchar.isDigit() )
                basenum = basenum * 10.0 + (double)curchar.digitValue();
            else if ( curchar.isLetter() )
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            else
                isfinish = true;
            break;

        case FIN_LEX_IN_NUM_FLOAT1:
            if ( curchar.isDigit() ) {
                basestep = 0.1;
                basenum += basestep * (double)curchar.digitValue();
                curstate = FIN_LEX_IN_NUM_FLOAT;
            } else {
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            }
            break;

        case FIN_LEX_IN_NUM_FLOAT:
            if ( curchar.toLower() == QChar('e') )
                curstate = FIN_LEX_IN_NUM_EX_INIT;
            else if ( curchar.isDigit() )
                basenum += (basestep /= 10.0) * (double)curchar.digitValue();
            else if ( curchar.isLetter() )
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            else
                isfinish = true;

            break;

        case FIN_LEX_IN_NUM_EX_INIT:
            if ( curchar == QChar('-') ) {
                expsign = false;
                curstate = FIN_LEX_IN_NUM_EX_INTEG1;
            } else if ( curchar == QChar('+') ) {
                expsign = true;
                curstate = FIN_LEX_IN_NUM_EX_INTEG1;
            } else if ( curchar.isDigit() ) {
                expnum = curchar.digitValue();
                curstate = FIN_LEX_IN_NUM_EX_INTEG;
            } else {
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            }
            break;

        case FIN_LEX_IN_NUM_EX_INTEG1:
            if ( curchar.isDigit() ) {
                expnum = curchar.digitValue();
                curstate = FIN_LEX_IN_NUM_EX_INTEG;
            } else {
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            }
            break;

        case FIN_LEX_IN_NUM_EX_INTEG:
            if ( curchar.isDigit() )
                expnum = expnum * 10 + curchar.digitValue();
            else if ( curchar.isLetter() || curchar == QChar('.') )
                return finErrorCodeKits::FIN_EC_NOT_FOUND;
            else
                isfinish = true;
            break;
        }

        if ( isfinish )
            break;
        trypos++;
    }

    if ( curstate == FIN_LEX_IN_NUM_INIT ||
         curstate == FIN_LEX_IN_NUM_INTEG1 || curstate == FIN_LEX_IN_NUM_FLOAT1 ||
         curstate == FIN_LEX_IN_NUM_EX_INIT || curstate == FIN_LEX_IN_NUM_EX_INTEG1 )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    retnode->setType(finLexNode::FIN_LN_TYPE_DECIMAL);
    retnode->setString(this->_inputStr.mid(this->_posIdx, trypos - this->_posIdx));
    this->_posIdx = trypos;

    basenum = (basesign ? basenum : -basenum);
    basestep = (expsign ? 10.0 : 0.1);
    for ( /*expnum*/; expnum > 0; expnum-- ) {
        basenum *= basestep;
    }
    retnode->setFloatValue(basenum);

    this->_lastNodeType = finLexNode::FIN_LN_TYPE_DECIMAL;
    this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::tryGetString(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = (unsigned long)this->_inputStr.length();

    if ( trypos >= strlength || this->_inputStr.at(trypos) != QChar('\"'))
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    QString retstr("");  // Create an empty (not null) string to store the result string.
    unsigned long grabstart = ++trypos;

    while ( trypos < strlength ) {
        QChar curch = this->_inputStr.at(trypos);

        // A string constant ends with another double-quote or a newline.
        if ( curch == QChar('\"') ||
             curch.category() == QChar::Separator_Line || curch.category() == QChar::Separator_Paragraph ) {
            if ( trypos > grabstart )
                retstr += this->_inputStr.mid(grabstart, trypos - grabstart);
            trypos++;
            break;
        }

        if ( curch == QChar('\\') ) {
            if ( trypos > grabstart )
                retstr += this->_inputStr.mid(grabstart, trypos - grabstart);

            // Read one more char if the transformer char is found at current location.
            curch = this->_inputStr.at(++trypos);

            if ( curch == QChar('n') ) {
                retstr += QChar::LineFeed;
                retstr += QChar::CarriageReturn;
            } else if ( curch == QChar('t') ) {
                retstr += QChar::Tabulation;
            } else if ( curch == QChar('\\') || curch == QChar('\"') ) {
                retstr += curch;
            } else {
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            }

            grabstart = trypos + 1;
        }

        trypos++;
    }

    retnode->setType(finLexNode::FIN_LN_TYPE_STRING);
    retnode->setString(this->_inputStr.mid(this->_posIdx, trypos - this->_posIdx));
    retnode->setStringValue(retstr);
    this->_posIdx = trypos;

    this->_lastNodeType = finLexNode::FIN_LN_TYPE_STRING;
    this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finLexReader::tryGetOperator(finLexNode *retnode)
{
    unsigned long trypos = this->_posIdx;
    unsigned long strlength = (unsigned long)this->_inputStr.length();

    if ( trypos >= strlength )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    QChar curchar = this->_inputStr.at(trypos);
    finLexOperatorType optype = finLexNode::FIN_LN_OPTYPE_DUMMY;
    trypos++;

    QChar nxtchar;
#define READ_NEXT_CHAR  do {                      \
        nxtchar = QChar('\0');                    \
        if ( trypos < strlength )                 \
            nxtchar = this->_inputStr.at(trypos); \
    } while (0)

    if ( curchar == QChar('+') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('+') ) {
            optype = finLexNode::FIN_LN_OPTYPE_ACCUMLT;
            this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_ADD;
            this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
        }
    } else if ( curchar == QChar('-') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('-') ) {
            optype = finLexNode::FIN_LN_OPTYPE_DESCEND;
            this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_SUB;
            this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
        }
    } else if ( curchar == QChar('*') ) {
        optype = finLexNode::FIN_LN_OPTYPE_MUL;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('/') ) {
        optype = finLexNode::FIN_LN_OPTYPE_DIV;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('%') ) {
        optype = finLexNode::FIN_LN_OPTYPE_MOD;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('^') ) {
        optype = finLexNode::FIN_LN_OPTYPE_POWER;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('=') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::FIN_LN_OPTYPE_EQUAL;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_LET;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('>') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::FIN_LN_OPTYPE_GRT_EQ;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_GRT;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('<') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::FIN_LN_OPTYPE_LES_EQ;
            trypos++;
        } else if ( nxtchar == QChar('>') ) {
            optype = finLexNode::FIN_LN_OPTYPE_NONEQUAL;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_LES;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('!') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('=') ) {
            optype = finLexNode::FIN_LN_OPTYPE_NONEQUAL;
            trypos++;
        } else {
            // Style 'num!' (Factorial) goes first, because the syntax reader can change it to LOGIC-NOT.
            optype = finLexNode::FIN_LN_OPTYPE_FACTORI;
            //optype = finLexNode::FIN_LN_OPTYPE_LOGIC_NOT;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('~') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('~') ) {
            optype = finLexNode::FIN_LN_OPTYPE_LOGIC_NOT;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_BIT_NOT;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('&') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('&') ) {
            optype = finLexNode::FIN_LN_OPTYPE_LOGIC_AND;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_BIT_AND;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('|') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('|') ) {
            optype = finLexNode::FIN_LN_OPTYPE_LOGIC_OR;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_BIT_OR;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('$') ) {
        READ_NEXT_CHAR;
        if ( nxtchar == QChar('$') ) {
            optype = finLexNode::FIN_LN_OPTYPE_LOGIC_XOR;
            trypos++;
        } else {
            optype = finLexNode::FIN_LN_OPTYPE_BIT_XOR;
        }
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('(') ) {
        optype = finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(')') ) {
        optype = finLexNode::FIN_LN_OPTYPE_R_RND_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar('[') ) {
        optype = finLexNode::FIN_LN_OPTYPE_L_SQR_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(']') ) {
        optype = finLexNode::FIN_LN_OPTYPE_R_SQR_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar('{') ) {
        optype = finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar('}') ) {
        optype = finLexNode::FIN_LN_OPTYPE_R_FLW_BRCKT;
        this->_nextReadOrder = FIN_LR_ORD_OPERATOR_FIRST;
    } else if ( curchar == QChar(';') ) {
        optype = finLexNode::FIN_LN_OPTYPE_SPLIT;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(',') ) {
        optype = finLexNode::FIN_LN_OPTYPE_COMMA;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else if ( curchar == QChar(':') ) {
        optype = finLexNode::FIN_LN_OPTYPE_COLON;
        this->_nextReadOrder = FIN_LR_ORD_NUMBER_FIRST;
    } else {
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }
#undef READ_NEXT_CHAR

    if ( optype == finLexNode::FIN_LN_OPTYPE_DUMMY )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    retnode->setType(finLexNode::FIN_LN_TYPE_OPERATOR);
    retnode->setString(this->_inputStr.mid(this->_posIdx, trypos - this->_posIdx));
    retnode->setOperator(optype);
    this->_posIdx = trypos;

    this->_lastNodeType = finLexNode::FIN_LN_TYPE_OPERATOR;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

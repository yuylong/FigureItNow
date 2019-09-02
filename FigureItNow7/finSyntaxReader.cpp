/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxReader.h"


finSyntaxReader::finSyntaxReader()
    : _lexReader(), _syntaxStack()
{
    this->_state = ST_DUMMY;
}

const finLexReader *finSyntaxReader::getLexReader() const
{
    return &this->_lexReader;
}

finLexReader *finSyntaxReader::getLexReader()
{
    return &this->_lexReader;
}

QString finSyntaxReader::getScriptCode() const
{
    return this->_lexReader.getString();
}

finErrorCode finSyntaxReader::setScriptCode(const QString &scriptcode)
{
    if ( this->isReading() )
        return finErrorKits::EC_STATE_ERROR;

    this->_lexReader.resetPosition();
    finErrorCode errcode = this->_lexReader.setString(scriptcode);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    if ( this->_state == ST_DUMMY)
        this->_state = ST_READY;
    return finErrorKits::EC_SUCCESS;
}

bool finSyntaxReader::isReading() const
{
    return this->_state == ST_READING || this->_state == ST_DONE;
}

finSyntaxReader::State finSyntaxReader::getState() const
{
    return this->_state;
}

finErrorCode finSyntaxReader::startRead()
{
    if ( this->_state != ST_READY )
        return finErrorKits::EC_STATE_ERROR;

    this->disposeAllRead();

    finErrorCode errcode = this->_lexReader.resetPosition();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    this->_state = ST_READING;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::stopRead()
{
    if ( this->_state == ST_DUMMY )
        return finErrorKits::EC_STATE_ERROR;
    else if ( !this->isReading() )
        return finErrorKits::EC_DUPLICATE_OP;

    this->_state = ST_READY;
    this->disposeAllRead();

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::readNextToken()
{
    if ( this->_state == ST_DONE )
        return finErrorKits::EC_REACH_BOTTOM;
    if ( this->_state != ST_READING )
        return finErrorKits::EC_STATE_ERROR;

    finLexNode lexnode;
    finErrorCode errcode = this->_lexReader.getNextLexNode(&lexnode);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    if ( errcode == finErrorKits::EC_REACH_BOTTOM ) {
        this->_state = ST_DONE;
        return errcode;
    }
    if ( lexnode.getType() == finLexNode::TP_DUMMY )
        return finErrorKits::EC_NORMAL_WARN;

    return this->processTypedNextToken(&lexnode, lexnode.getType());
}

finSyntaxTree *finSyntaxReader::getSyntaxTree()
{
    if ( !this->isReading() )
        return nullptr;

    finSyntaxTree *syntree = new finSyntaxTree();
    if ( syntree == nullptr )
        return nullptr;

    finErrorCode errcode = syntree->appendSyntaxNodeStack(&this->_syntaxStack);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete syntree;
        return nullptr;
    }

    syntree->setScriptCode(this->_lexReader.getString());
    syntree->appendSyntaxErrorList(&this->_errList);
    return syntree;
}


finErrorCode finSyntaxReader::disposeAllRead()
{
    while ( !this->_syntaxStack.empty() ) {
        finSyntaxNode *syntk = this->_syntaxStack.first();
        this->_syntaxStack.removeFirst();
        delete syntk;
    }

    this->_errList.clear();
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype)
{
    switch ( lextype ) {
      case finLexNode::TP_KEYWORD:
      case finLexNode::TP_VARIABLE:
      case finLexNode::TP_DECIMAL:
      case finLexNode::TP_STRING:
        return processInstanceToken(lexnode);

      case finLexNode::TP_OPERATOR:
        return processOperatorToken(lexnode);

      default:
        return processVirtualToken(lexnode);
    }
}

finErrorCode finSyntaxReader::processInstanceToken(finLexNode *lexnode)
{
    finSyntaxNodeType tokentype;
    switch ( lexnode->getType() ) {
      case finLexNode::TP_VARIABLE:
      case finLexNode::TP_DECIMAL:
      case finLexNode::TP_STRING:
        tokentype = finSyntaxNode::TP_EXPRESS;
        break;

      default:
        tokentype = finSyntaxNode::TP_SINGLE;
        break;
    }

    return this->pushSingleLexNode(lexnode, tokentype);
}

finErrorCode finSyntaxReader::processVirtualToken(finLexNode *lexnode)
{
    if ( lexnode == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    // Do nothing with the lex node, and release the memory merely.
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::processOperatorToken(finLexNode *lexnode)
{
    if ( isArithOperator(lexnode) )
        return this->processArithOperator(lexnode);
    else if ( isBracket(lexnode))
        return this->processBracket(lexnode);
    else
        return this->processSplitter(lexnode);
}

static struct _finSynLexOpTableItem {
    finLexOperatorType type;
    int priority;
    enum { FIN_MD_L2R, FIN_MD_R2L } meshDir;
    int bfParamCnt;
    int afParamCnt;
    finLexOperatorType zpTransOp;
} _finLexOpTable[] = {
    { finLexNode::OP_ADD,       0x1200, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_POSITIVE  },
    { finLexNode::OP_SUB,       0x1200, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_NEGATIVE  },
    { finLexNode::OP_POSITIVE,  0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_NEGATIVE,  0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_ACCUMLT,   0x2100, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 0, finLexNode::OP_ACCUMLT_2 },
    { finLexNode::OP_ACCUMLT_2, 0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_DESCEND,   0x2100, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 0, finLexNode::OP_DESCEND_2 },
    { finLexNode::OP_DESCEND_2, 0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_FACTORI,   0x2200, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 0, finLexNode::OP_LOGIC_NOT },
    { finLexNode::OP_MUL,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_DIV,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_MOD,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_POWER,     0x1280, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LET,       0x0100, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_EQUAL,     0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_GRT,       0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LES,       0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_NONEQUAL,  0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_GRT_EQ,    0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LES_EQ,    0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LOGIC_NOT, 0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LOGIC_AND, 0x0340, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LOGIC_OR,  0x0320, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_LOGIC_XOR, 0x0340, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_BIT_NOT,   0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_BIT_AND,   0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_BIT_OR,    0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     },
    { finLexNode::OP_BIT_XOR,   0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::OP_DUMMY     }
};

static struct _finSynLexOpTableItem *getLexOpTableItem(finLexOperatorType type)
{
    int tblitemcnt = static_cast<int>(sizeof (_finLexOpTable) / sizeof (struct _finSynLexOpTableItem));
    for ( int i = 0; i < tblitemcnt; i++ ) {
        if ( _finLexOpTable[i].type == type )
            return &_finLexOpTable[i];
    }
    return nullptr;
}

int finSyntaxReader::getOperatorBfParamCnt(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return -1;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return -1;

    return tblinfo->bfParamCnt;
}

int finSyntaxReader::getOperatorAfParamCnt(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return -1;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return -1;

    return tblinfo->afParamCnt;
}

finErrorCode finSyntaxReader::transformOpToNonBfParamOp(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return finErrorKits::EC_READ_ERROR;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return finErrorKits::EC_READ_ERROR;
    if ( tblinfo->zpTransOp == finLexNode::OP_DUMMY)
        return finErrorKits::EC_INVALID_PARAM;

    lexnode->setOperator(tblinfo->zpTransOp);
    return finErrorKits::EC_SUCCESS;
}

int finSyntaxReader::getOperatorPriority(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return 0x0;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return 0x0;

    return tblinfo->priority;
}

int finSyntaxReader::getOperatorMeshDir(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return static_cast<int>(_finSynLexOpTableItem::FIN_MD_L2R);

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return static_cast<int>(_finSynLexOpTableItem::FIN_MD_L2R);

    return static_cast<int>(tblinfo->meshDir);
}

bool finSyntaxReader::isArithOperator(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        return false;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        return false;
    else
        return true;
}

int finSyntaxReader::compareOperators(finLexNode *lexnode1, finLexNode *lexnode2)
{
    int cmpres = getOperatorPriority(lexnode1) - getOperatorPriority(lexnode2);
    if ( cmpres == 0 ) {
        if ( getOperatorMeshDir(lexnode2) == _finSynLexOpTableItem::FIN_MD_L2R )
            cmpres = 1;
        else
            cmpres = -1;
    }
    return cmpres;
}

finErrorCode finSyntaxReader::processArithOperator(finLexNode *lexnode)
{
    finErrorCode errcode;
    int bfparamcnt = getOperatorBfParamCnt(lexnode);
    int afparamcnt = getOperatorAfParamCnt(lexnode);
    if ( bfparamcnt < 0 || afparamcnt < 0 )
        return finErrorKits::EC_READ_ERROR;

    finSyntaxNode *prevtoken = nullptr;
    if ( this->_syntaxStack.count() > 0 )
        prevtoken = this->_syntaxStack.at(0);

    bool prev_is_express = false;
    if ( prevtoken != nullptr && prevtoken->getType() == finSyntaxNode::TP_EXPRESS )
        prev_is_express = true;

    if ( bfparamcnt > 0 && !prev_is_express ) {
        errcode = transformOpToNonBfParamOp(lexnode);
        if ( finErrorKits::isErrorResult(errcode) ) {
            return finErrorKits::EC_READ_ERROR;
        }
        bfparamcnt = 0/*getOperatorBfParamCnt(lexnode)*/;
        afparamcnt = getOperatorAfParamCnt(lexnode);
    }

    while ( true ) {
        finLexNode *prevop = nullptr;
        if ( this->_syntaxStack.count() > bfparamcnt ) {
            finSyntaxNode *preopsn = this->_syntaxStack.at(bfparamcnt);
            if ( preopsn->getType() == finSyntaxNode::TP_SINGLE ) {
                prevop = preopsn->getCommandLexNode();
                if ( prevop->getType() != finLexNode::TP_OPERATOR )
                    prevop = nullptr;
            }
        }

        if ( prevop == nullptr )
            break;
        if ( compareOperators (prevop, lexnode) < 0 )
            break;

        errcode = this->meshArithExpress();
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    finSyntaxNode *optoken = new finSyntaxNode();
    optoken->setType(finSyntaxNode::TP_SINGLE);
    optoken->setCommandLexNode(lexnode);
    this->_syntaxStack.prepend(optoken);

    if ( afparamcnt < 1 ) {
        errcode = this->meshArithExpress();
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    return finErrorKits::EC_SUCCESS;
}

bool finSyntaxReader::isLeftBracket(finLexNode *lexnode)
{
    switch ( lexnode->getOperator() ) {
      case finLexNode::OP_L_FLW_BRCKT:
      case finLexNode::OP_L_RND_BRCKT:
      case finLexNode::OP_L_SQR_BRCKT:
        return true;

      default:
        return false;
    }
}

bool finSyntaxReader::isRightBracket(finLexNode *lexnode)
{
    switch ( lexnode->getOperator() ) {
      case finLexNode::OP_R_FLW_BRCKT:
      case finLexNode::OP_R_RND_BRCKT:
      case finLexNode::OP_R_SQR_BRCKT:
        return true;

      default:
        return false;
    }
}

bool finSyntaxReader::isBracket(finLexNode *lexnode)
{
    return (isLeftBracket(lexnode) || isRightBracket(lexnode));
}

bool finSyntaxReader::isCorrespnBracket(finLexNode *lexnode1, finLexNode *lexnode2)
{
    finLexOperatorType ltype, rtype;
    if (isLeftBracket(lexnode1)) {
        ltype = lexnode1->getOperator();
        rtype = lexnode2->getOperator();
    } else {
        ltype = lexnode2->getOperator();
        rtype = lexnode1->getOperator();
    }

    if ( ltype == finLexNode::OP_L_FLW_BRCKT ) {
        if ( rtype == finLexNode::OP_R_FLW_BRCKT )
            return true;
        else
            return false;
    } else if ( ltype == finLexNode::OP_L_RND_BRCKT ) {
        if ( rtype == finLexNode::OP_R_RND_BRCKT )
            return true;
        else
            return false;
    } else if ( ltype == finLexNode::OP_L_SQR_BRCKT ) {
        if ( rtype == finLexNode::OP_R_SQR_BRCKT )
            return true;
        else
            return false;
    } else {
        return false;
    }
}

finErrorCode finSyntaxReader::processLeftBracket(finLexNode *lexnode)
{
    return this->pushSingleLexNode(lexnode, finSyntaxNode::TP_SINGLE);
}

finErrorCode finSyntaxReader::processRightBracket(finLexNode *lexnode)
{
    finErrorCode errcode;
    errcode = this->meshAllArithExpress();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->meshAllCommas();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finSyntaxNode *meshednode = new finSyntaxNode();
    if ( meshednode == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    meshednode->setType(finSyntaxNode::TP_EXPRESS);

    bool foundcosbrk = false;
    while ( this->_syntaxStack.count() > 0) {
        finSyntaxNode *curnode = this->_syntaxStack.first();
        finLexNode *curlex = curnode->getCommandLexNode();
        this->_syntaxStack.removeFirst();

        if ( curnode->getType() == finSyntaxNode::TP_SINGLE && isLeftBracket(curlex) ) {
            if ( isCorrespnBracket(curlex, lexnode) ) {
                meshednode->setCommandLexNode(curlex);
                delete curnode;
                foundcosbrk = true;
                break;
            } else {
                return finErrorKits::EC_READ_ERROR;
            }
        }

        meshednode->prependSubSyntaxNode(curnode);
    }
    if ( !foundcosbrk )
        return finErrorKits::EC_READ_ERROR;

    this->_syntaxStack.prepend(meshednode);

    switch ( lexnode->getOperator() ) {
      case finLexNode::OP_R_FLW_BRCKT:
        this->meshFlowerBracket();
        break;
      case finLexNode::OP_R_RND_BRCKT:
        this->meshRoundBracket();
        break;
      case finLexNode::OP_R_SQR_BRCKT:
        this->meshSquareBracket();
        break;
      default:
        break;
    }

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::processBracket(finLexNode *lexnode)
{
    if ( isLeftBracket(lexnode) )
        return this->processLeftBracket(lexnode);
    else
        return this->processRightBracket(lexnode);
}

bool finSyntaxReader::isSplitter(finLexNode *lexnode)
{
    switch ( lexnode->getOperator() ) {
      case finLexNode::OP_SPLIT:
      case finLexNode::OP_COMMA:
      case finLexNode::OP_COLON:
        return true;

      default:
        return false;
    }
}

finErrorCode finSyntaxReader::processSplitter(finLexNode *lexnode)
{
    finErrorCode errcode;
    errcode = this->meshAllArithExpress();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finSyntaxNode *presynnode = nullptr, *synnode = nullptr;

    switch ( lexnode->getOperator() ) {
      // Process S <- E; and S <- ; (empty statement)
      case finLexNode::OP_SPLIT:
        errcode = this->meshAllCommas();
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;

        synnode = new finSyntaxNode();
        if ( synnode == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;
        synnode->setType(finSyntaxNode::TP_STATEMENT);
        synnode->setCommandLexNode(lexnode);

        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);
        if ( presynnode != nullptr && presynnode->getType() == finSyntaxNode::TP_EXPRESS ) {
            synnode->appendSubSyntaxNode(presynnode);
            this->_syntaxStack.removeFirst();
        }
        this->_syntaxStack.prepend(synnode);

        errcode = this->meshStatementWithKeywords();
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        break;

      // Process L <- E:, where E <- var
      case finLexNode::OP_COLON:
        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);

        if ( presynnode == nullptr || presynnode->getType() != finSyntaxNode::TP_EXPRESS )
            return finErrorKits::EC_READ_ERROR;
        if ( presynnode->getCommandLexNode()->getType() != finLexNode::TP_VARIABLE )
            return finErrorKits::EC_READ_ERROR;

        presynnode->setType(finSyntaxNode::TP_LABEL);
        break;

      default:
        synnode = new finSyntaxNode();
        if ( synnode == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;

        synnode->setType(finSyntaxNode::TP_SINGLE);
        synnode->setCommandLexNode(lexnode);
        this->_syntaxStack.prepend(synnode);
        break;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype)
{
    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finErrorCode errcode;
    errcode = synnode->setType(tktype);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = synnode->setCommandLexNode(lexnode);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    this->_syntaxStack.prepend(synnode);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshArithExpress()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorKits::EC_READ_ERROR;

    finSyntaxNode *token0, *token1;
    token0 = this->_syntaxStack.at(0);
    token1 = this->_syntaxStack.at(1);

    finSyntaxNode *optoken = nullptr;
    finLexNode *oplex = nullptr;
    finSyntaxNode *oprand1 = nullptr, *oprand2 = nullptr;

    int afpcnt1, afpcnt2;
    if ( token0->getType() == finSyntaxNode::TP_SINGLE ) {
        afpcnt1 = 0;
        optoken = token0;
    } else if ( token0->getType() == finSyntaxNode::TP_EXPRESS ) {
        if ( token1->getType() != finSyntaxNode::TP_SINGLE )
            return finErrorKits::EC_READ_ERROR;
        afpcnt1 = 1;
        optoken = token1;
        oprand2 = token0;
    } else {
        return finErrorKits::EC_READ_ERROR;
    }

    oplex = optoken->getCommandLexNode();
    if ( oplex == nullptr )
        return finErrorKits::EC_READ_ERROR;

    afpcnt2 = getOperatorAfParamCnt(oplex);
    if ( afpcnt2 < 0 || afpcnt1 != afpcnt2 )
        return finErrorKits::EC_READ_ERROR;

    int bfpcnt = getOperatorBfParamCnt(oplex);
    if ( bfpcnt < 0 )
        return finErrorKits::EC_READ_ERROR;

    if ( bfpcnt > 0 ) {
        oprand1 = this->_syntaxStack.at(afpcnt1 + 1);
        if ( oprand1->getType() != finSyntaxNode::TP_EXPRESS )
            return finErrorKits::EC_READ_ERROR;
    }

    this->_syntaxStack.removeFirst();
    this->_syntaxStack.removeFirst();
    if ( bfpcnt > 0 && afpcnt1 > 0 )
        this->_syntaxStack.removeFirst();

    optoken->setType(finSyntaxNode::TP_EXPRESS);
    //optoken->setCommandLexNode(oplex);
    if ( bfpcnt > 0 && oprand1 != nullptr )
        optoken->appendSubSyntaxNode(oprand1);
    if ( afpcnt1 > 0 && oprand2 != nullptr )
        optoken->appendSubSyntaxNode(oprand2);
    this->_syntaxStack.prepend(optoken);

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshAllArithExpress()
{
    while ( this->meshArithExpress() == finErrorKits::EC_SUCCESS );
    return finErrorKits::EC_SUCCESS;
}

finSyntaxNode *finSyntaxReader::createDummyExpress()
{
    finLexNode dmyexplex;
    dmyexplex.setType(finLexNode::TP_DUMMY);

    finSyntaxNode *dmyexpsyn = new finSyntaxNode();
    if ( dmyexpsyn == nullptr )
        return nullptr;

    dmyexpsyn->setType(finSyntaxNode::TP_EXPRESS);
    dmyexpsyn->setCommandLexNode(&dmyexplex);
    return dmyexpsyn;
}

finErrorCode finSyntaxReader::meshAllCommas()
{
    bool hascomma =  false;
    for ( int i = 0; i < this->_syntaxStack.count(); i++ ) {
        finSyntaxNode *cursyn = this->_syntaxStack.at(i);
        finLexNode *curlex = cursyn->getCommandLexNode();
        if ( cursyn->getType() == finSyntaxNode::TP_EXPRESS )
            continue;

        if ( cursyn->getType() == finSyntaxNode::TP_SINGLE &&
             curlex->getType() == finLexNode::TP_OPERATOR &&
             curlex->getOperator() == finLexNode::OP_COMMA ) {
            hascomma = true;
            break;
        } else {
            hascomma = false;
            break;
        }
    }
    if ( !hascomma )
        return finErrorKits::EC_DUPLICATE_OP;

    // Process E <- E,E,...,E
    finLexNode lexnode;
    lexnode.setType(finLexNode::TP_OPERATOR);
    lexnode.setOperator(finLexNode::OP_COMMA);
    lexnode.setString(QString(","));

    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;
    synnode->setType(finSyntaxNode::TP_EXPRESS);
    synnode->setCommandLexNode(&lexnode);

    bool hasinst = false;
    while ( this->_syntaxStack.count() > 0 ) {
        finSyntaxNode *cursyn = this->_syntaxStack.first();
        finLexNode *curlex = cursyn->getCommandLexNode();

        if ( cursyn->getType() ==  finSyntaxNode::TP_EXPRESS ) {
            if ( hasinst )
                return finErrorKits::EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            synnode->prependSubSyntaxNode(cursyn);
            hasinst = true;
        } else if ( cursyn->getType() == finSyntaxNode::TP_SINGLE &&
                    curlex->getType() == finLexNode::TP_OPERATOR &&
                    curlex->getOperator() == finLexNode::OP_COMMA ) {
            if ( !hasinst ) {
                finSyntaxNode *dmyexpsyn = createDummyExpress();
                if ( dmyexpsyn == nullptr )
                    return finErrorKits::EC_OUT_OF_MEMORY;
                synnode->prependSubSyntaxNode(dmyexpsyn);
            }

            this->_syntaxStack.removeFirst();
            delete cursyn;
            hasinst = false;
        } else {
            break;
        }
    }

    if ( !hasinst ) {
        finSyntaxNode *dmyexpsyn = createDummyExpress();
        if ( dmyexpsyn == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;
        synnode->prependSubSyntaxNode(dmyexpsyn);
    }

    this->_syntaxStack.prepend(synnode);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshStatementWithKeywords()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorKits::EC_SUCCESS;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() != finSyntaxNode::TP_SINGLE ||
         prevlex->getType() != finLexNode::TP_KEYWORD )
        return finErrorKits::EC_SUCCESS;

    finSyntaxNode *stttk = this->_syntaxStack.first();
    finLexNode *sttlex = stttk->getCommandLexNode();

    if ( sttlex->getType() == finLexNode::TP_OPERATOR &&
         sttlex->getOperator() == finLexNode::OP_L_FLW_BRCKT ) {
        // Process B <- B'S where B' <- if(E)
        if ( QString::compare(prevlex->getString(), QString("if")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorKits::EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::TP_BRANCH);
            return finErrorKits::EC_SUCCESS;
        }

        // Process B <- BB'S where B' <- elif(E) | else and B does not contain
        // 'else'.
        bool handleelse = false;
        if ( QString::compare(prevlex->getString(), QString("elif")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorKits::EC_READ_ERROR;
            handleelse = true;
        } else if ( QString::compare(prevlex->getString(), QString("else")) == 0 ) {
            if ( prevtk->getSubListCount() > 0 )
                return finErrorKits::EC_READ_ERROR;
            handleelse = true;
        }

        if ( handleelse ) {
            finSyntaxNode *condtk = nullptr;
            if ( prevtk->getSubListCount() > 0 )
                condtk = prevtk->pickSubSyntaxNode(0);

            this->_syntaxStack.removeFirst();
            this->_syntaxStack.removeFirst();
            delete prevtk;
            if ( this->_syntaxStack.count() <= 0 )
                return finErrorKits::EC_READ_ERROR;

            prevtk = this->_syntaxStack.first();
            if ( prevtk->getType() != finSyntaxNode::TP_BRANCH )
                return finErrorKits::EC_READ_ERROR;
            if ( prevtk->getSubListCount() % 2 != 0 )
                return finErrorKits::EC_READ_ERROR;

            if ( condtk != nullptr )
                prevtk->appendSubSyntaxNode(condtk);
            prevtk->appendSubSyntaxNode(stttk);
            return finErrorKits::EC_SUCCESS;
        }

        // Process L <- L'S where L' <- for(S;S;E) | while(E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ||
             QString::compare(prevlex->getString(), QString("while")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorKits::EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::TP_LOOP);
            return finErrorKits::EC_SUCCESS;
        }
    }

    if ( sttlex->getType() == finLexNode::TP_OPERATOR &&
         sttlex->getOperator() == finLexNode::OP_SPLIT ) {
        // Process J <- J'E; where J' <- goto, and E <- var
        if ( QString::compare(prevlex->getString(), QString("goto")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 1 )
                return finErrorKits::EC_READ_ERROR;

            finSyntaxNode *sttsubtk = stttk->pickSubSyntaxNode(0);
            finLexNode *sttsublex = sttsubtk->getCommandLexNode();
            prevtk->appendSubSyntaxNode(sttsubtk);

            if ( sttsubtk->getType() != finSyntaxNode::TP_EXPRESS ||
                 sttsublex->getType() != finLexNode::TP_VARIABLE )
                return finErrorKits::EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_JUMP);
            return finErrorKits::EC_SUCCESS;
        }


        // Process J <- J'; where J' <- return(E) | exit(E) | return | exit
        // Process J <- J'E; where J' <- return | exit
        if ( QString::compare(prevlex->getString(), QString("return")) == 0 ||
             QString::compare(prevlex->getString(), QString("exit")) == 0 ) {
            int sttsubcnt = stttk->getSubListCount();
            if ( sttsubcnt > 0 )
                prevtk->appendSubSyntaxNode(stttk->pickSubSyntaxNode(0));

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_JUMP);
            return finErrorKits::EC_SUCCESS;
        }

        // Process J <- J'; where J' <- break | continue
        if ( QString::compare(prevlex->getString(), QString("break")) == 0 ||
             QString::compare(prevlex->getString(), QString("continue")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 0 )
                return finErrorKits::EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_JUMP);
            return finErrorKits::EC_SUCCESS;
        }

        // Process D <- var E;
        if ( QString::compare(prevlex->getString(), QString("var")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 1 )
                return finErrorKits::EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk->pickSubSyntaxNode(0));

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_DECLARE);
            return finErrorKits::EC_SUCCESS;
        }
    }

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::recogFlowerBracketType()
{
    finSyntaxNode *flwbtk = this->_syntaxStack.at(0);
    if ( flwbtk->getSubListCount() < 1 ) {
        flwbtk->setType(finSyntaxNode::TP_STATEMENT);
        return finErrorKits::EC_SUCCESS;
    }

    finSyntaxNode *curtk = flwbtk->getSubSyntaxNode(0);
    bool isstt = finSyntaxNode::isStatementLevelType(curtk->getType());
    if ( !isstt ) {
        if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
            return finErrorKits::EC_READ_ERROR;
    }

    for ( int i = 1; i < flwbtk->getSubListCount(); i++ ) {
        curtk = flwbtk->getSubSyntaxNode(i);
        if ( isstt ) {
            if ( !finSyntaxNode::isStatementLevelType(curtk->getType()) )
                return finErrorKits::EC_READ_ERROR;
        } else {
            if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
                return finErrorKits::EC_READ_ERROR;
        }
    }
    if ( isstt )
        flwbtk->setType(finSyntaxNode::TP_STATEMENT);
    else
        flwbtk->setType(finSyntaxNode::TP_EXPRESS);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshFlowerBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorKits::EC_SUCCESS;

    finErrorCode errcode = this->recogFlowerBracketType();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    // Process F <- E{S}, where E <- E(E)
    finSyntaxNode *curtk = this->_syntaxStack.at(0);
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS &&
         prevlex != nullptr &&
         prevlex->getType() == finLexNode::TP_OPERATOR &&
         prevlex->getOperator() == finLexNode::OP_FUNCTION ) {

        if ( curtk->getType() != finSyntaxNode::TP_STATEMENT )
            return finErrorKits::EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        prevtk->setType(finSyntaxNode::TP_FUNCTION);
        prevtk->appendSubSyntaxNode(curtk);

        return finErrorKits::EC_SUCCESS;
    }

    if ( curtk->getType() == finSyntaxNode::TP_STATEMENT ) {
        errcode = this->meshStatementWithKeywords();
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    return finErrorKits::EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxReader::meshRoundBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorKits::EC_SUCCESS;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);

    // Process E <- E(E)
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        int brcktk_subcnt = brcktk->getSubListCount();
        if ( brcktk_subcnt > 1 )
            return finErrorKits::EC_READ_ERROR;
        if ( brcktk_subcnt > 0 && brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
            return finErrorKits::EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::TP_OPERATOR);
        meshedlex.setOperator(finLexNode::OP_FUNCTION);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;
        meshedtk->setType(finSyntaxNode::TP_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return finErrorKits::EC_SUCCESS;
    }

    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() == finSyntaxNode::TP_SINGLE &&
         prevlex->getType() == finLexNode::TP_KEYWORD ) {

        // Process B' <- if(E) , B' <- elif(E) , and L' <- while(E)
        if ( QString::compare(prevlex->getString(), QString("if")) == 0 ||
             QString::compare(prevlex->getString(), QString("elif")) == 0 ||
             QString::compare(prevlex->getString(), QString("while")) == 0 ) {
            finSyntaxNode *brcktk = this->_syntaxStack.at(0);
            if ( brcktk->getSubListCount() != 1 )
                return finErrorKits::EC_READ_ERROR;
            if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
                return finErrorKits::EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return finErrorKits::EC_SUCCESS;
        }

        // Process L' <- for(S;S;E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ) {
            finSyntaxNode *brcktk = this->_syntaxStack.at(0);
            if ( brcktk->getSubListCount() != 3 )
                return finErrorKits::EC_READ_ERROR;

            finSyntaxNodeType sub0type = brcktk->getSubSyntaxNode(0)->getType(),
                              sub1type = brcktk->getSubSyntaxNode(1)->getType(),
                              sub2type = brcktk->getSubSyntaxNode(2)->getType();
            if ( !(finSyntaxNode::isStatementLevelType(sub0type) &&
                   sub0type != finSyntaxNode::TP_LABEL && sub0type != finSyntaxNode::TP_JUMP) )
                return finErrorKits::EC_READ_ERROR;
            if ( sub1type != finSyntaxNode::TP_STATEMENT )
                return finErrorKits::EC_READ_ERROR;
            if ( sub2type != finSyntaxNode::TP_EXPRESS )
                return finErrorKits::EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return finErrorKits::EC_SUCCESS;
        }
    }

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshSquareBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorKits::EC_SUCCESS;

    // Process E <- E[E]
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        if ( brcktk->getSubListCount() != 1 )
            return finErrorKits::EC_READ_ERROR;
        if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
            return finErrorKits::EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::TP_OPERATOR);
        meshedlex.setOperator(finLexNode::OP_ACCESS);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;
        meshedtk->setType(finSyntaxNode::TP_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return finErrorKits::EC_SUCCESS;
    }
    return finErrorKits::EC_SUCCESS;
}



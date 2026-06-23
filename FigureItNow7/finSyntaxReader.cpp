/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
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

QString finSyntaxReader::dumpObjInfo() const
{
    QString retstr;
    QTextStream ts(&retstr);

    ts << "SyntaxReader:State=" << this->_state
       << ",StackDepth=" << this->_syntaxStack.count()
       << ",Errors=" << this->_errList.count()
       << ",Lex={" << this->_lexReader.dumpObjInfo() << "}";
    return retstr;
}

void finSyntaxReader::setScriptCode(const QString &scriptcode)
{
    if ( this->isReading() )
        finThrowObj(finErrorKits::EC_STATE_ERROR, "Cannot change script while reading.");

    this->_lexReader.resetPosition();
    this->_lexReader.setString(scriptcode);

    if ( this->_state == ST_DUMMY)
        this->_state = ST_READY;
}

bool finSyntaxReader::isReading() const
{
    return this->_state == ST_READING || this->_state == ST_DONE;
}

finSyntaxReader::State finSyntaxReader::getState() const
{
    return this->_state;
}

void finSyntaxReader::startRead()
{
    if ( this->_state != ST_READY )
        finThrowObj(finErrorKits::EC_STATE_ERROR, "Syntax reader is not in the READY state.");

    this->disposeAllRead();
    this->_lexReader.resetPosition();

    this->_state = ST_READING;
}

void finSyntaxReader::stopRead()
{
    if ( this->_state == ST_DUMMY )
        finThrowObj(finErrorKits::EC_STATE_ERROR, "Syntax reader has not been set up.");
    if ( !this->isReading() )
        finThrowObj(finErrorKits::EC_DUPLICATE_OP, "Syntax reader is already stopped.");

    this->_state = ST_READY;
    this->disposeAllRead();
}

bool finSyntaxReader::readNextToken()
{
    if ( this->_state == ST_DONE )
        return false;
    if ( this->_state != ST_READING )
        finThrowObj(finErrorKits::EC_STATE_ERROR, "Syntax reader is not in the READING state.");

    finLexNode lexnode;
    try {
        if ( !this->_lexReader.getNextLexNode(&lexnode) ) {
            this->_state = ST_DONE;
            return false;
        }
    } catch (finException &e) {
        // Record the lex error and stop reading. The errList travels with the syntax tree.
        finSyntaxError synerr;
        synerr.setLevel(finSyntaxError::LV_ERROR);
        synerr.setStage(finSyntaxError::ST_COMPILE);
        synerr.setRow(0);
        synerr.setColumn(0);

        if ( const finLexReader *lexr = dynamic_cast<const finLexReader *>(e.getErrorObject()) ) {
            synerr.setRow(lexr->getRow());
            synerr.setColumn(lexr->getColumn());
        }

        synerr.setErrorString(e.getErrorDescription());
        this->_errList.append(synerr);
        this->_state = ST_DONE;
        return false;
    }

    if ( lexnode.getType() == finLexNode::TP_DUMMY )
        return true;

    this->processTypedNextToken(&lexnode, lexnode.getType());
    return true;
}

finSyntaxTree *finSyntaxReader::getSyntaxTree()
{
    if ( !this->isReading() )
        return nullptr;

    finSyntaxTree *syntree = new finSyntaxTree();
    if ( syntree == nullptr )
        return nullptr;

    syntree->appendSyntaxNodeStack(&this->_syntaxStack);
    syntree->setScriptCode(this->_lexReader.getString());
    syntree->appendSyntaxErrorList(&this->_errList);
    return syntree;
}


void finSyntaxReader::disposeAllRead()
{
    while ( !this->_syntaxStack.empty() ) {
        finSyntaxNode *syntk = this->_syntaxStack.first();
        this->_syntaxStack.removeFirst();
        delete syntk;
    }

    this->_errList.clear();
}

void finSyntaxReader::processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype)
{
    switch ( lextype ) {
      case finLexNode::TP_KEYWORD:
      case finLexNode::TP_VARIABLE:
      case finLexNode::TP_DECIMAL:
      case finLexNode::TP_STRING:
        this->processInstanceToken(lexnode);
        break;

      case finLexNode::TP_OPERATOR:
        this->processOperatorToken(lexnode);
        break;

      default:
        this->processVirtualToken(lexnode);
        break;
    }
}

void finSyntaxReader::processInstanceToken(finLexNode *lexnode)
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

    this->pushSingleLexNode(lexnode, tokentype);
}

void finSyntaxReader::processVirtualToken(finLexNode *lexnode)
{
    if ( lexnode == nullptr )
        finThrowObj(finErrorKits::EC_NULL_POINTER, "Process a null virtual token.");

    // Do nothing with the lex node, and release the memory merely.
}

void finSyntaxReader::processOperatorToken(finLexNode *lexnode)
{
    if ( isArithOperator(lexnode) )
        this->processArithOperator(lexnode);
    else if ( isBracket(lexnode))
        this->processBracket(lexnode);
    else
        this->processSplitter(lexnode);
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

void finSyntaxReader::transformOpToNonBfParamOp(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::TP_OPERATOR)
        finThrow(finErrorKits::EC_READ_ERROR, "Cannot transform non-operator to non-before-param form.");

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == nullptr )
        finThrow(finErrorKits::EC_READ_ERROR, "Unknown operator in syntax table.");
    if ( tblinfo->zpTransOp == finLexNode::OP_DUMMY)
        finThrow(finErrorKits::EC_INVALID_PARAM, "Operator has no non-before-param form.");

    lexnode->setOperator(tblinfo->zpTransOp);
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

void finSyntaxReader::processArithOperator(finLexNode *lexnode)
{
    int bfparamcnt = getOperatorBfParamCnt(lexnode);
    int afparamcnt = getOperatorAfParamCnt(lexnode);
    if ( bfparamcnt < 0 || afparamcnt < 0 )
        finThrowObj(finErrorKits::EC_READ_ERROR, "Unknown arithmetic operator in syntax table.");

    finSyntaxNode *prevtoken = nullptr;
    if ( this->_syntaxStack.count() > 0 )
        prevtoken = this->_syntaxStack.at(0);

    bool prev_is_express = false;
    if ( prevtoken != nullptr && prevtoken->getType() == finSyntaxNode::TP_EXPRESS )
        prev_is_express = true;

    if ( bfparamcnt > 0 && !prev_is_express ) {
        this->transformOpToNonBfParamOp(lexnode);
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

        if ( !this->meshArithExpress() )
            return;
    }

    finSyntaxNode *optoken = new finSyntaxNode();
    if ( optoken == nullptr )
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc arithmetic operator node failed.");
    optoken->setType(finSyntaxNode::TP_SINGLE);
    optoken->setCommandLexNode(lexnode);
    this->_syntaxStack.prepend(optoken);

    if ( afparamcnt < 1 )
        if ( !this->meshArithExpress() )
            return;
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

void finSyntaxReader::processLeftBracket(finLexNode *lexnode)
{
    this->pushSingleLexNode(lexnode, finSyntaxNode::TP_SINGLE);
}

void finSyntaxReader::processRightBracket(finLexNode *lexnode)
{
    this->meshAllArithExpress();
    this->meshAllCommas();

    finSyntaxNode *meshednode = new finSyntaxNode();
    if ( meshednode == nullptr )
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc right-bracket mesh node failed.");

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
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            QString("Mismatched bracket: %1 vs %2")
                                .arg(curlex->getString(), lexnode->getString()));
            }
        }

        meshednode->prependSubSyntaxNode(curnode);
    }
    if ( !foundcosbrk )
        finThrowObj(finErrorKits::EC_READ_ERROR,
                    QString("No matching left bracket for %1").arg(lexnode->getString()));

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
}

void finSyntaxReader::processBracket(finLexNode *lexnode)
{
    if ( isLeftBracket(lexnode) )
        this->processLeftBracket(lexnode);
    else
        this->processRightBracket(lexnode);
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

void
finSyntaxReader::processSplitter(finLexNode *lexnode)
{
    this->meshAllArithExpress();

    finSyntaxNode *presynnode = nullptr, *synnode = nullptr;

    switch ( lexnode->getOperator() ) {
      // Process S <- E; and S <- ; (empty statement)
      case finLexNode::OP_SPLIT:
        this->meshAllCommas();

        synnode = new finSyntaxNode();
        if ( synnode == nullptr )
            finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc split statement node failed.");
        synnode->setType(finSyntaxNode::TP_STATEMENT);
        synnode->setCommandLexNode(lexnode);

        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);
        if ( presynnode != nullptr && presynnode->getType() == finSyntaxNode::TP_EXPRESS ) {
            synnode->appendSubSyntaxNode(presynnode);
            this->_syntaxStack.removeFirst();
        }
        this->_syntaxStack.prepend(synnode);

        this->meshStatementWithKeywords();
        break;

      // Process L <- E:, where E <- var
      case finLexNode::OP_COLON:
        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);

        if ( presynnode == nullptr || presynnode->getType() != finSyntaxNode::TP_EXPRESS )
            finThrowObj(finErrorKits::EC_READ_ERROR, "Label ':' must follow an expression.");
        if ( presynnode->getCommandLexNode()->getType() != finLexNode::TP_VARIABLE )
            finThrowObj(finErrorKits::EC_READ_ERROR,
                        "Label ':' must follow a variable name.");

        presynnode->setType(finSyntaxNode::TP_LABEL);
        break;

      default:
        synnode = new finSyntaxNode();
        if ( synnode == nullptr )
            finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc splitter node failed.");

        synnode->setType(finSyntaxNode::TP_SINGLE);
        synnode->setCommandLexNode(lexnode);
        this->_syntaxStack.prepend(synnode);
        break;
    }
}

void finSyntaxReader::pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype)
{
    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == nullptr )
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc single-lex-node failed.");

    synnode->setType(tktype);
    synnode->setCommandLexNode(lexnode);

    this->_syntaxStack.prepend(synnode);
}

bool finSyntaxReader::meshArithExpress()
{
    if ( this->_syntaxStack.count() < 2 )
        return false;

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
            // token1 is a complete statement / function / program / etc., not an operator.
            // token0 is a new expression sitting on top of a previous statement. They are
            // not part of the same arithmetic expression; stop the drain here.
            return false;
        afpcnt1 = 1;
        optoken = token1;
        oprand2 = token0;
    } else {
        finThrowObj(finErrorKits::EC_READ_ERROR, "Malformed token in syntax stack.");
    }

    oplex = optoken->getCommandLexNode();
    if ( oplex == nullptr )
        finThrowObj(finErrorKits::EC_READ_ERROR, "Operator token has no command lex node.");

    // Left brackets (round / square / flower) are not arithmetic operators; they are
    // processed by processRightBracket via mesh{Round,Square,Flower}Bracket. Stop the
    // arithmetic drain here instead of reporting a spurious "mismatched after-param count".
    if ( isLeftBracket(oplex) )
        return false;

    afpcnt2 = getOperatorAfParamCnt(oplex);
    if ( afpcnt2 < 0 )
        // Not an arithmetic operator (comma / colon / semicolon / OP_FUNCTION /
        // OP_ACCESS, etc.). Stop the drain; it is the caller's job to handle these
        // tokens through the appropriate path.
        return false;
    if ( afpcnt1 != afpcnt2 )
        finThrowObj(finErrorKits::EC_READ_ERROR, "Arithmetic operator has mismatched after-param count.");

    int bfpcnt = getOperatorBfParamCnt(oplex);
    if ( bfpcnt < 0 )
        return false;  // Same reasoning: non-arithmetic operator; stop the drain.

    if ( bfpcnt > 0 ) {
        oprand1 = this->_syntaxStack.at(afpcnt1 + 1);
        if ( oprand1->getType() != finSyntaxNode::TP_EXPRESS )
            finThrowObj(finErrorKits::EC_READ_ERROR, "Operator expects an expression as before-param.");
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

    return true;
}

void finSyntaxReader::meshAllArithExpress()
{
    while ( this->meshArithExpress() ) { }
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

void finSyntaxReader::meshAllCommas()
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
        return;  // No comma in stack; nothing to mesh.

    // Process E <- E,E,...,E
    finLexNode lexnode;
    lexnode.setType(finLexNode::TP_OPERATOR);
    lexnode.setOperator(finLexNode::OP_COMMA);
    lexnode.setString(QString(","));

    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == nullptr )
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc comma expression node failed.");
    synnode->setType(finSyntaxNode::TP_EXPRESS);
    synnode->setCommandLexNode(&lexnode);

    bool hasinst = false;
    while ( this->_syntaxStack.count() > 0 ) {
        finSyntaxNode *cursyn = this->_syntaxStack.first();
        finLexNode *curlex = cursyn->getCommandLexNode();

        if ( cursyn->getType() ==  finSyntaxNode::TP_EXPRESS ) {
            if ( hasinst )
                finThrowObj(finErrorKits::EC_READ_ERROR, "Comma expression has no operator between operands.");

            this->_syntaxStack.removeFirst();
            synnode->prependSubSyntaxNode(cursyn);
            hasinst = true;
        } else if ( cursyn->getType() == finSyntaxNode::TP_SINGLE &&
                    curlex->getType() == finLexNode::TP_OPERATOR &&
                    curlex->getOperator() == finLexNode::OP_COMMA ) {
            if ( !hasinst ) {
                finSyntaxNode *dmyexpsyn = createDummyExpress();
                if ( dmyexpsyn == nullptr )
                    finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc dummy expression failed.");
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
            finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc dummy expression failed.");
        synnode->prependSubSyntaxNode(dmyexpsyn);
    }

    this->_syntaxStack.prepend(synnode);
}

void finSyntaxReader::meshStatementWithKeywords()
{
    if ( this->_syntaxStack.count() < 2 )
        return;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() != finSyntaxNode::TP_SINGLE ||
         prevlex->getType() != finLexNode::TP_KEYWORD )
        return;

    finSyntaxNode *stttk = this->_syntaxStack.first();
    finLexNode *sttlex = stttk->getCommandLexNode();

    if ( sttlex->getType() == finLexNode::TP_OPERATOR &&
         sttlex->getOperator() == finLexNode::OP_L_FLW_BRCKT ) {
        // Process B <- B'S where B' <- if(E)
        if ( QString::compare(prevlex->getString(), QString("if")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "if statement has unexpected sub-node count.");

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::TP_BRANCH);
            return;
        }

        // Process B <- BB'S where B' <- elif(E) | else and B does not contain
        // 'else'.
        bool handleelse = false;
        if ( QString::compare(prevlex->getString(), QString("elif")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "elif statement has unexpected sub-node count.");
            handleelse = true;
        } else if ( QString::compare(prevlex->getString(), QString("else")) == 0 ) {
            if ( prevtk->getSubListCount() > 0 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "else statement has unexpected sub-node count.");
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
                finThrowObj(finErrorKits::EC_READ_ERROR, "elif/else has no enclosing branch.");

            prevtk = this->_syntaxStack.first();
            if ( prevtk->getType() != finSyntaxNode::TP_BRANCH )
                finThrowObj(finErrorKits::EC_READ_ERROR, "elif/else must follow a branch.");
            if ( prevtk->getSubListCount() % 2 != 0 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "Branch has mismatched sub-node count.");

            if ( condtk != nullptr )
                prevtk->appendSubSyntaxNode(condtk);
            prevtk->appendSubSyntaxNode(stttk);
            return;
        }

        // Process L <- L'S where L' <- for(S;S;E) | while(E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ||
             QString::compare(prevlex->getString(), QString("while")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "Loop head has unexpected sub-node count.");

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::TP_LOOP);
            return;
        }
    }

    if ( sttlex->getType() == finLexNode::TP_OPERATOR &&
         sttlex->getOperator() == finLexNode::OP_SPLIT ) {
        // Process J <- J'E; where J' <- goto, and E <- var
        if ( QString::compare(prevlex->getString(), QString("goto")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 1 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "goto has unexpected sub-node count.");

            finSyntaxNode *sttsubtk = stttk->pickSubSyntaxNode(0);
            finLexNode *sttsublex = sttsubtk->getCommandLexNode();
            prevtk->appendSubSyntaxNode(sttsubtk);

            if ( sttsubtk->getType() != finSyntaxNode::TP_EXPRESS ||
                 sttsublex->getType() != finLexNode::TP_VARIABLE )
                finThrowObj(finErrorKits::EC_READ_ERROR, "goto target must be a variable.");

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_JUMP);
            return;
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
            return;
        }

        // Process J <- J'; where J' <- break | continue
        if ( QString::compare(prevlex->getString(), QString("break")) == 0 ||
             QString::compare(prevlex->getString(), QString("continue")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 0 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "break/continue has unexpected sub-node count.");

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_JUMP);
            return;
        }

        // Process D <- var E;
        if ( QString::compare(prevlex->getString(), QString("var")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 1 )
                finThrowObj(finErrorKits::EC_READ_ERROR, "var declaration has unexpected sub-node count.");

            prevtk->appendSubSyntaxNode(stttk->pickSubSyntaxNode(0));

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::TP_DECLARE);
            return;
        }
    }
}

void finSyntaxReader::recogFlowerBracketType()
{
    finSyntaxNode *flwbtk = this->_syntaxStack.at(0);
    if ( flwbtk->getSubListCount() < 1 ) {
        flwbtk->setType(finSyntaxNode::TP_STATEMENT);
        return;
    }

    finSyntaxNode *curtk = flwbtk->getSubSyntaxNode(0);
    bool isstt = finSyntaxNode::isStatementLevelType(curtk->getType());
    if ( !isstt ) {
        if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
            finThrowObj(finErrorKits::EC_READ_ERROR,
                        "Flower bracket contains an invalid syntax node type.");
    }

    for ( int i = 1; i < flwbtk->getSubListCount(); i++ ) {
        curtk = flwbtk->getSubSyntaxNode(i);
        if ( isstt ) {
            if ( !finSyntaxNode::isStatementLevelType(curtk->getType()) )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "Statement flower bracket contains a non-statement node.");
        } else {
            if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "Expression flower bracket contains a non-expression node.");
        }
    }
    if ( isstt )
        flwbtk->setType(finSyntaxNode::TP_STATEMENT);
    else
        flwbtk->setType(finSyntaxNode::TP_EXPRESS);
}

void finSyntaxReader::meshFlowerBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return;

    this->recogFlowerBracketType();

    // Process F <- E{S}, where E <- E(E)
    finSyntaxNode *curtk = this->_syntaxStack.at(0);
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS &&
         prevlex != nullptr &&
         prevlex->getType() == finLexNode::TP_OPERATOR &&
         prevlex->getOperator() == finLexNode::OP_FUNCTION ) {

        if ( curtk->getType() != finSyntaxNode::TP_STATEMENT )
            finThrowObj(finErrorKits::EC_READ_ERROR,
                        "Function body must be a statement flower bracket.");

        this->_syntaxStack.removeFirst();
        prevtk->setType(finSyntaxNode::TP_FUNCTION);
        prevtk->appendSubSyntaxNode(curtk);

        return;
    }

    if ( curtk->getType() == finSyntaxNode::TP_STATEMENT )
        this->meshStatementWithKeywords();

    // TODO: Flower bracket cases not yet implemented.
    finThrowObj(finErrorKits::EC_NON_IMPLEMENT, "Unhandled flower bracket case.");
}

void finSyntaxReader::meshRoundBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);

    // Process E <- E(E)
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        int brcktk_subcnt = brcktk->getSubListCount();
        // The bracket may legitimately contain more than one subnode (e.g., the script
        // wrote `f(a b c)` with no commas — those are still treated as a sequence of
        // arguments). Do not throw on structural mismatches; just create the function
        // call with whatever subnodes the bracket has.
        if ( brcktk_subcnt > 0 && brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
            return;

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::TP_OPERATOR);
        meshedlex.setOperator(finLexNode::OP_FUNCTION);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == nullptr )
            finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc function-call node failed.");
        meshedtk->setType(finSyntaxNode::TP_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return;
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
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "if/elif/while head must contain exactly one sub-node.");
            if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "if/elif/while head must be an expression.");

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return;
        }

        // Process L' <- for(S;S;E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ) {
            finSyntaxNode *brcktk = this->_syntaxStack.at(0);
            if ( brcktk->getSubListCount() != 3 )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "for head must contain exactly three sub-nodes.");

            finSyntaxNodeType sub0type = brcktk->getSubSyntaxNode(0)->getType(),
                              sub1type = brcktk->getSubSyntaxNode(1)->getType(),
                              sub2type = brcktk->getSubSyntaxNode(2)->getType();
            if ( !(finSyntaxNode::isStatementLevelType(sub0type) &&
                   sub0type != finSyntaxNode::TP_LABEL && sub0type != finSyntaxNode::TP_JUMP) )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "for init head must be a simple statement (no label or jump).");
            if ( sub1type != finSyntaxNode::TP_STATEMENT )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "for condition head must be a statement.");
            if ( sub2type != finSyntaxNode::TP_EXPRESS )
                finThrowObj(finErrorKits::EC_READ_ERROR,
                            "for step head must be an expression.");

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return;
        }
    }
}

void finSyntaxReader::meshSquareBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return;

    // Process E <- E[E]
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    if ( prevtk->getType() == finSyntaxNode::TP_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        if ( brcktk->getSubListCount() != 1 )
            finThrowObj(finErrorKits::EC_READ_ERROR,
                        "Array access must contain exactly one sub-node.");
        if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::TP_EXPRESS )
            finThrowObj(finErrorKits::EC_READ_ERROR,
                        "Array access sub-node must be an expression.");

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::TP_OPERATOR);
        meshedlex.setOperator(finLexNode::OP_ACCESS);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == nullptr )
            finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc array-access node failed.");
        meshedtk->setType(finSyntaxNode::TP_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return;
    }
}



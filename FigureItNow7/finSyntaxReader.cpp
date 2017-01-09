#include "finSyntaxReader.h"

finSyntaxReader::finSyntaxReader()
    : _lexReader(), _syntaxStack()
{
    this->_isReading = false;
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
    if ( this->_isReading )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_lexReader.resetPosition();
    finErrorCode errcode = this->_lexReader.setString(scriptcode);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finSyntaxReader::isReading() const
{
    return this->_isReading;
}

finErrorCode finSyntaxReader::startRead()
{
    if ( this->_isReading )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->disposeAllRead();

    finErrorCode errcode = this->_lexReader.resetPosition();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->_isReading = true;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::stopRead()
{
    if ( !this->_isReading )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_isReading = false;
    this->disposeAllRead();

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::readNextToken()
{
    if ( !this->_isReading )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finLexNode lexnode;
    finErrorCode errcode = this->_lexReader.getNextLexNode(&lexnode);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    if ( errcode == finErrorCodeKits::FIN_EC_REACH_BOTTOM )
        return errcode;
    if ( lexnode.getType() == finLexNode::FIN_LN_TYPE_DUMMY )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    return this->processTypedNextToken(&lexnode, lexnode.getType());
}

finSyntaxTree *finSyntaxReader::getSyntaxTree()
{
    if ( !this->_isReading )
        return NULL;

    finSyntaxTree *syntree = new finSyntaxTree();
    if ( syntree == NULL )
        return NULL;

    finErrorCode errcode = syntree->appendSyntaxNodeStack(&this->_syntaxStack);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete syntree;
        return NULL;
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
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype)
{
    switch ( lextype ) {
      case finLexNode::FIN_LN_TYPE_KEYWORD:
      case finLexNode::FIN_LN_TYPE_VARIABLE:
      case finLexNode::FIN_LN_TYPE_DECIMAL:
      case finLexNode::FIN_LN_TYPE_STRING:
        return processInstanceToken(lexnode);

      case finLexNode::FIN_LN_TYPE_OPERATOR:
        return processOperatorToken(lexnode);

      default:
        return processOperatorToken(lexnode);
    }
}

finErrorCode finSyntaxReader::processInstanceToken(finLexNode *lexnode)
{
    finSyntaxNodeType tokentype;
    switch ( lexnode->getType() ) {
      case finLexNode::FIN_LN_TYPE_VARIABLE:
      case finLexNode::FIN_LN_TYPE_DECIMAL:
      case finLexNode::FIN_LN_TYPE_STRING:
        tokentype = finSyntaxNode::FIN_SN_TYPE_EXPRESS;
        break;

      default:
        tokentype = finSyntaxNode::FIN_SN_TYPE_SINGLE;
        break;
    }

    return this->pushSingleLexNode(lexnode, tokentype);
}

finErrorCode finSyntaxReader::processVirtualToken(finLexNode *lexnode)
{
    if ( lexnode == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    // Do nothing with the lex node, and release the memory merely.
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    { finLexNode::FIN_LN_OPTYPE_ADD,       0x1200, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_POSITIVE  },
    { finLexNode::FIN_LN_OPTYPE_SUB,       0x1200, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_NEGATIVE  },
    { finLexNode::FIN_LN_OPTYPE_POSITIVE,  0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_NEGATIVE,  0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT,   0x2100, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 0, finLexNode::FIN_LN_OPTYPE_ACCUMLT_2 },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT_2, 0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_MUL,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_DIV,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_MOD,       0x1240, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_POWER,     0x1280, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LET,       0x0100, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_EQUAL,     0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_GRT,       0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LES,       0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_NONEQUAL,  0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_GRT_EQ,    0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LES_EQ,    0x0600, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_NOT, 0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_AND, 0x0340, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_OR,  0x0320, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_XOR, 0x0340, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_BIT_NOT,   0x2000, _finSynLexOpTableItem::FIN_MD_R2L,
                                                        0, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_BIT_AND,   0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_BIT_OR,    0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     },
    { finLexNode::FIN_LN_OPTYPE_BIT_XOR,   0x1140, _finSynLexOpTableItem::FIN_MD_L2R,
                                                        1, 1, finLexNode::FIN_LN_OPTYPE_DUMMY     }
};

static struct _finSynLexOpTableItem *getLexOpTableItem(finLexOperatorType type)
{
    for ( int i = 0; i < (int)(sizeof (_finLexOpTable) / sizeof (struct _finSynLexOpTableItem)); i++ ) {
        if ( _finLexOpTable[i].type == type )
            return &_finLexOpTable[i];
    }
    return NULL;
}

int finSyntaxReader::getOperatorBfParamCnt(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return -1;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
        return -1;

    return tblinfo->bfParamCnt;
}

int finSyntaxReader::getOperatorAfParamCnt(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return -1;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
        return -1;

    return tblinfo->afParamCnt;
}

finErrorCode finSyntaxReader::transformOpToNonBfParamOp(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    if ( tblinfo->zpTransOp == finLexNode::FIN_LN_OPTYPE_DUMMY)
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    lexnode->setOperator(tblinfo->zpTransOp);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

int finSyntaxReader::getOperatorPriority(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return 0x0;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
        return 0x0;

    return tblinfo->priority;
}

int finSyntaxReader::getOperatorMeshDir(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return (int)_finSynLexOpTableItem::FIN_MD_L2R;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
        return (int)_finSynLexOpTableItem::FIN_MD_L2R;

    return (int)tblinfo->meshDir;
}

bool finSyntaxReader::isArithOperator(finLexNode *lexnode)
{
    if (lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR)
        return false;

    struct _finSynLexOpTableItem *tblinfo = getLexOpTableItem(lexnode->getOperator());
    if ( tblinfo == NULL )
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
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    finSyntaxNode *prevtoken = NULL;
    if ( this->_syntaxStack.count() > 0 )
        prevtoken = this->_syntaxStack.at(0);

    bool prev_is_express = false;
    if ( prevtoken != NULL && prevtoken->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS )
        prev_is_express = true;

    if ( bfparamcnt > 0 && !prev_is_express ) {
        errcode = transformOpToNonBfParamOp(lexnode);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        }
        bfparamcnt = 0/*getOperatorBfParamCnt(lexnode)*/;
    }

    while ( true ) {
        finLexNode *prevop = NULL;
        if ( this->_syntaxStack.count() > bfparamcnt ) {
            finSyntaxNode *preopsn = this->_syntaxStack.at(bfparamcnt);
            if ( preopsn->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE ) {
                prevop = preopsn->getCommandLexNode();
                if ( prevop->getType() != finLexNode::FIN_LN_TYPE_OPERATOR )
                    prevop = NULL;
            }
        }

        if ( prevop == NULL )
            break;
        if ( compareOperators (prevop, lexnode) < 0 )
            break;

        errcode = this->meshArithExpress();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }

    finSyntaxNode *optoken = new finSyntaxNode();
    optoken->setType(finSyntaxNode::FIN_SN_TYPE_SINGLE);
    optoken->setCommandLexNode(lexnode);
    this->_syntaxStack.prepend(optoken);

    if ( afparamcnt < 1 ) {
        errcode = this->meshArithExpress();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finSyntaxReader::isLeftBracket(finLexNode *lexnode)
{
    switch ( lexnode->getOperator() ) {
      case finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT:
      case finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT:
      case finLexNode::FIN_LN_OPTYPE_L_SQR_BRCKT:
        return true;

      default:
        return false;
    }
}

bool finSyntaxReader::isRightBracket(finLexNode *lexnode)
{
    switch ( lexnode->getOperator() ) {
      case finLexNode::FIN_LN_OPTYPE_R_FLW_BRCKT:
      case finLexNode::FIN_LN_OPTYPE_R_RND_BRCKT:
      case finLexNode::FIN_LN_OPTYPE_R_SQR_BRCKT:
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
    finLexNode::finLexNodeOperatorType ltype, rtype;
    if (isLeftBracket(lexnode1)) {
        ltype = lexnode1->getOperator();
        rtype = lexnode2->getOperator();
    } else {
        ltype = lexnode2->getOperator();
        rtype = lexnode1->getOperator();
    }

    if ( ltype == finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT ) {
        if ( rtype == finLexNode::FIN_LN_OPTYPE_R_FLW_BRCKT )
            return true;
        else
            return false;
    } else if ( ltype == finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        if ( rtype == finLexNode::FIN_LN_OPTYPE_R_RND_BRCKT )
            return true;
        else
            return false;
    } else if ( ltype == finLexNode::FIN_LN_OPTYPE_L_SQR_BRCKT ) {
        if ( rtype == finLexNode::FIN_LN_OPTYPE_R_SQR_BRCKT )
            return true;
        else
            return false;
    } else {
        return false;
    }
}

finErrorCode finSyntaxReader::processLeftBracket(finLexNode *lexnode)
{
    return this->pushSingleLexNode(lexnode, finSyntaxNode::FIN_SN_TYPE_SINGLE);
}

finErrorCode finSyntaxReader::processRightBracket(finLexNode *lexnode)
{
    finErrorCode errcode;
    errcode = this->meshAllArithExpress();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->meshAllCommas();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finSyntaxNode *meshednode = new finSyntaxNode();
    if ( meshednode == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    meshednode->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);

    bool foundcosbrk = false;
    while ( this->_syntaxStack.count() > 0) {
        finSyntaxNode *curnode = this->_syntaxStack.first();
        finLexNode *curlex = curnode->getCommandLexNode();
        this->_syntaxStack.removeFirst();

        if ( curnode->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE && isLeftBracket(curlex) ) {
            if ( isCorrespnBracket(curlex, lexnode) ) {
                meshednode->setCommandLexNode(curlex);
                delete curnode;
                foundcosbrk = true;
                break;
            } else {
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            }
        }

        meshednode->prependSubSyntaxNode(curnode);
    }
    if ( !foundcosbrk )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    this->_syntaxStack.prepend(meshednode);

    switch ( lexnode->getOperator() ) {
      case finLexNode::FIN_LN_OPTYPE_R_FLW_BRCKT:
        this->meshFlowerBracket();
        break;
      case finLexNode::FIN_LN_OPTYPE_R_RND_BRCKT:
        this->meshRoundBracket();
        break;
      case finLexNode::FIN_LN_OPTYPE_R_SQR_BRCKT:
        this->meshSquareBracket();
        break;
      default:
        break;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
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
      case finLexNode::FIN_LN_OPTYPE_SPLIT:
      case finLexNode::FIN_LN_OPTYPE_COMMA:
      case finLexNode::FIN_LN_OPTYPE_COLON:
        return true;

      default:
        return false;
    }
}

finErrorCode finSyntaxReader::processSplitter(finLexNode *lexnode)
{
    finErrorCode errcode;
    errcode = this->meshAllArithExpress();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finSyntaxNode *presynnode = NULL, *synnode = NULL;

    switch ( lexnode->getOperator() ) {
      // Process S <- E; and S <- ; (empty statement)
      case finLexNode::FIN_LN_OPTYPE_SPLIT:
        errcode = this->meshAllCommas();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        synnode = new finSyntaxNode();
        if ( synnode == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        synnode->setType(finSyntaxNode::FIN_SN_TYPE_STATEMENT);
        synnode->setCommandLexNode(lexnode);

        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);
        if ( presynnode != NULL && presynnode->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
            synnode->appendSubSyntaxNode(presynnode);
            this->_syntaxStack.removeFirst();
        }
        this->_syntaxStack.prepend(synnode);

        errcode = this->meshStatementWithKeywords();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        break;

      // Process L <- E:, where E <- var
      case finLexNode::FIN_LN_OPTYPE_COLON:
        if ( this->_syntaxStack.count() > 0 )
            presynnode = this->_syntaxStack.at(0);

        if ( presynnode == NULL || presynnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        if ( presynnode->getCommandLexNode()->getType() != finLexNode::FIN_LN_TYPE_VARIABLE )
            return finErrorCodeKits::FIN_EC_READ_ERROR;

        presynnode->setType(finSyntaxNode::FIN_SN_TYPE_LABEL);
        break;

      default:
        synnode = new finSyntaxNode();
        if ( synnode == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        synnode->setType(finSyntaxNode::FIN_SN_TYPE_SINGLE);
        synnode->setCommandLexNode(lexnode);
        this->_syntaxStack.prepend(synnode);
        break;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype)
{
    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finErrorCode errcode;
    errcode = synnode->setType(tktype);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = synnode->setCommandLexNode(lexnode);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->_syntaxStack.prepend(synnode);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshArithExpress()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    finSyntaxNode *token0, *token1;
    token0 = this->_syntaxStack.at(0);
    token1 = this->_syntaxStack.at(1);

    finSyntaxNode *optoken = NULL;
    finLexNode *oplex = NULL;
    finSyntaxNode *oprand1 = NULL, *oprand2 = NULL;

    int afpcnt1, afpcnt2;
    if ( token0->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE ) {
        afpcnt1 = 0;
        optoken = token0;
        oprand2 = new finSyntaxNode();
    } else if ( token0->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        if ( token1->getType() != finSyntaxNode::FIN_SN_TYPE_SINGLE )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        afpcnt1 = 1;
        optoken = token1;
        oprand2 = token0;
    } else {
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    oplex = optoken->getCommandLexNode();
    if ( oplex == NULL )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    afpcnt2 = getOperatorAfParamCnt(oplex);
    if ( afpcnt2 < 0 || afpcnt1 != afpcnt2 )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    int bfpcnt = getOperatorBfParamCnt(oplex);
    if ( bfpcnt < 0 )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    if ( bfpcnt > 0 ) {
        oprand1 = this->_syntaxStack.at(afpcnt1 + 1);
        if ( oprand1->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    this->_syntaxStack.removeFirst();
    this->_syntaxStack.removeFirst();
    if ( bfpcnt > 0 && afpcnt1 > 0 )
        this->_syntaxStack.removeFirst();

    optoken->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
    //optoken->setCommandLexNode(oplex);
    if ( bfpcnt > 0 )
        optoken->appendSubSyntaxNode(oprand1);
    optoken->appendSubSyntaxNode(oprand2);
    this->_syntaxStack.prepend(optoken);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshAllArithExpress()
{
    while ( this->meshArithExpress() == finErrorCodeKits::FIN_EC_SUCCESS );
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finSyntaxNode *finSyntaxReader::createDummyExpress()
{
    finLexNode dmyexplex;
    dmyexplex.setType(finLexNode::FIN_LN_TYPE_DUMMY);

    finSyntaxNode *dmyexpsyn = new finSyntaxNode();
    if ( dmyexpsyn == NULL )
        return NULL;

    dmyexpsyn->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
    dmyexpsyn->setCommandLexNode(&dmyexplex);
    return dmyexpsyn;
}

finErrorCode finSyntaxReader::meshAllCommas()
{
    bool hascomma =  false;
    for ( int i = 0; i < this->_syntaxStack.count(); i++ ) {
        finSyntaxNode *cursyn = this->_syntaxStack.at(i);
        finLexNode *curlex = cursyn->getCommandLexNode();
        if ( cursyn->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS )
            continue;

        if ( cursyn->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE &&
             curlex->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
             curlex->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
            hascomma = true;
            break;
        } else {
            hascomma = false;
            break;
        }
    }
    if ( !hascomma )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    // Process E <- E,E,...,E
    finLexNode lexnode;
    lexnode.setType(finLexNode::FIN_LN_TYPE_OPERATOR);
    lexnode.setOperator(finLexNode::FIN_LN_OPTYPE_COMMA);
    lexnode.setString(QString(","));

    finSyntaxNode *synnode = new finSyntaxNode();
    if ( synnode == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    synnode->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
    synnode->setCommandLexNode(&lexnode);

    bool hasinst = false;
    while ( this->_syntaxStack.count() > 0 ) {
        finSyntaxNode *cursyn = this->_syntaxStack.first();
        finLexNode *curlex = cursyn->getCommandLexNode();

        if ( cursyn->getType() ==  finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
            if ( hasinst )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            synnode->prependSubSyntaxNode(cursyn);
            hasinst = true;
        } else if ( cursyn->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE &&
                    curlex->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
                    curlex->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
            if ( !hasinst ) {
                finSyntaxNode *dmyexpsyn = createDummyExpress();
                if ( dmyexpsyn == NULL )
                    return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
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
        if ( dmyexpsyn == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        synnode->prependSubSyntaxNode(dmyexpsyn);
    }

    this->_syntaxStack.prepend(synnode);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshStatementWithKeywords()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() != finSyntaxNode::FIN_SN_TYPE_SINGLE ||
         prevlex->getType() != finLexNode::FIN_LN_TYPE_KEYWORD )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finSyntaxNode *stttk = this->_syntaxStack.first();
    finLexNode *sttlex = stttk->getCommandLexNode();

    if ( sttlex->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         sttlex->getOperator() == finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT ) {
        // Process B <- B'S where B' <- if(E)
        if ( QString::compare(prevlex->getString(), QString("if")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::FIN_SN_TYPE_BRANCH);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }

        // Process B <- BB'S where B' <- elif(E) | else and B does not contain
        // 'else'.
        bool handleelse = false;
        if ( QString::compare(prevlex->getString(), QString("elif")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            handleelse = true;
        } else if ( QString::compare(prevlex->getString(), QString("else")) == 0 ) {
            if ( prevtk->getSubListCount() > 0 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            handleelse = true;
        }

        if ( handleelse ) {
            finSyntaxNode *condtk = NULL;
            if ( prevtk->getSubListCount() > 0 )
                condtk = prevtk->pickSubSyntaxNode(0);

            this->_syntaxStack.removeFirst();
            this->_syntaxStack.removeFirst();
            delete prevtk;
            if ( this->_syntaxStack.count() <= 0 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            prevtk = this->_syntaxStack.first();
            if ( prevtk->getType() != finSyntaxNode::FIN_SN_TYPE_BRANCH )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            if ( prevtk->getSubListCount() % 2 != 0 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            if ( condtk != NULL )
                prevtk->appendSubSyntaxNode(condtk);
            prevtk->appendSubSyntaxNode(stttk);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }

        // Process L <- L'S where L' <- for(S;S;E) | while(E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ||
             QString::compare(prevlex->getString(), QString("while")) == 0 ) {
            if ( prevtk->getSubListCount() != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk);
            this->_syntaxStack.removeFirst();
            prevtk->setType(finSyntaxNode::FIN_SN_TYPE_LOOP);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }
    }

    if ( sttlex->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         sttlex->getOperator() == finLexNode::FIN_LN_OPTYPE_SPLIT ) {
        // Process J <- J'; where J' <- goto(E) | return(E) | exit(E)
        // Process J <- J'E; where J' <- goto | return | exit
        if ( QString::compare(prevlex->getString(), QString("goto")) == 0 ||
             QString::compare(prevlex->getString(), QString("return")) == 0 ||
             QString::compare(prevlex->getString(), QString("exit")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt + sttsubcnt != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            if ( sttsubcnt > 0 )
                prevtk->appendSubSyntaxNode(stttk->pickSubSyntaxNode(0));

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::FIN_SN_TYPE_JUMP);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }

        // Process J <- J'; where J' <- break | continue
        if ( QString::compare(prevlex->getString(), QString("break")) == 0 ||
             QString::compare(prevlex->getString(), QString("continue")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 0 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::FIN_SN_TYPE_JUMP);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }

        // Process D <- var E;
        if ( QString::compare(prevlex->getString(), QString("var")) == 0 ) {
            int prevsubcnt = prevtk->getSubListCount();
            int sttsubcnt = stttk->getSubListCount();
            if ( prevsubcnt != 0 || sttsubcnt != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            prevtk->appendSubSyntaxNode(stttk->pickSubSyntaxNode(0));

            this->_syntaxStack.removeFirst();
            delete stttk;
            prevtk->setType(finSyntaxNode::FIN_SN_TYPE_DECLARE);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::recogFlowerBracketType()
{
    finSyntaxNode *flwbtk = this->_syntaxStack.at(0);
    if ( flwbtk->getSubListCount() < 1 ) {
        flwbtk->setType(finSyntaxNode::FIN_SN_TYPE_STATEMENT);
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    finSyntaxNode *curtk = flwbtk->getSubSyntaxNode(0);
    bool isstt = finSyntaxNode::isStatementLevelType(curtk->getType());
    if ( !isstt ) {
        if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    for ( int i = 1; i < flwbtk->getSubListCount(); i++ ) {
        curtk = flwbtk->getSubSyntaxNode(i);
        if ( isstt ) {
            if ( !finSyntaxNode::isStatementLevelType(curtk->getType()) )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
        } else {
            if ( !finSyntaxNode::isExpressLevelType(curtk->getType()) )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
        }
    }
    if ( isstt )
        flwbtk->setType(finSyntaxNode::FIN_SN_TYPE_STATEMENT);
    else
        flwbtk->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshFlowerBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finErrorCode errcode = this->recogFlowerBracketType();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    // Process F <- E{S}, where E <- E(E)
    finSyntaxNode *curtk = this->_syntaxStack.at(0);
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS &&
         prevlex != NULL &&
         prevlex->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         prevlex->getOperator() == finLexNode::FIN_LN_OPTYPE_FUNCTION ) {

        if ( curtk->getType() != finSyntaxNode::FIN_SN_TYPE_STATEMENT )
            return finErrorCodeKits::FIN_EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        prevtk->setType(finSyntaxNode::FIN_SN_TYPE_FUNCTION);
        prevtk->appendSubSyntaxNode(curtk);

        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    if ( curtk->getType() == finSyntaxNode::FIN_SN_TYPE_STATEMENT ) {
        errcode = this->meshStatementWithKeywords();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxReader::meshRoundBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finSyntaxNode *prevtk = this->_syntaxStack.at(1);

    // Process E <- E(E)
    if ( prevtk->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        int brcktk_subcnt = brcktk->getSubListCount();
        if ( brcktk_subcnt > 1 )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        if ( brcktk_subcnt > 0 && brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
            return finErrorCodeKits::FIN_EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::FIN_LN_TYPE_OPERATOR);
        meshedlex.setOperator(finLexNode::FIN_LN_OPTYPE_FUNCTION);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        meshedtk->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    finLexNode *prevlex = prevtk->getCommandLexNode();
    if ( prevtk->getType() == finSyntaxNode::FIN_SN_TYPE_SINGLE &&
         prevlex->getType() == finLexNode::FIN_LN_TYPE_KEYWORD ) {

        // Process B' <- if(E) , B' <- elif(E) , and L' <- while(E)
        if ( QString::compare(prevlex->getString(), QString("if")) == 0 ||
             QString::compare(prevlex->getString(), QString("elif")) == 0 ||
             QString::compare(prevlex->getString(), QString("while")) == 0 ) {
            finSyntaxNode *brcktk = this->_syntaxStack.at(0);
            if ( brcktk->getSubListCount() != 1 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }

        // Process L' <- for(S;S;E)
        if ( QString::compare(prevlex->getString(), QString("for")) == 0 ) {
            finSyntaxNode *brcktk = this->_syntaxStack.at(0);
            if ( brcktk->getSubListCount() != 3 )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            finSyntaxNodeType sub0type = brcktk->getSubSyntaxNode(0)->getType(),
                              sub1type = brcktk->getSubSyntaxNode(1)->getType(),
                              sub2type = brcktk->getSubSyntaxNode(2)->getType();
            if ( !(finSyntaxNode::isStatementLevelType(sub0type) &&
                   sub0type != finSyntaxNode::FIN_SN_TYPE_LABEL && sub0type != finSyntaxNode::FIN_SN_TYPE_JUMP) )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            if ( sub1type != finSyntaxNode::FIN_SN_TYPE_STATEMENT )
                return finErrorCodeKits::FIN_EC_READ_ERROR;
            if ( sub2type != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
                return finErrorCodeKits::FIN_EC_READ_ERROR;

            this->_syntaxStack.removeFirst();
            prevtk->appendSubSyntaxNode(brcktk);
            return finErrorCodeKits::FIN_EC_SUCCESS;
        }
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxReader::meshSquareBracket()
{
    if ( this->_syntaxStack.count() < 2 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    // Process E <- E[E]
    finSyntaxNode *prevtk = this->_syntaxStack.at(1);
    if ( prevtk->getType() == finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        finSyntaxNode *brcktk = this->_syntaxStack.at(0);
        if ( brcktk->getSubListCount() != 1 )
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        if ( brcktk->getSubSyntaxNode(0)->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
            return finErrorCodeKits::FIN_EC_READ_ERROR;

        this->_syntaxStack.removeFirst();
        this->_syntaxStack.removeFirst();

        finLexNode meshedlex;
        meshedlex.setType(finLexNode::FIN_LN_TYPE_OPERATOR);
        meshedlex.setOperator(finLexNode::FIN_LN_OPTYPE_ACCESS);

        finSyntaxNode *meshedtk = new finSyntaxNode();
        if ( meshedtk == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        meshedtk->setType(finSyntaxNode::FIN_SN_TYPE_EXPRESS);
        meshedtk->setCommandLexNode(&meshedlex);
        meshedtk->appendSubSyntaxNode(prevtk);
        meshedtk->appendSubSyntaxNode(brcktk);

        this->_syntaxStack.prepend(meshedtk);
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}



#include "finSyntaxNode.h"

finSyntaxNode::finSyntaxNode()
    : _subSyntaxList()
{
    this->_type = FIN_SN_TYPE_DUMMY;
    this->_cmdLexNode = NULL;
}

finSyntaxNodeType finSyntaxNode::getType() const
{
    return this->_type;
}

finLexNode *finSyntaxNode::getCommandLexNode() const
{
    return this->_cmdLexNode;
}

int finSyntaxNode::getSubListCount() const
{
    return this->_subSyntaxList.count();
}

finSyntaxNode *finSyntaxNode::getSubSyntaxNode(int idx) const
{
    return this->_subSyntaxList.at(idx);
}

finErrorCode finSyntaxNode::setType(finSyntaxNodeType type)
{
    if ( this->_type == type )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_type = type;

    if ( type == FIN_SN_TYPE_DUMMY || type == FIN_SN_TYPE_MAX )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxNode::setCommandLexNode(finLexNode *lexnode)
{
    this->_cmdLexNode = lexnode;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxNode::appendSubSyntaxNode(finSyntaxNode *synnode)
{
    this->_subSyntaxList.append(synnode);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxNode::prependSubSyntaxNode(finSyntaxNode *synnode)
{
    this->_subSyntaxList.prepend(synnode);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finSyntaxNode::isExpressLevelType(finSyntaxNodeType type)
{
    switch ( type ) {
      case FIN_SN_TYPE_EXPRESS:
      case FIN_SN_TYPE_SINGLE:
        return true;

      default:
        return false;
    }
}

finErrorCode finSyntaxNode::disposeCommandLexNode()
{
    if ( this->_cmdLexNode == NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    delete this->_cmdLexNode;
    this->_cmdLexNode = NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxNode::disposeSubSyntaxNodes()
{
    if ( this->_subSyntaxList.count() == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    while ( this->_subSyntaxList.count() > 0 ) {
        finSyntaxNode *subtk = this->_subSyntaxList.at(0);
        this->_subSyntaxList.removeFirst();
        subtk->disposeAll();
        delete subtk;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxNode::disposeAll()
{
    finErrorCode errcode1, errcode2;

    errcode1 = this->disposeCommandLexNode();
    errcode2 = this->disposeSubSyntaxNodes();

    if ( errcode1 == finErrorCodeKits::FIN_EC_SUCCESS )
        return errcode2;
    else
        return errcode1;
}

bool finSyntaxNode::isStatementLevelType(finSyntaxNodeType type)
{
    switch ( type ) {
      case FIN_SN_TYPE_BRANCH:
      case FIN_SN_TYPE_JUMP:
      case FIN_SN_TYPE_LABEL:
      case FIN_SN_TYPE_LOOP:
      case FIN_SN_TYPE_STATEMENT:
        return true;

      default:
        return false;
    }
}

void finSyntaxNode::dump() const
{
    this->dumpLeveled(0);
    fflush(stdout);
}

void finSyntaxNode::dumpLeveled(int level) const
{
    finLexNode *cmdlex = this->getCommandLexNode();

    for (int i = 0; i < level; i++)
        printf("  ");

    printf("* type=%d, ", (int)this->getType());
    if ( cmdlex != NULL ) {
        printf("lex=%d, str=%s, ", (int)cmdlex->getType(), cmdlex->getString().toLatin1().data());
    } else {
        printf("lex=NULL, ");
    }
    printf("sublen=%d\n", this->getSubListCount());

    for (int i = 0; i < this->getSubListCount(); i++) {
        this->getSubSyntaxNode(i)->dumpLeveled(level + 1);
    }
}


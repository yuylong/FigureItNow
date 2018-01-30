/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxNode.h"


finSyntaxNode::finSyntaxNode()
    : _cmdLexNode(), _subSyntaxList()
{
    this->_type = FIN_SN_TYPE_DUMMY;
}

finSyntaxNode::~finSyntaxNode()
{
    this->disposeAll();
}

finErrorCode finSyntaxNode::copyNode(const finSyntaxNode *srcnode)
{
    finErrorCode errcode;
    this->disposeAll();

    if ( srcnode == NULL )
        return finErrorKits::EC_SUCCESS;

    this->_type = srcnode->getType();

    errcode = this->_cmdLexNode.copyNode(srcnode->getCommandLexNode());
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    for ( int i = 0; i < srcnode->getSubListCount(); i++ ) {
        finSyntaxNode *synnode = new finSyntaxNode();
        if ( synnode == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        errcode = synnode->copyNode(srcnode->getSubSyntaxNode(i));
        if ( finErrorKits::isErrorResult(errcode) ) {
            synnode->disposeAll();
            delete synnode;
            return errcode;
        }

        errcode = this->appendSubSyntaxNode(synnode);
        if ( finErrorKits::isErrorResult(errcode) ) {
            synnode->disposeAll();
            delete synnode;
            return errcode;
        }
    }
    return finErrorKits::EC_SUCCESS;
}

finSyntaxNodeType finSyntaxNode::getType() const
{
    return this->_type;
}

const finLexNode *finSyntaxNode::getCommandLexNode() const
{
    if ( this->_cmdLexNode.getType() == finLexNode::TP_DUMMY )
        return NULL;
    else
        return &this->_cmdLexNode;
}

finLexNode *finSyntaxNode::getCommandLexNode()
{
    if ( this->_cmdLexNode.getType() == finLexNode::TP_DUMMY )
        return NULL;
    else
        return &this->_cmdLexNode;
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
        return finErrorKits::EC_DUPLICATE_OP;

    this->_type = type;

    if ( type == FIN_SN_TYPE_DUMMY || type == FIN_SN_TYPE_MAX )
        return finErrorKits::EC_NORMAL_WARN;
    else
        return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxNode::setCommandLexNode(const finLexNode *lexnode)
{
    this->_cmdLexNode.copyNode(lexnode);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxNode::appendSubSyntaxNode(finSyntaxNode *synnode)
{
    this->_subSyntaxList.append(synnode);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxNode::prependSubSyntaxNode(finSyntaxNode *synnode)
{
    this->_subSyntaxList.prepend(synnode);
    return finErrorKits::EC_SUCCESS;
}

finSyntaxNode *finSyntaxNode::pickSubSyntaxNode(int idx)
{
    if ( idx < 0 || idx >= this->_subSyntaxList.count() )
        return NULL;

    finSyntaxNode *retnode = this->_subSyntaxList.at(idx);
    this->_subSyntaxList.removeAt(idx);
    return retnode;
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
    this->_cmdLexNode.setType(finLexNode::TP_DUMMY);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxNode::disposeSubSyntaxNodes()
{
    if ( this->_subSyntaxList.count() == 0 )
        return finErrorKits::EC_DUPLICATE_OP;

    while ( this->_subSyntaxList.count() > 0 ) {
        finSyntaxNode *subtk = this->_subSyntaxList.at(0);
        this->_subSyntaxList.removeFirst();
        delete subtk;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxNode::disposeAll()
{
    finErrorCode errcode1, errcode2;

    errcode1 = this->disposeCommandLexNode();
    errcode2 = this->disposeSubSyntaxNodes();

    if ( finErrorKits::isErrorResult(errcode1) )
        return errcode1;
    else if ( finErrorKits::isErrorResult(errcode2) )
        return errcode2;
    else
        return finErrorKits::EC_SUCCESS;
}

bool finSyntaxNode::isStatementLevelType(finSyntaxNodeType type)
{
    switch ( type ) {
      case FIN_SN_TYPE_DECLARE:
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
    const finLexNode *cmdlex = this->getCommandLexNode();

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

int finSyntaxNode::findLabelIdx(const QString &labelname)
{
    for ( int i = 0; i < this->getSubListCount(); i++ ) {
        finSyntaxNode *subnode = this->getSubSyntaxNode(i);
        if ( subnode->getType() != finSyntaxNode::FIN_SN_TYPE_LABEL )
            continue;

        finLexNode *lexnode = subnode->getCommandLexNode();
        if ( QString::compare(lexnode->getString(), labelname) == 0 )
            return i;
    }
    return -1;
}

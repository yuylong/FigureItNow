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
    this->_type = TP_DUMMY;
}

finSyntaxNode::~finSyntaxNode()
{
    this->disposeAll();
}

finErrorCode finSyntaxNode::copyNode(const finSyntaxNode *srcnode)
{
    finErrorCode errcode;
    this->disposeAll();

    if ( srcnode == nullptr )
        return finErrorKits::EC_SUCCESS;

    this->_type = srcnode->getType();
    this->_cmdLexNode.copyNode(srcnode->getCommandLexNode());

    for ( int i = 0; i < srcnode->getSubListCount(); i++ ) {
        finSyntaxNode *synnode = new finSyntaxNode();
        if ( synnode == nullptr )
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
        return nullptr;
    else
        return &this->_cmdLexNode;
}

finLexNode *finSyntaxNode::getCommandLexNode()
{
    if ( this->_cmdLexNode.getType() == finLexNode::TP_DUMMY )
        return nullptr;
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

    if ( type == TP_DUMMY || type == TP_MAX )
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
        return nullptr;

    finSyntaxNode *retnode = this->_subSyntaxList.at(idx);
    this->_subSyntaxList.removeAt(idx);
    return retnode;
}

bool finSyntaxNode::isExpressLevelType(finSyntaxNodeType type)
{
    switch ( type ) {
      case TP_EXPRESS:
      case TP_SINGLE:
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
      case TP_DECLARE:
      case TP_BRANCH:
      case TP_JUMP:
      case TP_LABEL:
      case TP_LOOP:
      case TP_STATEMENT:
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

    printf("* type=%d, ", static_cast<int>(this->getType()));
    if ( cmdlex != nullptr ) {
        printf("lex=%d, str=%s, ", static_cast<int>(cmdlex->getType()), cmdlex->getString().toLatin1().data());
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
        if ( subnode->getType() != finSyntaxNode::TP_LABEL )
            continue;

        finLexNode *lexnode = subnode->getCommandLexNode();
        if ( QString::compare(lexnode->getString(), labelname) == 0 )
            return i;
    }
    return -1;
}

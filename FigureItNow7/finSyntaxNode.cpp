/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxNode.h"

#include <QtGlobal>

finSyntaxNode::finSyntaxNode()
    : _type(TP_DUMMY), _cmdLexNode(), _subSyntaxList()
{ /* Do nothing */ }

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

        this->appendSubSyntaxNode(synnode);
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

void finSyntaxNode::setType(finSyntaxNodeType type)
{
    this->_type = type;
}

void finSyntaxNode::setCommandLexNode(const finLexNode *lexnode)
{
    Q_ASSERT(lexnode != nullptr);
    this->_cmdLexNode.copyNode(lexnode);
}

void finSyntaxNode::appendSubSyntaxNode(finSyntaxNode *synnode)
{
    Q_ASSERT(synnode != nullptr);
    this->_subSyntaxList.append(synnode);
}

void finSyntaxNode::prependSubSyntaxNode(finSyntaxNode *synnode)
{
    Q_ASSERT(synnode != nullptr);
    this->_subSyntaxList.prepend(synnode);
}

finSyntaxNode *finSyntaxNode::pickSubSyntaxNode(int idx)
{
    if ( idx < 0 || idx >= this->_subSyntaxList.count() ) {
        finThrow(finErrorCode::EC_INVALID_PARAM,
                 QString("Idx-%1 out of range (%2).").arg(idx).arg(this->_subSyntaxList.count()));
    }

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

void finSyntaxNode::disposeCommandLexNode()
{
    this->_cmdLexNode.setType(finLexNode::TP_DUMMY);
}

void finSyntaxNode::disposeSubSyntaxNodes()
{
    if ( this->_subSyntaxList.count() == 0 )
        return;

    while ( this->_subSyntaxList.count() > 0 ) {
        finSyntaxNode *subtk = this->_subSyntaxList.at(0);
        this->_subSyntaxList.removeFirst();
        delete subtk;
    }
}

void finSyntaxNode::disposeAll()
{
    this->disposeCommandLexNode();
    this->disposeSubSyntaxNodes();
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
        Q_ASSERT(this->getSubSyntaxNode(i) != nullptr);
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

/*! \file finSyntaxNode.h
 *  \brief The declarations of the Syntax Node.
 *
 * This file defines the data structure used by a syntax node, which is a node in the syntax tree. A syntax tree is
 * used as a compiled result of the FIN-7 figuring script. which will lead a faster running and computing the points
 * drawing on the panel with a lower overhead.
 */
/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20160301    0  Yulong Yu    Create this file.
 * 20160530    1  Yulong Yu    Add the doxygen-styled comments.
 * 20161223    2  Yulong Yu    Refine the comment styles and the copyright license.
 */

#ifndef FINSYNTAXNODE_H
#define FINSYNTAXNODE_H

#include <QList>

#include "finLexNode.h"

/*! \class finSyntaxNode
 *  \brief The finSyntaxNode class
 */
class finSyntaxNode
{
public:
    enum finSyntaxNodeType {
        FIN_SN_TYPE_DUMMY = 0,
        FIN_SN_TYPE_SINGLE,
        FIN_SN_TYPE_DECLARE,
        FIN_SN_TYPE_STATEMENT,
        FIN_SN_TYPE_EXPRESS,
        FIN_SN_TYPE_FUNCTION,
        FIN_SN_TYPE_BRANCH,
        FIN_SN_TYPE_LOOP,
        FIN_SN_TYPE_LABEL,
        FIN_SN_TYPE_JUMP,
        FIN_SN_TYPE_PROGRAM,
        FIN_SN_TYPE_MAX
    };

protected:
    finSyntaxNodeType _type;
    finLexNode *_cmdLexNode;
    QList<finSyntaxNode *> _subSyntaxList;

public:
    finSyntaxNode();

    finSyntaxNodeType getType() const;
    finLexNode *getCommandLexNode() const;
    int getSubListCount() const;
    finSyntaxNode *getSubSyntaxNode(int idx) const;

    void dump() const;

    finErrorCode setType(finSyntaxNodeType type);
    finErrorCode setCommandLexNode(finLexNode *lexnode);
    finErrorCode appendSubSyntaxNode(finSyntaxNode *synnode);
    finErrorCode prependSubSyntaxNode(finSyntaxNode *synnode);

    finErrorCode disposeCommandLexNode();
    finErrorCode disposeSubSyntaxNodes();
    finErrorCode disposeAll();

    static bool isExpressLevelType(finSyntaxNodeType type);
    static bool isStatementLevelType(finSyntaxNodeType type);

private:
    void dumpLeveled(int level) const;

};

typedef finSyntaxNode::finSyntaxNodeType finSyntaxNodeType;

#endif // FINSYNTAXNODE_H

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finSyntaxNode.h
 *  \brief The declarations of the Syntax Node.
 *
 * This file defines the data structure used by a syntax node, which is a node in the syntax tree. A syntax tree is
 * used as a compiled result of the FIN-7 figuring script. which will lead a faster running and computing the points
 * drawing on the panel with a lower overhead.
 */

#ifndef FINSYNTAXNODE_H
#define FINSYNTAXNODE_H

#include <QList>

#include "finErrorCode.h"
#include "finLexNode.h"

/*! \class finSyntaxNode
 *  \brief The finSyntaxNode class
 */
class finSyntaxNode : finExceptionObject
{
public:
    enum Type {
        TP_DUMMY = 0,
        TP_SINGLE,
        TP_DECLARE,
        TP_STATEMENT,
        TP_EXPRESS,
        TP_FUNCTION,
        TP_BRANCH,
        TP_LOOP,
        TP_LABEL,
        TP_JUMP,
        TP_PROGRAM,
        TP_MAX
    };

protected:
    Type _type;
    finLexNode _cmdLexNode;
    QList<finSyntaxNode *> _subSyntaxList;

public:
    finSyntaxNode();
    virtual ~finSyntaxNode();

    finErrorCode copyNode(const finSyntaxNode *srcnode);

    Type getType() const;
    const finLexNode *getCommandLexNode() const;
    finLexNode *getCommandLexNode();
    int getSubListCount() const;
    finSyntaxNode *getSubSyntaxNode(int idx) const;

    void dump() const;
    virtual QString dumpObjInfo() const;

    void setType(Type type);
    void setCommandLexNode(const finLexNode *lexnode);
    void appendSubSyntaxNode(finSyntaxNode *synnode);
    void prependSubSyntaxNode(finSyntaxNode *synnode);
    finSyntaxNode *pickSubSyntaxNode(int idx);

    void disposeCommandLexNode();
    void disposeSubSyntaxNodes();
    void disposeAll();

    static bool isExpressLevelType(Type type);
    static bool isStatementLevelType(Type type);

    int findLabelIdx(const QString &labelname);

private:
    void dumpLeveled(int level) const;

};

typedef finSyntaxNode::Type finSyntaxNodeType;

#endif // FINSYNTAXNODE_H

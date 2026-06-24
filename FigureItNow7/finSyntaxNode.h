/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finSyntaxNode.h
 *  \brief Declarations of the syntax tree node and its Type enumeration.
 *
 *  This header defines finSyntaxNode, the building block of the syntax tree produced by
 *  finSyntaxReader and walked by finExecMachine, plus the Type enumeration used to classify
 *  each node and the convenience typedef finSyntaxNodeType.
 */

#ifndef FINSYNTAXNODE_H
#define FINSYNTAXNODE_H

#include <QList>

#include "finErrorCode.h"
#include "finLexNode.h"

/*! \class finSyntaxNode : public finExceptionObject
 *  \brief A single node in the FIN-7 syntax tree.
 *
 *  finSyntaxNode is the building block of the tree produced by finSyntaxReader and walked by
 *  finExecMachine. A node carries:
 *  - a top-level type (Type enum) saying what kind of construct it is (expression, statement,
 *    function definition, branch, loop, label, jump, program, ...);
 *  - an optional head lex node (`getCommandLexNode()`) holding the "command" token the node is
 *    built around (operator name, keyword, identifier, ...);
 *  - a list of child syntax nodes (the sub-syntax of this node).
 *
 *  The class itself is a single concrete C++ type; the polymorphism lives in how the parser,
 *  executor, and AST peephole pass treat each Type value. Children are owned by the parent
 *  and are freed when the parent is destroyed or when one of the dispose*() methods is called.
 *
 *  \see finSyntaxReader
 *  \see finExecMachine
 *  \see finExceptionObject
 */
class finSyntaxNode : public finExceptionObject
{
public:
    /*! \enum finSyntaxNode::Type
     *  \brief Top-level node types in the FIN-7 syntax tree.
     *
     *  Each value identifies what kind of construct the node represents. The category drives:
     *  - the executor (finExecMachine) — how to walk the node;
     *  - the AST peephole pass (finSyntaxOptimzer) — which simplifications apply;
     *  - the static helpers isExpressLevelType() and isStatementLevelType() — the
     *    "produces a value" vs "does not" classification.
     *
     *  \see finSyntaxNode::isExpressLevelType
     *  \see finSyntaxNode::isStatementLevelType
     */
    enum Type {
        TP_DUMMY     = 0,   //!< Reserved sentinel; not a real node type.

        TP_SINGLE,          //!< A single value (literal, identifier, parenthesised expression).
        TP_DECLARE,         //!< A variable declaration (`var` / assignment).
        TP_STATEMENT,       //!< A general statement (function call, expression statement).
        TP_EXPRESS,         //!< A binary / unary / postfix expression that produces a value.
        TP_FUNCTION,        //!< A function definition (the head holds the function name).
        TP_BRANCH,          //!< An if / elif / else branch (the head holds the condition for if/elif).
        TP_LOOP,            //!< A for / while loop (the head holds the loop condition).
        TP_LABEL,           //!< A goto label (the head holds the label name).
        TP_JUMP,            //!< A jump (goto / break / continue / return / exit; the head holds the kind).
        TP_PROGRAM,         //!< The top-level program; the root of the syntax tree.

        TP_MAX              //!< Sentinel marking the end of the enum.
    };

protected:
    Type _type;                          //!< The node's top-level type.
    finLexNode _cmdLexNode;              //!< The head lex node (the "command" this node is built around).
    QList<finSyntaxNode *> _subSyntaxList;   //!< Owned children; freed by disposeSubSyntaxNodes() or disposeAll().

public:
    /*!
     *  \brief Default-constructs an empty TP_DUMMY node with no children.
     */
    finSyntaxNode();

    /*!
     *  \brief Destructs this node and all of its children.
     */
    virtual ~finSyntaxNode();

    /*!
     *  \brief Deep-copies the source node's type, head lex node, and entire sub-tree.
     *
     *  Existing children of this node are disposed first. The copy is recursive: each child is
     *  allocated via `new finSyntaxNode()` and is then itself copyNode()'d from the matching
     *  source child.
     *
     *  \param srcnode  The source node to copy from.
     *  \throws finException with EC_NULL_POINTER if \a srcnode is \c nullptr.
     *  \throws finException with EC_OUT_OF_MEMORY if a child allocation fails.
     */
    void copyNode(const finSyntaxNode *srcnode);

    /*!
     *  \brief Returns the node's top-level type.
     */
    Type getType() const;

    /*!
     *  \brief Returns the head lex node, or \c nullptr if the head is unset (TP_DUMMY).
     */
    const finLexNode *getCommandLexNode() const;

    /*!
     *  \brief Returns the head lex node, or \c nullptr if the head is unset (TP_DUMMY).
     */
    finLexNode *getCommandLexNode();

    /*!
     *  \brief Returns the number of child nodes.
     */
    int getSubListCount() const;

    /*!
     *  \brief Returns the child node at \a idx.
     *
     *  No bounds check; the caller is responsible for staying within [0, getSubListCount()).
     */
    finSyntaxNode *getSubSyntaxNode(int idx) const;

    /*!
     *  \brief Prints the node and its sub-tree to stdout in a human-readable indented form.
     *
     *  Output goes to stdout, not the finDebug stream. Intended for ad-hoc inspection rather
     *  than for production logging.
     */
    void dump() const;

    /*!
     *  \brief Returns a one-line textual description of the node, used for logging.
     *
     *  Includes the type, the head lex node's dumpObjInfo() (if any), and the child count.
     */
    virtual QString dumpObjInfo() const override;

    /*!
     *  \brief Sets the node's top-level type.
     */
    void setType(Type type);

    /*!
     *  \brief Deep-copies \a lexnode into the head lex node slot.
     */
    void setCommandLexNode(const finLexNode *lexnode);

    /*!
     *  \brief Appends \a synnode to the end of the child list.
     *
     *  The parent takes ownership of \a synnode and will delete it on destruction or on
     *  disposeSubSyntaxNodes(). The pointer must not be \c nullptr.
     */
    void appendSubSyntaxNode(finSyntaxNode *synnode);

    /*!
     *  \brief Prepends \a synnode to the start of the child list.
     *
     *  The parent takes ownership of \a synnode and will delete it on destruction or on
     *  disposeSubSyntaxNodes(). The pointer must not be \c nullptr.
     */
    void prependSubSyntaxNode(finSyntaxNode *synnode);

    /*!
     *  \brief Removes and returns the child at \a idx, leaving the child un-owned.
     *
     *  The caller becomes responsible for the returned pointer (typically to attach it
     *  elsewhere, or to delete it).
     *
     *  \throws finException with EC_INVALID_PARAM if \a idx is out of range.
     */
    finSyntaxNode *pickSubSyntaxNode(int idx);

    /*!
     *  \brief Resets the head lex node to an empty TP_DUMMY state.
     */
    void disposeCommandLexNode();

    /*!
     *  \brief Deletes every child node and clears the child list.
     */
    void disposeSubSyntaxNodes();

    /*!
     *  \brief Calls disposeCommandLexNode() followed by disposeSubSyntaxNodes().
     */
    void disposeAll();

    /*!
     *  \brief Returns \c true if \a type is an expression-level node (produces a value).
     *
     *  Currently matches TP_EXPRESS and TP_SINGLE.
     */
    static bool isExpressLevelType(Type type);

    /*!
     *  \brief Returns \c true if \a type is a statement-level node (does not produce a value).
     *
     *  Currently matches TP_DECLARE, TP_BRANCH, TP_JUMP, TP_LABEL, TP_LOOP, and TP_STATEMENT.
     */
    static bool isStatementLevelType(Type type);

    /*!
     *  \brief Searches the immediate children for a TP_LABEL whose text matches \a labelname.
     *
     *  Only the immediate children are searched; nested labels are not followed. The text
     *  comparison is a QString::compare(), i.e. case-sensitive equality.
     *
     *  \return The index of the matching child, or -1 if no match is found.
     */
    int findLabelIdx(const QString &labelname);

private:
    /*!
     *  \brief Recursive helper for dump() that prints this node and its children indented
     *         \a level levels deep.
     */
    void dumpLeveled(int level) const;
};

/*! \typedef finSyntaxNodeType
 *  \brief Shorthand alias for finSyntaxNode::Type.
 */
typedef finSyntaxNode::Type finSyntaxNodeType;

#endif // FINSYNTAXNODE_H

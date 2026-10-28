/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXTREE_H
#define FINSYNTAXTREE_H

#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finSyntaxNode.h"

/*! \file finSyntaxTree.h
 *  \brief Declaration of the finSyntaxTree container.
 *
 *  This header defines finSyntaxTree, the container that owns the syntax tree produced by
 *  finSyntaxReader: a single TP_PROGRAM root node plus the source script text, kept line by
 *  line for diagnostics and pretty-printing.
 */

/*! \class finSyntaxTree : public finExceptionObject
 *  \brief The container that owns the complete syntax tree of a FIN script.
 *
 *  finSyntaxTree holds two things:
 *  - the root node of the syntax tree, always of type finSyntaxNode::TP_PROGRAM, whose children
 *    are the top-level statements of the script;
 *  - the source script text, split into lines, so that diagnostics and dumps can refer back to
 *    the original code (see getCodeLine() and dumpObjInfo()).
 *
 *  Nodes appended to the tree are deep-copied: the tree allocates its own finSyntaxNode for
 *  every appendSyntaxNode() / prependSyntaxNode() call and owns the copy; the caller's node is
 *  never referenced afterwards. All owned children are freed by disposeAll(), which the
 *  destructor invokes.
 *
 *  Being a finExceptionObject, the tree can also be attached to a finException and format
 *  itself via dumpObjInfo().
 *
 *  \see finSyntaxNode
 *  \see finSyntaxReader
 *  \see finExceptionObject
 */
class finSyntaxTree : public finExceptionObject
{
protected:
    finSyntaxNode _rootNode;          //!< The TP_PROGRAM root; its children are the script statements.
    QStringList _scriptCodes;         //!< The source script, split into one entry per line.

public:
    /*!
     *  \brief Constructs an empty syntax tree with a fresh TP_PROGRAM root and no script code.
     */
    finSyntaxTree();

    /*!
     *  \brief Destructs the tree and disposes all owned syntax nodes and script lines.
     */
    virtual ~finSyntaxTree();

    /*!
     *  \brief Returns a read-only pointer to the root node.
     */
    const finSyntaxNode *getRootNode() const;

    /*!
     *  \brief Returns a mutable pointer to the root node.
     */
    finSyntaxNode *getRootNode();

    /*!
     *  \brief Reassembles and returns the stored source script as a single string.
     *
     *  Lines are joined with line separators, each entry followed by one, so the result always
     *  ends with a newline when the script is non-empty.
     */
    QString getScriptCode() const;

    /*!
     *  \brief Returns the source line with the given 0-based index.
     *
     *  \param line  The 0-based line index; must be within [0, line count).
     *  \return The requested line, or an empty string when \a line is out of range.
     */
    QString getCodeLine(int line) const;

    /*!
     *  \brief Replaces the whole tree with a deep copy of the given root node.
     *
     *  The node is dumped for debugging and then deep-copied into this tree; the copy replaces
     *  the previous root content.
     *
     *  \param rtnode  The node to copy; must be non-null and of type TP_PROGRAM.
     *  \throws finException with EC_INVALID_PARAM if \a rtnode is not of type TP_PROGRAM.
     */
    void setRootNode(const finSyntaxNode *rtnode);

    /*!
     *  \brief Deep-copies \a synnode and appends the copy as a new child of the root node.
     *
     *  \param synnode  The node to copy; must be non-null.
     *  \throws finException with EC_OUT_OF_MEMORY if the copy allocation fails.
     */
    void appendSyntaxNode(const finSyntaxNode *synnode);

    /*!
     *  \brief Deep-copies \a synnode and prepends the copy as the first child of the root node.
     *
     *  \param synnode  The node to copy; must be non-null.
     *  \throws finException with EC_OUT_OF_MEMORY if the copy allocation fails.
     */
    void prependSyntaxNode(const finSyntaxNode *synnode);

    /*!
     *  \brief Appends deep copies of all nodes in \a list, in list order.
     *
     *  Null entries in the list are skipped with a warning.
     *
     *  \param list  The list of nodes to copy; must be non-null.
     *  \throws finException with EC_OUT_OF_MEMORY if a copy allocation fails.
     */
    void appendSyntaxNodeList(const QList<finSyntaxNode *> *list);

    /*!
     *  \brief Appends deep copies of all nodes in \a list in reverse order, from last to first.
     *
     *  This matches popping a parser's node stack: the first node appended is the last element
     *  of \a list. Null entries in the list are skipped with a warning.
     *
     *  \param list  The list of nodes to copy; must be non-null.
     *  \throws finException with EC_OUT_OF_MEMORY if a copy allocation fails.
     */
    void appendSyntaxNodeStack(const QList<finSyntaxNode *> *list);

    /*!
     *  \brief Not implemented yet; always throws.
     *
     *  \throws finException with EC_NON_IMPLEMENT.
     */
    void clearSyntaxNodes();

    /*!
     *  \brief Stores the script text, split into lines with empty lines kept.
     */
    void setScriptCode(const QString &script);

    /*!
     *  \brief Formats the tree for a logged finException.
     *
     *  \return A string holding the root node's dump and a summary of the stored script
     *          (line count plus the first line).
     */
    virtual QString dumpObjInfo() const override;

    /*!
     *  \brief Disposes all owned syntax nodes and clears the stored script.
     *
     *  The root node is reset to an empty TP_PROGRAM node, ready to be filled again.
     */
    void disposeAll();
};

#endif // FINSYNTAXTREE_H

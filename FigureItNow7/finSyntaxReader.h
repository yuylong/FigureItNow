/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finSyntaxReader.h
 *  \brief Declarations of the recursive-descent parser that turns lex nodes into a syntax tree.
 *
 *  This header defines finSyntaxReader, the streaming parser that consumes a FIN-7 script
 *  token-by-token (via an embedded finLexReader) and emits a finSyntaxTree. The parser also
 *  accumulates per-token syntax errors into a list that travels with the resulting tree, so
 *  the executor can report them alongside the successfully parsed code.
 */

#ifndef FINSYNTAXREADER_H
#define FINSYNTAXREADER_H

#include <QList>
#include <QString>

#include "finErrorCode.h"
#include "finLexReader.h"
#include "finSyntaxNode.h"
#include "finSyntaxError.h"
#include "finSyntaxTree.h"


/*! \class finSyntaxReader : public finExceptionObject
 *  \brief Recursive-descent parser that turns a stream of lex nodes into a syntax tree.
 *
 *  finSyntaxReader walks the token stream produced by an embedded finLexReader and uses a
 *  single syntax stack (the newest node is at index 0) to build a finSyntaxTree whose root
 *  is a TP_PROGRAM node. The reader is driven one token at a time via readNextToken(); a
 *  typical pipeline calls startRead(), readNextToken() in a loop until it returns \c false,
 *  and then getSyntaxTree() to harvest the result.
 *
 *  Parsing is split into three dispatch paths based on the token's lex type: value-like
 *  tokens (keyword / variable / decimal / string) are pushed as a TP_EXPRESS or TP_SINGLE
 *  node; operator tokens are routed to the arithmetic / bracket / splitter handlers (which
 *  implement operator precedence, bracket matching, and statement / declaration framing
 *  respectively); virtual tokens (TP_DUMMY, TP_NOTE, ...) are silently dropped.
 *
 *  Errors detected during parsing are recorded as finSyntaxError entries in the internal
 *  error list, which is moved into the syntax tree handed to the executor; the reader itself
 *  only throws on internal / state-machine misuse, never on a script-level parse error.
 *
 *  The reader owns the syntax stack nodes it allocates and the lex reader it embeds; the
 *  caller owns the finSyntaxTree returned by getSyntaxTree().
 *
 *  \see finLexReader
 *  \see finSyntaxNode
 *  \see finSyntaxTree
 *  \see finSyntaxError
 *  \see finExceptionObject
 */
class finSyntaxReader : public finExceptionObject
{
public:
    /*! \enum finSyntaxReader::State
     *  \brief Lifecycle state of the parser.
     *
     *  Transitions: ST_DUMMY -> ST_READY (via setScriptCode()), ST_READY -> ST_READING
     *  (via startRead()), ST_READING -> ST_DONE (when readNextToken() exhausts the input
     *  or records a lex error), and ST_READING / ST_DONE -> ST_READY (via stopRead()).
     *  isReading() is \c true for both ST_READING and ST_DONE so the caller can still
     *  query the syntax tree after the last token has been consumed.
     */
    enum State {
        ST_DUMMY,    //!< Reserved sentinel; the parser has never had a script loaded.
        ST_READY,    //!< A script is loaded; readNextToken() is not currently in progress.
        ST_READING,  //!< A read pass is in progress; readNextToken() may be called.
        ST_DONE,     //!< The read pass has finished (input exhausted or a lex error was recorded).
    };

protected:
    finLexReader _lexReader;             //!< The streaming tokenizer that feeds the parser; owned by this reader.
    State _state;                        //!< Current parser state (see State).
    QList<finSyntaxNode *> _syntaxStack; //!< Parser working stack; the newest node is at index 0.
                                         //!< Every node in this list is heap-allocated and owned by the reader.
    QList<finSyntaxError> _errList;      //!< Parse errors recorded so far; attached to the syntax tree.

public:
    /*!
     *  \brief Constructs an empty parser with no script loaded, in the ST_DUMMY state.
     */
    finSyntaxReader();

    /*!
     *  \brief Returns a pointer to the embedded lex reader. The parser owns the lex reader.
     */
    const finLexReader *getLexReader() const;

    /*!
     *  \brief Returns a pointer to the embedded lex reader. The parser owns the lex reader.
     */
    finLexReader *getLexReader();

    /*!
     *  \brief Returns the script currently loaded into the embedded lex reader, or an
     *         empty string if no script is loaded.
     */
    QString getScriptCode() const;

    /*!
     *  \brief Loads \a scriptcode into the embedded lex reader and transitions the parser
     *         from ST_DUMMY to ST_READY.
     *
     *  Any prior parse state is discarded: the lex reader is rewound to the start, and
     *  the parser's syntax stack and error list are cleared.
     *
     *  \throws finException with EC_STATE_ERROR if a parse is already in progress (state
     *          ST_READING or ST_DONE).
     */
    void setScriptCode(const QString &scriptcode);

    /*!
     *  \brief Returns \c true if the parser is in the ST_READING or ST_DONE state.
     */
    bool isReading() const;

    /*!
     *  \brief Returns the parser's current state (see State).
     */
    State getState() const;

    /*!
     *  \brief Transitions the parser from ST_READY to ST_READING and clears any leftover
     *         stack nodes and error entries.
     *
     *  After this call, readNextToken() can be invoked repeatedly to drive the parser.
     *
     *  \throws finException with EC_STATE_ERROR if the parser is not in the ST_READY state.
     */
    void startRead();

    /*!
     *  \brief Transitions the parser from ST_READING / ST_DONE back to ST_READY and
     *         discards the current syntax stack and error list.
     *
     *  \throws finException with EC_STATE_ERROR if the parser is in the ST_DUMMY state.
     *  \throws finException with EC_DUPLICATE_OP if the parser is already stopped (ST_READY).
     */
    void stopRead();

    /*!
     *  \brief Reads the next lex node and folds it into the syntax stack.
     *
     *  A single call advances the embedded lex reader by one token, dispatches the token
     *  to the matching parser handler, and returns. TP_DUMMY tokens are silently dropped
     *  (the call still returns \c true so the caller keeps pulling).
     *
     *  \return \c true if more tokens are still expected (or the call was a no-op);
     *          \c false if the input is exhausted or a lex error was recorded. In the
     *          latter case the parser transitions to ST_DONE and getSyntaxTree() will
     *          return a tree that carries the recorded error.
     *
     *  \throws finException with EC_STATE_ERROR if the parser is not in the ST_READING state.
     */
    bool readNextToken();

    /*!
     *  \brief Packages the current syntax stack, script code, and error list into a
     *         freshly allocated finSyntaxTree and returns it to the caller.
     *
     *  The returned tree owns the syntax stack nodes (they are moved out of the reader),
     *  a copy of the script code, and a copy of the error list. The reader itself is left
     *  empty and still in the READING / DONE state, so a subsequent call will return
     *  \c nullptr until startRead() is called again.
     *
     *  \return Pointer to a newly allocated finSyntaxTree, or \c nullptr if the parser is
     *          not in the READING / DONE state or if allocation failed. The caller is
     *          responsible for deleting the returned tree.
     */
    finSyntaxTree *getSyntaxTree();

    /*!
     *  \brief Returns a one-line textual description of the reader, used for logging.
     *
     *  Includes the current state, the syntax stack depth, the error count, and the
     *  embedded lex reader's own dumpObjInfo().
     */
    virtual QString dumpObjInfo() const override;

private:
    /*!
     *  \brief Deletes every node in the syntax stack and clears the error list.
     *
     *  Used internally by startRead() / stopRead() to reset the parser between passes.
     */
    void disposeAllRead();

    /*!
     *  \brief Dispatches a freshly lexed node to the matching token handler.
     *
     *  Keyword / variable / decimal / string tokens are routed to processInstanceToken();
     *  operator tokens to processOperatorToken(); everything else (TP_DUMMY, TP_NOTE, ...)
     *  to processVirtualToken().
     */
    void processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype);

    /*!
     *  \brief Wraps a value-like token in a syntax node and pushes it onto the syntax stack.
     *
     *  Variables, decimals, and strings are wrapped as TP_EXPRESS (the node produces a
     *  value); keywords are wrapped as TP_SINGLE (the node is a command / marker).
     */
    void processInstanceToken(finLexNode *lexnode);

    /*!
     *  \brief Discards a non-substantive token (TP_DUMMY, TP_NOTE, ...).
     *
     *  \throws finException with EC_NULL_POINTER if \a lexnode is \c nullptr.
     */
    void processVirtualToken(finLexNode *lexnode);

    /*!
     *  \brief Routes an operator token to processArithOperator(), processBracket(), or
     *         processSplitter() based on the operator's category.
     */
    void processOperatorToken(finLexNode *lexnode);

    /*!
     *  \brief Returns the number of before-parameter operands the operator expects, or
     *         \c -1 if \a lexnode is not a known arithmetic operator.
     *
     *  The result is read from the static operator table. \c 0 means the operator is in
     *  unary-prefix form (e.g. `-x`, `!x`); \c 1 means it is in infix or postfix form
     *  (e.g. `a + b`, `i++`).
     */
    static int getOperatorBfParamCnt(finLexNode *lexnode);

    /*!
     *  \brief Returns the number of after-parameter operands the operator expects, or
     *         \c -1 if \a lexnode is not a known arithmetic operator.
     */
    static int getOperatorAfParamCnt(finLexNode *lexnode);

    /*!
     *  \brief Rewrites \a lexnode's operator to the matching non-before-parameter form.
     *
     *  Used when a binary operator appears in a context where there is no before-parameter
     *  expression, so it must be reinterpreted as a unary-prefix operator (e.g. `-`
     *  becomes OP_NEGATIVE, `+` becomes OP_POSITIVE).
     *
     *  \throws finException with EC_READ_ERROR if \a lexnode is not an operator, if the
     *          operator is not in the table, or if the operator has no non-before-parameter
     *          counterpart.
     */
    static void transformOpToNonBfParamOp(finLexNode *lexnode);

    /*!
     *  \brief Returns the operator's precedence level (higher binds tighter), or \c 0 if
     *         \a lexnode is not a known arithmetic operator.
     */
    static int getOperatorPriority(finLexNode *lexnode);

    /*!
     *  \brief Returns the operator's associativity as a mesh direction.
     *
     *  \return \c FIN_MD_L2R for left-to-right associativity, \c FIN_MD_R2L for
     *          right-to-left associativity. Defaults to \c FIN_MD_L2R if \a lexnode
     *          is not a known operator.
     */
    static int getOperatorMeshDir(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode is a known arithmetic operator (one that
     *         has both a precedence and a parameter count in the operator table).
     */
    static bool isArithOperator(finLexNode *lexnode);

    /*!
     *  \brief Compares two operators by precedence, using associativity as the tiebreaker.
     *
     *  \return A negative number if \a lexnode1 binds less tightly than \a lexnode2; a
     *          positive number if it binds more tightly. The result is never zero, since
     *          the tiebreaker always forces a non-zero return.
     */
    static int compareOperators(finLexNode *lexnode1, finLexNode *lexnode2);

    /*!
     *  \brief Pushes an arithmetic operator onto the syntax stack, first draining any
     *         adjacent operator / operand nodes that bind more tightly.
     *
     *  If the operator has a non-zero before-parameter count but the previous token on
     *  the stack is not an expression, the operator is rewritten to its non-before-
     *  parameter form (see transformOpToNonBfParamOp()).
     */
    void processArithOperator(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode is a left bracket (round, square, or flower).
     */
    static bool isLeftBracket(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode is a right bracket (round, square, or flower).
     */
    static bool isRightBracket(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode is any kind of bracket (left or right).
     */
    static bool isBracket(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode1 and \a lexnode2 form a matching bracket pair
     *         (one left and the same kind of right).
     *
     *  The two arguments may be in either order; the helper detects which one is on the
     *  left.
     */
    static bool isCorrespnBracket(finLexNode *lexnode1, finLexNode *lexnode2);

    /*!
     *  \brief Pushes a left bracket onto the syntax stack as a TP_SINGLE node. The actual
     *         bracket pairing is handled later, when the matching right bracket arrives.
     */
    void processLeftBracket(finLexNode *lexnode);

    /*!
     *  \brief Closes a bracket: drains pending arithmetic, meshes commas, packages the
     *         bracketed sub-tree as a single TP_EXPRESS node, then dispatches to the
     *         flower / round / square bracket handler as appropriate.
     *
     *  \throws finException with EC_READ_ERROR if the matching left bracket is missing
     *          or if the right bracket does not match the kind of left bracket on the
     *          stack.
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void processRightBracket(finLexNode *lexnode);

    /*!
     *  \brief Dispatches a bracket token to processLeftBracket() or processRightBracket().
     */
    void processBracket(finLexNode *lexnode);

    /*!
     *  \brief Returns \c true if \a lexnode is a statement-level splitter (semicolon,
     *         comma, or colon).
     */
    static bool isSplitter(finLexNode *lexnode);

    /*!
     *  \brief Handles a statement-level splitter.
     *
     *  Semicolons terminate the current statement; colons convert a preceding variable
     *  expression into a TP_LABEL; commas are bookkeeping markers and are left on the
     *  stack for meshAllCommas() to consume later.
     *
     *  \throws finException with EC_READ_ERROR if a colon is not preceded by a variable
     *          expression.
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void processSplitter(finLexNode *lexnode);

    /*!
     *  \brief Allocates a TP_SINGLE or TP_EXPRESS syntax node, attaches \a lexnode to it
     *         as the head, and prepends it to the syntax stack.
     *
     *  Ownership of \a lexnode is transferred to the new node.
     *
     *  \param lexnode  The token to wrap.
     *  \param tktype   The syntax node type to assign (typically TP_SINGLE or TP_EXPRESS).
     *
     *  \throws finException with EC_OUT_OF_MEMORY if the node allocation fails.
     */
    void pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype);

    /*!
     *  \brief Drains one binary / unary arithmetic expression off the top of the stack
     *         and folds it into a single TP_EXPRESS node.
     *
     *  \return \c true if an expression was successfully meshed; \c false if the top of
     *          the stack is not in a state where an arithmetic drain is possible (e.g.
     *          only one token, the top is a statement, the operator is a non-arithmetic
     *          token, or the top is a left bracket).
     */
    bool meshArithExpress();

    /*!
     *  \brief Repeatedly calls meshArithExpress() until it can no longer drain anything.
     *
     *  A failed drain inside the loop is treated as "no more to mesh" and stops the loop;
     *  the only exception that escapes is an EC_OUT_OF_MEMORY, which propagates because
     *  retrying will not help.
     */
    void meshAllArithExpress();

    /*!
     *  \brief Allocates a TP_EXPRESS node whose head lex node is TP_DUMMY, used as a
     *         placeholder for an omitted operand.
     *
     *  \return The new dummy expression, or \c nullptr if allocation failed.
     */
    static finSyntaxNode *createDummyExpress();

    /*!
     *  \brief Folds a top-level comma-separated list (e.g. function arguments) into a
     *         single TP_EXPRESS node whose head is the comma operator.
     *
     *  Missing operands are filled in with a dummy expression (see createDummyExpress()).
     *  The function is a no-op if no comma is present on the stack.
     *
     *  \throws finException with EC_READ_ERROR if two operands are seen with no comma
     *          between them.
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void meshAllCommas();

    /*!
     *  \brief Looks at the top two stack entries and folds them into a keyword-shaped
     *         construct (if / elif / else, for / while, goto, return / exit / break /
     *         continue, var).
     *
     *  The caller is responsible for ensuring the top of the stack already contains the
     *  right material (e.g. a keyword + a sub-tree, a keyword + a statement, ...). If the
     *  top two entries do not match any recognised keyword pattern, the function is a
     *  no-op.
     *
     *  \throws finException with EC_READ_ERROR on a structural mismatch (e.g. an `if`
     *          head without a single sub-expression, a `for` head without three
     *          sub-nodes, a `goto` whose target is not a variable).
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void meshStatementWithKeywords();

    /*!
     *  \brief Classifies a closed flower-bracket sub-tree as TP_STATEMENT or TP_EXPRESS
     *         based on the types of its children.
     *
     *  An empty flower bracket is treated as a TP_STATEMENT; otherwise the children's
     *  types must all be at the same level (either all statement-level or all
     *  expression-level). The chosen level becomes the flower-bracket node's type.
     *
     *  \throws finException with EC_READ_ERROR if the children are a mix of statement
     *          and expression levels, or if any child is at neither level.
     */
    void recogFlowerBracketType();

    /*!
     *  \brief Finishes a closed flower-bracket sub-tree and, if it directly follows a
     *         function head (`E(E)`), promotes the pair into a TP_FUNCTION node.
     *
     *  If the flower bracket is a statement block and the previous stack entry is a
     *  keyword that expects a body (if / elif / else / for / while), the body is attached
     *  via meshStatementWithKeywords().
     *
     *  \throws finException with EC_READ_ERROR if the flower bracket is the body of a
     *          function call but is not a statement block.
     */
    void meshFlowerBracket();

    /*!
     *  \brief Finishes a closed round-bracket sub-tree and attaches it to the previous
     *         stack entry.
     *
     *  - After an expression: produces a function-call node (head OP_FUNCTION) with the
     *    expression as callee and the bracket contents as arguments.
     *  - After an `if` / `elif` / `while` keyword: attaches the bracket as the head's
     *    single condition expression.
     *  - After a `for` keyword: expects the bracket to contain exactly three sub-nodes
     *    (init, condition, step) of the right kinds and attaches them to the head.
     *
     *  \throws finException with EC_READ_ERROR on a structural mismatch (wrong sub-node
     *          count, wrong sub-node type, missing enclosing keyword).
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void meshRoundBracket();

    /*!
     *  \brief Finishes a closed square-bracket sub-tree and, if it follows an expression,
     *         promotes the pair into an array-access node (head OP_ACCESS).
     *
     *  \throws finException with EC_READ_ERROR if the bracket does not contain exactly
     *          one expression sub-node, or if the previous stack entry is not an
     *          expression.
     *  \throws finException with EC_OUT_OF_MEMORY if a node allocation fails.
     */
    void meshSquareBracket();
};

#endif // FINSYNTAXREADER_H

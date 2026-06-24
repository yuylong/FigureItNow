/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finLexReader.h
 *  \brief Declarations of the FIN-7 streaming tokenizer.
 *
 *  This header defines finLexReader, the class that turns a FIN-7 source string into a sequence
 *  of finLexNode tokens, plus the private helpers used to walk the input and the inner
 *  finLexReaderOrder enum used to disambiguate overlapping token prefixes.
 */

#ifndef FINLEXREADER_H
#define FINLEXREADER_H

#include <QString>

#include "finErrorCode.h"
#include "finLexNode.h"


/*! \class finLexReader : public finExceptionObject
 *  \brief Streaming tokenizer for FIN-7 scripts.
 *
 *  finLexReader consumes a FIN-7 source string one token at a time. Each call to
 *  getNextLexNode() advances the read position and fills in a caller-supplied finLexNode with
 *  the next token's type, source text, payload, and source position. Whitespace (space, tab,
 *  newline) is skipped automatically.
 *
 *  Disambiguation between token types that share a leading character (notably TP_DECIMAL vs
 *  TP_OPERATOR for `+` and `-`) is driven by a small state machine: after a value the reader
 *  expects an operator next, after an operator it expects a value next, and the tiebreaker
 *  order is driven by finLexReaderOrder. TP_NOTE / TP_STRING / TP_VARIABLE are always probed
 *  first because their leading characters do not overlap with the value/operator set.
 *
 *  finLexReader is single-pass and non-reentrant; it does not buffer the input.
 *
 *  \see finLexNode
 *  \see finExceptionObject
 */
class finLexReader : public finExceptionObject
{
private:
    QString _inputStr;                  //!< The script currently being lexed.
    unsigned long _posIdx;              //!< 0-based read position (character index into _inputStr).
    unsigned int _curRow, _curCol;      //!< 0-based source row and column of the character at _posIdx.
    finLexNodeType _lastNodeType;       //!< Type of the most recently lexed token; kept for diagnostics.

public:
    /*!
     *  \brief Default-constructs an empty reader with no script loaded.
     */
    finLexReader();

    /*!
     *  \brief Constructs a reader seeded with \a inputstr.
     */
    finLexReader(const QString &inputstr);

    /*!
     *  \brief Destructs the reader.
     */
    virtual ~finLexReader();

    /*!
     *  \brief Returns the script currently being lexed.
     */
    QString getString() const;

    /*!
     *  \brief Returns the 0-based read position (character index into the script).
     */
    unsigned long getCurrentPosition() const;

    /*!
     *  \brief Returns the 0-based source row of the character at the read position.
     */
    unsigned int getRow() const;

    /*!
     *  \brief Returns the 0-based source column of the character at the read position.
     */
    unsigned int getColumn() const;

    /*!
     *  \brief Returns \c true if the read position is at or past the end of the input.
     */
    bool isReachedBottom() const;

    /*!
     *  \brief Replaces the script being lexed and rewinds the read position.
     *
     *  Throws finException with EC_STATE_ERROR if a previous lex pass is still in progress
     *  (i.e. _inputStr is non-empty and _posIdx is not 0). Call resetPosition() first, or
     *  construct a new reader, to change the script mid-pass.
     */
    void setString(const QString &instr);

    /*!
     *  \brief Rewinds the read position to the start of the input.
     */
    void resetPosition();

    /*!
     *  \brief Lexes the next token into \a retnode.
     *
     *  Skips leading whitespace, then probes the token types in the order selected by
     *  finLexReaderOrder and returns the first probe that successfully consumes a token.
     *
     *  \param retnode  Output node to fill; must not be \c nullptr.
     *  \return \c true if a token was lexed and stored in \a *retnode; \c false if the input
     *          was exhausted (only whitespace remained after the previous token).
     *
     *  \throws finException with EC_STATE_ERROR if the reader has no script loaded.
     *  \throws finException with EC_READ_ERROR if no probe matched the current character.
     */
    bool getNextLexNode(finLexNode *retnode);

    /*!
     *  \brief Returns a one-line textual description of the reader, used for logging.
     *
     *  Includes a snippet of the script and the current read position.
     */
    virtual QString dumpObjInfo() const override;

private:
    /*!
     *  \brief Formats \a str for inclusion in a debug log line.
     *
     *  Truncates to the first 10 characters, simplifies whitespace, and wraps the result in
     *  quotes (or renders it as `<EMPTY>` if the string is empty) so a long script does not
     *  flood the log.
     */
    static QString dbgScript(const QString str);

    /*!
     *  \brief Returns the length of the current input.
     */
    unsigned long scriptLength() const;

    /*!
     *  \brief Returns the character at absolute position \a pos, or QChar::Null if out of range.
     */
    QChar getScriptCharAt(unsigned long pos) const;

    /*!
     *  \brief Returns the character at the current read position (or QChar::Null at end-of-input).
     */
    QChar getScriptChar() const;

    /*!
     *  \brief Returns up to \a len characters of the input starting at absolute position \a pos.
     *
     *  Clamps to the end of the input if \a pos + \a len would overflow.
     */
    QString getScriptSubAt(unsigned long pos, unsigned long len) const;

    /*!
     *  \brief Returns up to \a len characters of the input starting at the current read position.
     */
    QString getScriptSub(unsigned long len) const;

    /*!
     *  \brief Advances the read position by one character, updating _curRow and _curCol.
     *
     *  A line feed increments the row and resets the column; any other character increments the
     *  column.
     *
     *  \return \c true if the new position is still in range; \c false if the advance ran off
     *          the end of the input (in which case _posIdx is clamped to scriptLength()).
     */
    bool moveReadPos();

    /*!
     *  \brief Advances the read position by up to \a detpos characters, stopping early at end-of-input.
     *
     *  \return \c true if the full \a detpos was consumed; \c false if end-of-input was hit first.
     */
    bool moveReadPosWith(unsigned long detpos);

    /*!
     *  \brief Skips whitespace at the current read position.
     *
     *  \return \c true if a non-whitespace character was found; \c false if the input was
     *          exhausted while skipping.
     */
    bool moveToNextNonblank();

    /*!
     *  \brief Fills \a retnode with a token of the given \a type covering the input up to
     *         absolute position \a endpos, then advances the read position to \a endpos.
     *
     *  The verbatim source slice [_posIdx, \a endpos) is recorded as the node's source string,
     *  and the current row/column (before the advance) is recorded as the source position.
     *
     *  \throws finException with EC_INVALID_PARAM if \a endpos is not past the current position.
     */
    void buildLexNode(finLexNode *retnode, finLexNodeType type, unsigned long endpos);

    /*! \enum finLexReader::finLexReaderOrder
     *  \brief Tiebreaker order used when probing token types in getNextLexNode().
     *
     *  The reader walks a fixed list of type probes (TP_NOTE, TP_STRING, TP_VARIABLE, then
     *  either TP_DECIMAL or TP_OPERATOR last) and returns the first one that successfully
     *  consumes a token. The two entries here swap the order of the last two probes:
     *
     *  - ORD_NUMBER_FIRST probes TP_DECIMAL before TP_OPERATOR. Used right after an operator
     *    so that `+5` lexes as a single signed number rather than as `+` followed by `5`.
     *  - ORD_OPERATOR_FIRST probes TP_OPERATOR before TP_DECIMAL. Used right after a value so
     *    that `5+3` lexes as three tokens (`5`, `+`, `3`) rather than as `5` followed by a
     *    malformed number starting with `+`.
     *
     *  The reader switches between the two orders automatically as it consumes tokens.
     */
    enum finLexReaderOrder {
        ORD_NUMBER_FIRST,     //!< Probe TP_DECIMAL before TP_OPERATOR.
        ORD_OPERATOR_FIRST    //!< Probe TP_OPERATOR before TP_DECIMAL.
    };

    finLexReaderOrder _nextReadOrder;   //!< Tiebreaker order to use for the next call to getNextLexNode().

    /*!
     *  \brief Returns the current tiebreaker order and the matching type-probe list.
     */
    void getLexTypeOrder(const finLexNodeType **typeorder, int *typenum);

    /*!
     *  \brief Returns the type-probe list for the explicit \a order.
     */
    void getLexTypeOrder(finLexReader::finLexReaderOrder order,
                         const finLexNodeType **typeorder, int *typenum);

    /*!
     *  \brief Dispatches to the matching tryGetXxx() helper for the given \a lextype.
     *
     *  Returns \c false for any lextype that has no tryGetXxx() helper.
     */
    bool tryGetTypedNode(finLexNode *retnode, finLexNodeType lextype);

    /*!
     *  \brief Tries to lex a `//` line comment or a `/* ... * /` block comment.
     *
     *  Returns \c false if the current position does not start a comment.
     */
    bool tryGetNote(finLexNode *retnode);

    /*!
     *  \brief Tries to lex an identifier and, on a match, promote it to a keyword.
     */
    bool tryGetVariable(finLexNode *retnode);

    /*!
     *  \brief Promotes \a retnode from TP_VARIABLE to TP_KEYWORD if its text matches a
     *         reserved word.
     *
     *  The reserved-word list lives in the .cpp file (kwlist[]).
     */
    bool tryRecogKeyword(finLexNode *retnode);

    /*!
     *  \brief Tries to lex a numeric literal (integer, decimal, or scientific notation).
     *
     *  All numbers in FIN-7 are stored as double; the parser does not distinguish fixed from
     *  floating point. Returns \c false if the current position does not start a number.
     */
    bool tryGetNumber(finLexNode *retnode);

    /*!
     *  \brief Tries to lex a double-quoted string literal, decoding escape sequences.
     *
     *  Supports `\\`, `\"`, `\n`, and `\t`. Other backslash sequences throw EC_READ_ERROR.
     */
    bool tryGetString(finLexNode *retnode);

    /*!
     *  \brief Tries to lex an operator or punctuation token.
     *
     *  Returns \c false if the current position does not start a recognized operator.
     */
    bool tryGetOperator(finLexNode *retnode);

    /*!
     *  \brief Returns \c true if \a ch can start a FIN-7 identifier (letter or underscore).
     */
    static bool isVariableStartChar(const QChar &ch);

    /*!
     *  \brief Returns \c true if \a ch can appear inside a FIN-7 identifier (letter, digit, or underscore).
     */
    static bool isVariableChar(const QChar &ch);
};

#endif // FINLEXREADER_H

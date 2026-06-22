/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finLexNode.h
 *  \brief Declarations of the lexer node and its token type / operator enumerations.
 *
 *  This header defines finLexNode, a tagged-union value class that holds a single token produced
 *  by finLexReader, plus the Type and Operator enumerations used to classify the token and its
 *  payload, and the convenience typedefs for those enumerations.
 */

#ifndef FINLEXNODE_H
#define FINLEXNODE_H

#include <QString>

#include "finErrorCode.h"


/*! \class finLexNode : public finExceptionObject
 *  \brief A single token produced by the FIN-7 lexer.
 *
 *  finLexNode is a tagged-union value class. A node carries:
 *  - a top-level type (Type enum) telling callers which payload slot is valid;
 *  - the verbatim source text of the token (the slice of script consumed by the lexer);
 *  - a payload stored in a small union, covering numeric literals, decoded string literals,
 *    and operator kinds;
 *  - a 0-based source position (row, column) of the first character of the token.
 *
 *  Nodes are produced by finLexReader::getNextLexNode(); they are mutable so the lexer can fill
 *  them in place rather than allocating a fresh one for every token.
 *
 *  \see finLexReader
 *  \see finExceptionObject
 */
class finLexNode : public finExceptionObject
{
public:
    /*! \enum finLexNode::Type
     *  \brief Top-level token types produced by the lexer.
     *
     *  Each value identifies how to read a finLexNode's payload. The lexer decides the type from
     *  the first non-blank character of the upcoming token; TP_KEYWORD is the only type the
     *  lexer assigns retroactively (it promotes a TP_VARIABLE whose text matches a reserved word).
     *
     *  \see finLexNode::Operator
     *  \see finLexNode::getType
     */
    enum Type {
        TP_DUMMY = 0,    //!< Reserved sentinel; not a real token type.

        TP_DECIMAL,      //!< Numeric literal (integer or floating-point, with optional sign and exponent).
        TP_STRING,       //!< String literal enclosed in double quotes; the payload holds the decoded text.
        TP_OPERATOR,     //!< Operator or punctuation token; the payload holds the operator kind.
        TP_NOTE,         //!< Comment token (line `//` or block `/* ... */`); kept to preserve source positions.
        TP_VARIABLE,     //!< Identifier token; promoted to TP_KEYWORD when the text matches a reserved word.
        TP_KEYWORD,      //!< Reserved keyword (e.g. `if`, `for`, `while`, `return`); see finLexReader::tryRecogKeyword.

        TP_MAX           //!< Sentinel marking the end of the enum.
    };

    /*! \enum finLexNode::Operator
     *  \brief Operator / punctuation kinds stored in a TP_OPERATOR node.
     *
     *  The lexer emits only a subset of these. Entries marked "parser-only" are never produced by
     *  the lexer itself; the parser assigns them when it disambiguates an already-lexed token
     *  (for example, promoting a lexed `-` to OP_NEGATIVE when used as a unary sign).
     *
     *  Bitwise and logical operators are lexed as distinct tokens: `~` is OP_BIT_NOT, `~~` is
     *  OP_LOGIC_NOT, `&` is OP_BIT_AND, `&&` is OP_LOGIC_AND, `$` is OP_BIT_XOR, `$$` is
     *  OP_LOGIC_XOR. Keeping the two families as separate tokens means the parser never has to
     *  re-disambiguate them.
     *
     *  \see finLexNode::Type
     *  \see finLexNode::getOperator
     */
    enum Operator {
        OP_DUMMY,        //!< Reserved sentinel; not a real operator.

        OP_ADD,          //!< Binary plus `+`.
        OP_SUB,          //!< Binary minus `-`.
        OP_POSITIVE,     //!< Unary plus (parser-only; never emitted by the lexer).
        OP_NEGATIVE,     //!< Unary minus (parser-only; never emitted by the lexer).
        OP_ACCUMLT,      //!< Increment `++` (lexer emits as prefix; parser also uses for postfix).
        OP_ACCUMLT_2,    //!< Postfix increment `++` (parser-internal alternative; lexer emits OP_ACCUMLT).
        OP_DESCEND,      //!< Decrement `--` (lexer emits as prefix; parser also uses for postfix).
        OP_DESCEND_2,    //!< Postfix decrement `--` (parser-internal alternative; lexer emits OP_DESCEND).
        OP_MUL,          //!< Multiplication `*`.
        OP_DIV,          //!< Division `/`.
        OP_MOD,          //!< Modulo `%`.
        OP_POWER,        //!< Power `^`.
        OP_FACTORI,      //!< Postfix factorial `!` (the parser may rewrite to OP_LOGIC_NOT).

        OP_L_RND_BRCKT,  //!< Left parenthesis `(`.
        OP_R_RND_BRCKT,  //!< Right parenthesis `)`.
        OP_L_SQR_BRCKT,  //!< Left square bracket `[`.
        OP_R_SQR_BRCKT,  //!< Right square bracket `]`.
        OP_L_FLW_BRCKT,  //!< Left curly brace `{`.
        OP_R_FLW_BRCKT,  //!< Right curly brace `}`.

        OP_LET,          //!< Assignment `=`.
        OP_EQUAL,        //!< Equality comparison `==`.
        OP_GRT,          //!< Greater than `>`.
        OP_LES,          //!< Less than `<`.
        OP_NONEQUAL,     //!< Not-equal comparison `<>`.
        OP_GRT_EQ,       //!< Greater-than-or-equal `>=`.
        OP_LES_EQ,       //!< Less-than-or-equal `<=`.

        OP_LOGIC_NOT,    //!< Logical NOT `~~` (the lexer distinguishes this from OP_BIT_NOT `~`).
        OP_LOGIC_AND,    //!< Logical AND `&&` (the lexer distinguishes this from OP_BIT_AND `&`).
        OP_LOGIC_OR,     //!< Logical OR `||`.
        OP_LOGIC_XOR,    //!< Logical XOR `$$` (the lexer distinguishes this from OP_BIT_XOR `$`).

        OP_BIT_NOT,      //!< Bitwise NOT `~` (single `~`; double `~~` is OP_LOGIC_NOT).
        OP_BIT_AND,      //!< Bitwise AND `&` (single `&`; double `&&` is OP_LOGIC_AND).
        OP_BIT_OR,       //!< Bitwise OR `|`.
        OP_BIT_XOR,      //!< Bitwise XOR `$` (single `$`; double `$$` is OP_LOGIC_XOR).

        OP_SPLIT,        //!< Statement separator `;`.
        OP_COMMA,        //!< Comma `,`.
        OP_COLON,        //!< Colon `:`.

        OP_FUNCTION,     //!< Function call marker (parser-only; never emitted by the lexer).
        OP_ACCESS,       //!< Member access `.` (parser-only; never emitted by the lexer).

        OP_MAX           //!< Sentinel marking the end of the enum.
    };

private:
    Type _type;                    //!< Top-level token type.
    QString _string;               //!< Verbatim source text of the token (the slice consumed by the lexer).
    QString _stringValue;          //!< Decoded payload: numeric text for TP_DECIMAL, unescaped text for TP_STRING.
    union {                        //!< Type-specific payload; consult _type before reading any field.
        long long _numericValue;   //!< Reserved for a future integer-literal type; currently unused.
        double _floatValue;        //!< Floating-point payload; valid when _type == TP_DECIMAL.
        Operator _operator;        //!< Operator kind; valid when _type == TP_OPERATOR.
        char _rawData[8];          //!< Raw byte view used to zero the union on reset and copy.
    } _u;
    unsigned int _row;             //!< 0-based source row of the token's first character.
    unsigned int _column;          //!< 0-based source column of the token's first character.

public:
    /*!
     *  \brief Default-constructs an empty TP_DUMMY node at source position (0, 0).
     */
    finLexNode();

    /*!
     *  \brief Copy-constructs a node by deep-copying the source node's union payload.
     */
    finLexNode(const finLexNode &src);

    /*!
     *  \brief Resets the node to a fresh TP_DUMMY state at source position (0, 0).
     */
    void reset();

    /*!
     *  \brief Deep-copies another node's payload into this one.
     *
     *  If \a srcnode is \c nullptr, a warning is logged via finWarning and the node is reset.
     *
     *  \param srcnode  The source node to copy from; may be \c nullptr.
     */
    void copyNode(const finLexNode *srcnode);

    /*!
     *  \brief Returns the top-level token type.
     */
    Type getType() const;

    /*!
     *  \brief Returns the verbatim source text of the token.
     */
    QString getString() const;

    /*!
     *  \brief Returns the floating-point payload, or 0.0 if _type is not TP_DECIMAL.
     */
    double getFloatValue() const;

    /*!
     *  \brief Returns the operator kind, or OP_DUMMY if _type is not TP_OPERATOR.
     */
    Operator getOperator() const;

    /*!
     *  \brief Returns the decoded string payload, or an empty string if _type is not TP_STRING.
     */
    QString getStringValue() const;

    /*!
     *  \brief Returns the 0-based source row of the token's first character.
     */
    unsigned int getRow() const;

    /*!
     *  \brief Returns the 0-based source column of the token's first character.
     */
    unsigned int getColumn() const;

    /*!
     *  \brief Sets the top-level token type.
     */
    void setType(Type type);

    /*!
     *  \brief Sets the verbatim source text.
     */
    void setString(const QString &str);

    /*!
     *  \brief Sets the floating-point payload.
     *
     *  Throws finException with EC_STATE_ERROR if the current type is not TP_DECIMAL.
     */
    void setFloatValue(double val);

    /*!
     *  \brief Sets the operator kind.
     *
     *  Throws finException with EC_STATE_ERROR if the current type is not TP_OPERATOR.
     */
    void setOperator(Operator optype);

    /*!
     *  \brief Sets the decoded string payload.
     *
     *  Throws finException with EC_STATE_ERROR if the current type is not TP_STRING.
     */
    void setStringValue(const QString &strval);

    /*!
     *  \brief Sets the 0-based source row.
     */
    void setRow(unsigned int row);

    /*!
     *  \brief Sets the 0-based source column.
     */
    void setColumn(unsigned int column);

    /*!
     *  \brief Returns a one-line textual description of the node, used for logging.
     *
     *  Includes the source row/column, the source text, the type, and the type-specific payload.
     */
    virtual QString dumpObjInfo() const override;
};

/*! \typedef finLexNodeType
 *  \brief Shorthand alias for finLexNode::Type.
 */
typedef enum finLexNode::Type finLexNodeType;

/*! \typedef finLexOperatorType
 *  \brief Shorthand alias for finLexNode::Operator.
 */
typedef enum finLexNode::Operator finLexOperatorType;

#endif // FINLEXNODE_H

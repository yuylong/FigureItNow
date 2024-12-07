/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINLEXNODE_H
#define FINLEXNODE_H

#include <QString>

#include "finErrorCode.h"


class finLexNode : finExceptionObject
{
public:
    enum Type
    {
        TP_DUMMY = 0,
        TP_DECIMAL,
        TP_STRING,
        TP_OPERATOR,
        TP_NOTE,
        TP_VARIABLE,
        TP_KEYWORD,
        TP_MAX
    };

    enum Operator
    {
        OP_DUMMY = 0,
        OP_ADD,
        OP_SUB,
        OP_POSITIVE,
        OP_NEGATIVE,
        OP_ACCUMLT,
        OP_ACCUMLT_2,
        OP_DESCEND,
        OP_DESCEND_2,
        OP_MUL,
        OP_DIV,
        OP_MOD,
        OP_POWER,
        OP_FACTORI,
        OP_L_RND_BRCKT,
        OP_R_RND_BRCKT,
        OP_L_SQR_BRCKT,
        OP_R_SQR_BRCKT,
        OP_L_FLW_BRCKT,
        OP_R_FLW_BRCKT,
        OP_LET,
        OP_EQUAL,
        OP_GRT,
        OP_LES,
        OP_NONEQUAL,
        OP_GRT_EQ,
        OP_LES_EQ,
        OP_LOGIC_NOT,
        OP_LOGIC_AND,
        OP_LOGIC_OR,
        OP_LOGIC_XOR,
        OP_BIT_NOT,
        OP_BIT_AND,
        OP_BIT_OR,
        OP_BIT_XOR,
        OP_SPLIT,
        OP_COMMA,
        OP_COLON,
        OP_FUNCTION,
        OP_ACCESS,
        OP_MAX
    };

private:
    Type _type;
    QString _string;

    QString _stringValue;
    union {
        long long _numericValue;
        double _floatValue;
        Operator _operator;
        char _rawData[8];
    } _u;

    unsigned int _row;
    unsigned int _column;

public:
    finLexNode();
    finLexNode(const finLexNode &src);

    void reset();
    void copyNode(const finLexNode *srcnode);

    Type getType() const;
    QString getString() const;
    double getFloatValue() const;
    Operator getOperator() const;
    QString getStringValue() const;
    unsigned int getRow() const;
    unsigned int getColumn() const;

    void setType(Type type);
    void setString(const QString &str);
    void setFloatValue(double val);
    void setOperator(Operator optype);
    void setStringValue(const QString &strval);
    void setRow(unsigned int row);
    void setColumn(unsigned int column);

    virtual QString dumpObjInfo() const override;
};

typedef enum finLexNode::Type finLexNodeType;
typedef enum finLexNode::Operator finLexOperatorType;

#endif // FINLEXNODE_H

#ifndef FINLEXNODE_H
#define FINLEXNODE_H

#include <QString>

#include "finErrorCode.h"

class finLexNode
{
public:
    enum finLexNodeType
    {
        FIN_LN_TYPE_DUMMY = 0,
        FIN_LN_TYPE_DECIMAL,
        FIN_LN_TYPE_STRING,
        FIN_LN_TYPE_OPERATOR,
        FIN_LN_TYPE_NOTE,
        FIN_LN_TYPE_VARIABLE,
        FIN_LN_TYPE_KEYWORD,
        FIN_LN_TYPE_MAX
    };

    enum finLexNodeOperatorType
    {
        FIN_LN_OPTYPE_DUMMY = 0,
        FIN_LN_OPTYPE_ADD,
        FIN_LN_OPTYPE_SUB,
        FIN_LN_OPTYPE_POSITIVE,
        FIN_LN_OPTYPE_NEGATIVE,
        FIN_LN_OPTYPE_ACCUMLT,
        FIN_LN_OPTYPE_ACCUMLT_2,
        FIN_LN_OPTYPE_MUL,
        FIN_LN_OPTYPE_DIV,
        FIN_LN_OPTYPE_MOD,
        FIN_LN_OPTYPE_POWER,
        FIN_LN_OPTYPE_L_RND_BRCKT,
        FIN_LN_OPTYPE_R_RND_BRCKT,
        FIN_LN_OPTYPE_L_SQR_BRCKT,
        FIN_LN_OPTYPE_R_SQR_BRCKT,
        FIN_LN_OPTYPE_L_FLW_BRCKT,
        FIN_LN_OPTYPE_R_FLW_BRCKT,
        FIN_LN_OPTYPE_LET,
        FIN_LN_OPTYPE_EQUAL,
        FIN_LN_OPTYPE_GRT,
        FIN_LN_OPTYPE_LES,
        FIN_LN_OPTYPE_NONEQUAL,
        FIN_LN_OPTYPE_GRT_EQ,
        FIN_LN_OPTYPE_LES_EQ,
        FIN_LN_OPTYPE_LOGIC_NOT,
        FIN_LN_OPTYPE_LOGIC_AND,
        FIN_LN_OPTYPE_LOGIC_OR,
        FIN_LN_OPTYPE_LOGIC_XOR,
        FIN_LN_OPTYPE_BIT_NOT,
        FIN_LN_OPTYPE_BIT_AND,
        FIN_LN_OPTYPE_BIT_OR,
        FIN_LN_OPTYPE_BIT_XOR,
        FIN_LN_OPTYPE_SPLIT,
        FIN_LN_OPTYPE_COMMA,
        FIN_LN_OPTYPE_COLON,
        FIN_LN_OPTYPE_FUNCTION,
        FIN_LN_OPTYPE_ACCESS,
        FIN_LN_OPTYPE_MAX
    };

private:
    finLexNodeType _type;
    QString _string;

    QString _stringValue;
    union {
        long long _numericValue;
        double _floatValue;
        finLexNodeOperatorType _operator;
        char _rawData[8];
    } _u;

    unsigned int _row;
    unsigned int _column;

public:
    finLexNode();

    finLexNodeType getType() const;
    QString getString() const;
    double getFloatValue() const;
    finLexNodeOperatorType getOperator() const;
    QString getStringValue() const;
    unsigned int getRow() const;
    unsigned int getColumn() const;

    finErrorCode setType(finLexNodeType type);
    finErrorCode setString(const QString &str);
    finErrorCode setFloatValue(double val);
    finErrorCode setOperator(finLexNodeOperatorType optype);
    finErrorCode setStringValue(const QString &strval);
    finErrorCode setRow(unsigned int row);
    finErrorCode setColumn(unsigned int column);
};

typedef enum finLexNode::finLexNodeType finLexNodeType;
typedef enum finLexNode::finLexNodeOperatorType finLexOperatorType;

#endif // FINLEXNODE_H

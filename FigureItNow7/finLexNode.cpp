/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */

#include "finLexNode.h"

#include <stdlib.h>
#include <QTextStream>


finLexNode::finLexNode() :
    _string(), _stringValue()
{
    this->_type = TP_DUMMY;
    memset(this->_u._rawData, 0, sizeof (this->_u));
    this->_row = 0;
    this->_column = 0;
}

finLexNode::finLexNode(const finLexNode &src)
{
    this->copyNode(&src);
}

void finLexNode::reset()
{
    this->_string = QString();
    this->_type = TP_DUMMY;
    this->_stringValue = QString();
    memset(this->_u._rawData, 0, sizeof (this->_u));
    this->_row = 0;
    this->_column = 0;
}

void finLexNode::copyNode(const finLexNode *srcnode)
{
    if ( srcnode == nullptr )
        return this->reset();

    this->_string = srcnode->getString();
    this->_type = srcnode->getType();
    this->_stringValue = srcnode->getStringValue();
    this->_row = srcnode->getRow();
    this->_column = srcnode->getColumn();

    switch ( this->_type ) {
      case finLexNode::TP_DECIMAL:
        this->_u._floatValue = srcnode->getFloatValue();
        break;

      case finLexNode::TP_OPERATOR:
        this->_u._operator = srcnode->getOperator();
        break;

      default:
        memset(this->_u._rawData, 0, sizeof (this->_u));
        break;
    }
}

finLexNodeType finLexNode::getType() const
{
    return this->_type;
}

QString finLexNode::getString() const
{
    return this->_string;
}

double finLexNode::getFloatValue() const
{
    if ( this->_type != TP_DECIMAL )
        return 0.0;

    return this->_u._floatValue;
}

finLexOperatorType finLexNode::getOperator() const
{
    if ( this->_type != TP_OPERATOR )
        return finLexNode::OP_DUMMY;

    return this->_u._operator;
}

QString finLexNode::getStringValue() const
{
    if ( this->_type != TP_STRING )
        return QString();

    return this->_stringValue;
}

unsigned int finLexNode::getRow() const
{
    return this->_row;
}

unsigned int finLexNode::getColumn() const
{
    return this->_column;
}

void finLexNode::setType(finLexNodeType type)
{
    this->_type = type;
}

void finLexNode::setString(const QString &str)
{
    this->_string = str;
}

void finLexNode::setFloatValue(double val)
{
    if ( this->_type != TP_DECIMAL )
        throw finException(finErrorKits::EC_STATE_ERROR, "Setup float value to non-numeric lex node.", this);

    this->_u._floatValue = val;
}

void finLexNode::setOperator(finLexOperatorType optype)
{
    if ( this->_type != TP_OPERATOR )
        throw finException(finErrorKits::EC_STATE_ERROR, "Setup operator type to non-operator lex node.", this);

    this->_u._operator = optype;
}

void finLexNode::setStringValue(const QString &strval)
{
    if ( this->_type != TP_STRING )
        throw finException(finErrorKits::EC_STATE_ERROR, "Setup string to non-string lex node.", this);

    this->_stringValue = strval;
}

void finLexNode::setRow(unsigned int row)
{
    this->_row = row;
}

void finLexNode::setColumn(unsigned int column)
{
    this->_column = column;
}

QString finLexNode::dumpObjInfo() const
{
    QString retstr;
    QTextStream ts(&retstr);

    ts << "[" << this->_row << ":" << this->_column << "] " << this->_string << "(" << this->_type << ") = ";
    switch (this->_type) {
    case finLexNode::TP_DECIMAL:
        ts << this->_u._floatValue; break;
    case finLexNode::TP_OPERATOR:
        ts << this->_u._operator; break;
    default:
        ts << this->_stringValue; break;
    }

    return retstr;
}

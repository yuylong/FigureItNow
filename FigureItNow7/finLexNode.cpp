/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finLexNode.h"

#include <stdlib.h>


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

finErrorCode finLexNode::reset()
{
    this->_string = QString();
    this->_type = TP_DUMMY;
    this->_stringValue = QString();
    memset(this->_u._rawData, 0, sizeof (this->_u));
    this->_row = 0;
    this->_column = 0;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::copyNode(const finLexNode *srcnode)
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
    return finErrorKits::EC_SUCCESS;
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

finErrorCode finLexNode::setType(finLexNodeType type)
{
    this->_type = type;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setString(const QString &str)
{
    this->_string = str;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setFloatValue(double val)
{
    if ( this->_type != TP_DECIMAL )
        return finErrorKits::EC_STATE_ERROR;

    this->_u._floatValue = val;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setOperator(finLexOperatorType optype)
{
    if ( this->_type != TP_OPERATOR )
        return finErrorKits::EC_STATE_ERROR;

    this->_u._operator = optype;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setStringValue(const QString &strval)
{
    if ( this->_type != TP_STRING )
        return finErrorKits::EC_STATE_ERROR;

    this->_stringValue = strval;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setRow(unsigned int row)
{
    this->_row = row;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finLexNode::setColumn(unsigned int column)
{
    this->_column = column;
    return finErrorKits::EC_SUCCESS;
}

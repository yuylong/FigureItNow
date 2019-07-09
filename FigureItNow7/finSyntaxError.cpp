/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxError.h"


finSyntaxError::finSyntaxError()
    : _errString()
{
    this->_row = 0;
    this->_column = 0;
}

finSyntaxError::finSyntaxError(const finSyntaxError &src)
{
    this->copySyntaxError(&src);
}

finErrorCode finSyntaxError::copySyntaxError(const finSyntaxError *src)
{
    this->_level = src->_level;
    this->_stage = src->_stage;
    this->_row = src->getRow();
    this->_column = src->getColumn();
    this->_errString = src->getErrorString();
    return finErrorKits::EC_SUCCESS;
}

finSyntaxError::Level finSyntaxError::getLevel() const
{
    return this->_level;
}

finSyntaxError::Stage finSyntaxError::getStage() const
{
    return this->_stage;
}

unsigned int finSyntaxError::getRow() const
{
    return this->_row;
}

unsigned int finSyntaxError::getColumn() const
{
    return this->_column;
}

QString finSyntaxError::getErrorString() const
{
    return this->_errString;
}

finErrorCode finSyntaxError::setLevel(finSyntaxError::Level level)
{
    this->_level = level;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::setStage(finSyntaxError::Stage stage)
{
    this->_stage = stage;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::setRow(unsigned int row)
{
    this->_row = row;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::setColumn(unsigned int col)
{
    this->_column = col;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::setErrorString(const QString &errstr)
{
    this->_errString = errstr;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                                  const QString &errinfo)
{
    if ( lexnode == nullptr || errlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finSyntaxError synerr;
    synerr.setRow(lexnode->getRow());
    synerr.setColumn(lexnode->getColumn());
    synerr.setErrorString(errinfo);

    errlist->append(synerr);
    return finErrorKits::EC_SUCCESS;
}

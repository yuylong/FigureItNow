#include "finSyntaxError.h"

finSyntaxError::finSyntaxError()
    : _errString()
{
    this->_row = 0;
    this->_column = 0;
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

finErrorCode finSyntaxError::setRow(unsigned int row)
{
    this->_row = row;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxError::setColumn(unsigned int col)
{
    this->_column = col;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finSyntaxError::setErrorString(const QString &errstr)
{
    this->_errString = errstr;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

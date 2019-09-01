/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include <QObject>

#include "finSyntaxError.h"


finSyntaxError::finSyntaxError()
    : _errString()
{
    this->_level = LV_DUMMY;
    this->_stage = ST_DUMMY;
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

finErrorCode finSyntaxError::dumpErrorInfo(QTextStream *ts) const
{
    if ( ts == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    (*ts) << "<" << getLevelName(this->_level) << "> "
          << getStageName(this->_stage) << " "
          << "[" << this->_row << ":" << this->_column << "] "
          << this->_errString << endl;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxError::dumpErrorInfo(finSyntaxErrorDump *dumper) const
{
    QString errinfo = this->makeErrorInfoString();
    if ( errinfo.isEmpty() )
        return finErrorKits::EC_INVALID_PARAM;

    dumper->dumpText(errinfo);
    return finErrorKits::EC_SUCCESS;
}

QString finSyntaxError::makeErrorInfoString() const
{
    finErrorCode errcode = finErrorKits::EC_SUCCESS;
    QString retstr = "";
    QTextStream ts(&retstr);

    errcode = dumpErrorInfo(&ts);
    if ( finErrorKits::isErrorResult(errcode) )
        return "";

    return retstr;
}

const finSyntaxError &finSyntaxError::dummySyntaxError()
{
    static finSyntaxError retval;
    static bool retval_isset = false;

    if ( !retval_isset ) {
        retval._level = LV_DUMMY;
        retval._stage = ST_DUMMY;
        retval._row = 0;
        retval._column = 0;
        retval._errString = QObject::tr("Invalid syntax error entry.");
        retval_isset = true;
    }

    return retval;
}

QString finSyntaxError::getLevelName(Level level)
{
    switch ( level ) {
      case LV_DEBUG:
        return QObject::tr("Debug");

      case LV_WARNING:
        return QObject::tr("Warning");

      case LV_ERROR:
        return QObject::tr("Error");

      default:
        return QObject::tr("Dummy");
    }
}

QString finSyntaxError::getStageName(Stage stage)
{
    switch ( stage ) {
      case ST_COMPILE:
        return QObject::tr("Compile");

      case ST_EXECUTE:
        return QObject::tr("Execute");

      default:
        return QObject::tr("Dummy");
    }
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

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include <QObject>

#include "finSyntaxError.h"


finSyntaxError::finSyntaxError()
    : _errString()
{
    this->_code = CD_DUMMY;
    this->_level = LV_DUMMY;
    this->_stage = ST_DUMMY;
    this->_row = 0;
    this->_column = 0;
}

finSyntaxError::finSyntaxError(const finSyntaxError &src)
{
    this->copySyntaxError(&src);
}

void finSyntaxError::copySyntaxError(const finSyntaxError *src)
{
    if ( src == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Copy from a null syntax error.");

    this->_code = src->getCode();
    this->_level = src->getLevel();
    this->_stage = src->getStage();
    this->_row = src->getRow();
    this->_column = src->getColumn();
    this->_errString = src->getErrorString();
}

finSyntaxError &finSyntaxError::operator =(const finSyntaxError &src)
{
    copySyntaxError(&src);
    return *this;
}

finSyntaxError::Code finSyntaxError::getCode() const
{
    return this->_code;
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

void finSyntaxError::setCode(finSyntaxError::Code code)
{
    this->_code = code;
}

void finSyntaxError::setLevel(finSyntaxError::Level level)
{
    this->_level = level;
}

void finSyntaxError::setStage(finSyntaxError::Stage stage)
{
    this->_stage = stage;
}

void finSyntaxError::setRow(unsigned int row)
{
    this->_row = row;
}

void finSyntaxError::setColumn(unsigned int col)
{
    this->_column = col;
}

void finSyntaxError::setErrorString(const QString &errstr)
{
    this->_errString = errstr;
}

void finSyntaxError::setErrorInfo(Level level, Stage stage, Code code,
                                  const finLexNode *lexnode, const QString &errstr)
{
    this->setLevel(level);
    this->setStage(stage);
    this->setCode(code);
    if ( lexnode != nullptr ) {
        this->setRow(lexnode->getRow());
        this->setColumn(lexnode->getColumn());
    } else {
        this->setRow(0);
        this->setColumn(0);
    }
    this->setErrorString(errstr);
}

void finSyntaxError::dumpErrorInfo(QTextStream *ts) const
{
    if ( ts == nullptr ) {
        throw finException(finErrorKits::EC_NULL_POINTER, "Cannot dump syntax error to a NULL text stream.");
    }

    (*ts) << "<" << getLevelName(this->_level) << "> "
          << getStageName(this->_stage) << " "
          << "[" << this->_row << ":" << this->_column << "] "
          << "(" << getCodeName(this->_code) << ") "
          << this->_errString << Qt::endl;
}

void finSyntaxError::dumpErrorInfo(finSyntaxErrorDump *dumper) const
{
    QString errinfo = this->makeErrorInfoString();
    if ( errinfo.isEmpty() )
        return;

    dumper->dumpText(errinfo);
}

QString finSyntaxError::makeErrorInfoString() const
{
    QString retstr = "";
    QTextStream ts(&retstr);

    dumpErrorInfo(&ts);
    return retstr;
}

const finSyntaxError &finSyntaxError::dummySyntaxError()
{
    static finSyntaxError retval;
    static bool retval_isset = false;

    if ( !retval_isset ) {
        retval._code = CD_DUMMY;
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

      case LV_INFO:
        return QObject::tr("Info");

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
      case ST_LEX:
        return QObject::tr("Lex");

      case ST_COMPILE:
        return QObject::tr("Compile");

      case ST_EXECUTE:
        return QObject::tr("Execute");

      default:
        return QObject::tr("Dummy");
    }
}

QString finSyntaxError::getCodeName(Code code)
{
    switch ( code ) {
      case CD_LEX_ERROR:
        return QObject::tr("LexError");

      case CD_UNEXPECTED_TOKEN:
        return QObject::tr("UnexpectedToken");

      case CD_EXPECTED_TOKEN:
        return QObject::tr("ExpectedToken");

      case CD_BRACKET_MISMATCH:
        return QObject::tr("BracketMismatch");

      case CD_STRUCTURE_MISMATCH:
        return QObject::tr("StructureMismatch");

      case CD_OPERAND_MISSING:
        return QObject::tr("OperandMissing");

      case CD_INVALID_LABEL:
        return QObject::tr("InvalidLabel");

      case CD_UNKNOWN_SYMBOL:
        return QObject::tr("UnknownSymbol");

      default:
        return QObject::tr("Dummy");
    }
}

void finSyntaxError::appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                          const QString &errinfo)
{
    if ( lexnode == nullptr || errlist == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Append execution error with null pointer.");

    finSyntaxError synerr;
    synerr.setRow(lexnode->getRow());
    synerr.setColumn(lexnode->getColumn());
    synerr.setErrorString(errinfo);

    errlist->append(synerr);
}

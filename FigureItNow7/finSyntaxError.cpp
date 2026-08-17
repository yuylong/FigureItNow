/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finSyntaxError.cpp
 *  \brief Implementations of the finSyntaxError diagnostic record.
 *
 *  Provides the constructors and field accessors declared in finSyntaxError.h, the formatted
 *  dump and lookup helpers used by the front ends, the shared dummy entry, and the static
 *  appendExecutionError() convenience wrapper.
 */

#include <QObject>

#include "finSyntaxError.h"


// An empty diagnostic: every field keeps its dummy value and position (0, 0).
finSyntaxError::finSyntaxError()
    : _errString()
{
    this->_code = CD_DUMMY;
    this->_level = LV_DUMMY;
    this->_stage = ST_DUMMY;
    this->_row = 0;
    this->_column = 0;
}

// Copy construction and copy assignment both funnel through copySyntaxError() so the field list
// stays in one place.
finSyntaxError::finSyntaxError(const finSyntaxError &src)
{
    this->copySyntaxError(&src);
}

void finSyntaxError::copySyntaxError(const finSyntaxError *src)
{
    // A null source is a caller bug; fail loudly instead of copying garbage.
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

// Single-call entry point for error producers: fill level, stage, code, position, and message
// at once. Without a source token, reset the position instead of leaving stale coordinates.
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

// Render one diagnostic line: "<Level> Stage [row:column] (Code) message", with the trailing
// newline. The stream must be valid; a null stream is a caller bug.
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

// Reuse the stream formatter so every sink produces byte-identical output; skip empty text.
void finSyntaxError::dumpErrorInfo(finSyntaxErrorDump *dumper) const
{
    QString errinfo = this->makeErrorInfoString();
    if ( errinfo.isEmpty() )
        return;

    dumper->dumpText(errinfo);
}

// Format into an in-memory QTextStream so callers get one complete string, newline included.
QString finSyntaxError::makeErrorInfoString() const
{
    QString retstr = "";
    QTextStream ts(&retstr);

    dumpErrorInfo(&ts);
    return retstr;
}

// Lazily initialize the shared dummy entry once; it stands in for invalid or out-of-range
// lookups so callers always receive a usable object.
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

// Name lookups are intentionally tolerant: unknown or sentinel values fall back to "Dummy"
// instead of throwing, so a malformed entry can still be rendered.
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

// Convenience path used by execution error producers: copy the token position plus the message.
// Level, stage, and code stay at their dummy values; use finSyntaxErrorList::appendEntry() when
// the full field set is needed.
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

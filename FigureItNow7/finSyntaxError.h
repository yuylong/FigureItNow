/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXERROR_H
#define FINSYNTAXERROR_H

#include <QString>
#include <QTextStream>

#include "finErrorCode.h"
#include "finSyntaxErrorDump.h"
#include "finLexNode.h"


class finSyntaxError
{
public:
    enum Level {
        LV_DUMMY,
        LV_DEBUG,
        LV_INFO,
        LV_WARNING,
        LV_ERROR,
    };

    enum Stage {
        ST_DUMMY,
        ST_COMPILE,
        ST_EXECUTE,
    };

    /*! \enum finSyntaxError::Code
     *  \brief Machine-readable category of a diagnostic.
     *
     *  Complements the free-form message (see setErrorString()) with a stable, comparable category so
     *  consumers can filter, color-code, and localize diagnostics without parsing the message text.
     *  The granularity is deliberately coarse; the specific failure remains in the message string.
     */
    enum Code {
        CD_DUMMY,              //!< Reserved sentinel; the diagnostic has not been classified.
        CD_LEX_ERROR,          //!< The lexer could not recognize a token.
        CD_UNEXPECTED_TOKEN,   //!< A token appeared where it is not allowed (e.g. an extra `)`).
        CD_EXPECTED_TOKEN,     //!< A required token is missing (e.g. a missing `;`).
        CD_BRACKET_MISMATCH,   //!< A closing bracket does not match its enclosing opening bracket.
        CD_STRUCTURE_MISMATCH, //!< A statement / declaration / jump / loop structure is malformed.
        CD_OPERAND_MISSING,    //!< An operator is missing a before- or after-operand.
        CD_INVALID_LABEL,      //!< A label (`name:`) is malformed.
        CD_UNKNOWN_SYMBOL,     //!< A symbol could not be resolved.
    };

protected:
    Code _code;              //!< Error category (see Code).
    Level _level;
    Stage _stage;
    unsigned int _row;
    unsigned int _column;
    QString _errString;

public:
    finSyntaxError();
    finSyntaxError(const finSyntaxError &src);

    finSyntaxError &operator =(const finSyntaxError &src);

    void copySyntaxError(const finSyntaxError *src);

    Code getCode() const;
    Level getLevel() const;
    Stage getStage() const;
    unsigned int getRow() const;
    unsigned int getColumn() const;
    QString getErrorString() const;

    void setCode(Code code);
    void setLevel(Level level);
    void setStage(Stage stage);
    void setRow(unsigned int row);
    void setColumn(unsigned int col);
    void setErrorString(const QString &errstr);

    /*!
     *  \brief Fills all diagnostic fields in one call.
     *
     *  Convenience setter for error producers (the parser, compiler wrapper, and execution machine)
     *  to populate level, stage, code, source position, and message in one shot. When \a lexnode is
     *  \c nullptr, the source position is reset to (0, 0).
     */
    void setErrorInfo(Level level, Stage stage, Code code,
                      const finLexNode *lexnode, const QString &errstr);

    void dumpErrorInfo(QTextStream *ts) const;
    void dumpErrorInfo(finSyntaxErrorDump *dumper) const;
    QString makeErrorInfoString() const;

    static const finSyntaxError &dummySyntaxError();

    static QString getLevelName(Level level);
    static QString getStageName(Stage stage);
    static QString getCodeName(Code code);

    static void appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                     const QString &errinfo);
};

#endif // FINSYNTAXERROR_H

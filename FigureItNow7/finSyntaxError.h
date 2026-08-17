/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finSyntaxError.h
 *  \brief Declaration of the finSyntaxError diagnostic record.
 *
 *  This header defines finSyntaxError, the value type that carries one diagnostic entry from any
 *  stage of the FIN-7 pipeline. Problems are reported by the lexer and parser (stored in the
 *  syntax tree) and by the execution machine; the UI and command-line front ends then filter the
 *  entries by severity and render each one as a single line.
 */

#ifndef FINSYNTAXERROR_H
#define FINSYNTAXERROR_H

#include <QString>
#include <QTextStream>

#include "finErrorCode.h"
#include "finSyntaxErrorDump.h"
#include "finLexNode.h"


/*! \class finSyntaxError
 *  \brief One diagnostic entry produced while lexing, compiling, or executing a FIN script.
 *
 *  A finSyntaxError value bundles everything needed to present and filter one problem:
 *
 *  - a severity (Level);
 *  - the pipeline stage that reported it (Stage);
 *  - a machine-readable category (Code);
 *  - a 0-based source position (row, column), taken from the offending finLexNode;
 *  - a free-form, human-readable message.
 *
 *  Entries are plain values: they are appended to the lists kept by the syntax tree and by
 *  finSyntaxErrorList, then rendered with dumpErrorInfo() or makeErrorInfoString(). Severities are
 *  ordered numerically so a Level can be compared directly ("at least as severe as");
 *  finSyntaxErrorList relies on that when it filters entries for dumping.
 *
 *  \see finSyntaxErrorList
 *  \see finLexNode
 */
class finSyntaxError
{
public:
    /*! \enum finSyntaxError::Level
     *  \brief Severity of a diagnostic entry.
     *
     *  The values are ordered by increasing severity so that they can be compared directly:
     *  finSyntaxErrorList dumps only the entries whose Level is greater than or equal to its
     *  starting level.
     *
     *  \see finSyntaxErrorList::getEntryCountFromLevel
     */
    enum Level {
        LV_DUMMY,    //!< Reserved sentinel; not a real severity.
        LV_DEBUG,    //!< Verbose developer diagnostic; usually filtered out of user-facing dumps.
        LV_INFO,     //!< Informational message that does not indicate a problem.
        LV_WARNING,  //!< Recoverable problem; the script can continue, but the result may be suspect.
        LV_ERROR,    //!< Error; the affected statement, or the script run as a whole, failed.
    };

    /*! \enum finSyntaxError::Stage
     *  \brief Pipeline stage that produced a diagnostic entry.
     *
     *  Mirrors the FIN-7 pipeline: the lexer turns script text into tokens, the parser builds the
     *  syntax tree, and the execution machine walks it. Recording the stage lets the front ends
     *  tell a load-time problem from a runtime one.
     */
    enum Stage {
        ST_DUMMY,    //!< Reserved sentinel; not a real stage.
        ST_LEX,      //!< Reported while reading tokens from the script text.
        ST_COMPILE,  //!< Reported while parsing tokens into the syntax tree.
        ST_EXECUTE,  //!< Reported while walking and executing the syntax tree.
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
    Level _level;            //!< Severity of the entry (see Level).
    Stage _stage;            //!< Pipeline stage that reported the entry (see Stage).
    unsigned int _row;       //!< 0-based source row of the offending token, or 0 when unknown.
    unsigned int _column;    //!< 0-based source column of the offending token, or 0 when unknown.
    QString _errString;      //!< Free-form, human-readable message.

public:
    /*!
     *  \brief Constructs an empty, unclassified diagnostic.
     *
     *  Every field starts at its dummy value and the source position is (0, 0).
     */
    finSyntaxError();

    /*!
     *  \brief Copy constructor; copies all fields of \a src.
     */
    finSyntaxError(const finSyntaxError &src);

    /*!
     *  \brief Copy assignment; copies all fields of \a src into this entry.
     *
     *  \return Reference to this object.
     */
    finSyntaxError &operator =(const finSyntaxError &src);

    /*!
     *  \brief Copies all fields from another diagnostic.
     *
     *  Throws finException with EC_NULL_POINTER when \a src is \c nullptr.
     *
     *  \param src  The entry to copy from.
     */
    void copySyntaxError(const finSyntaxError *src);

    /*!
     *  \brief Returns the machine-readable category of the entry.
     *
     *  \see Code
     */
    Code getCode() const;

    /*!
     *  \brief Returns the severity of the entry.
     *
     *  \see Level
     */
    Level getLevel() const;

    /*!
     *  \brief Returns the pipeline stage that reported the entry.
     *
     *  \see Stage
     */
    Stage getStage() const;

    /*!
     *  \brief Returns the 0-based source row, or 0 when the position is unknown.
     */
    unsigned int getRow() const;

    /*!
     *  \brief Returns the 0-based source column, or 0 when the position is unknown.
     */
    unsigned int getColumn() const;

    /*!
     *  \brief Returns the free-form, human-readable message.
     */
    QString getErrorString() const;

    /*!
     *  \brief Sets the machine-readable category.
     */
    void setCode(Code code);

    /*!
     *  \brief Sets the severity.
     */
    void setLevel(Level level);

    /*!
     *  \brief Sets the pipeline stage.
     */
    void setStage(Stage stage);

    /*!
     *  \brief Sets the 0-based source row.
     */
    void setRow(unsigned int row);

    /*!
     *  \brief Sets the 0-based source column.
     */
    void setColumn(unsigned int col);

    /*!
     *  \brief Sets the free-form message.
     */
    void setErrorString(const QString &errstr);

    /*!
     *  \brief Fills all diagnostic fields in one call.
     *
     *  Convenience setter for error producers (such as the syntax reader) to populate level, stage,
     *  code, source position, and message in one shot. When \a lexnode is \c nullptr, the source
     *  position is reset to (0, 0).
     */
    void setErrorInfo(Level level, Stage stage, Code code,
                      const finLexNode *lexnode, const QString &errstr);

    /*!
     *  \brief Writes the diagnostic as one formatted line to a text stream.
     *
     *  The line has the form `<Level> Stage [row:column] (Code) message` and ends with a newline.
     *  Level, Stage, and Code are rendered through the matching getLevelName() / getStageName() /
     *  getCodeName() helper.
     *
     *  Throws finException with EC_NULL_POINTER when \a ts is \c nullptr.
     *
     *  \param ts  Stream to write to.
     */
    void dumpErrorInfo(QTextStream *ts) const;

    /*!
     *  \brief Forwards the formatted diagnostic to a dump sink.
     *
     *  Formats the entry with makeErrorInfoString() and passes it to \a dumper->dumpText(); nothing
     *  is forwarded when the formatted text is empty. The dumper must be valid; it is not checked
     *  here.
     *
     *  \param dumper  Sink that receives the text, e.g. the GUI plain-text panel.
     *
     *  \see makeErrorInfoString
     */
    void dumpErrorInfo(finSyntaxErrorDump *dumper) const;

    /*!
     *  \brief Returns the diagnostic as a single formatted line.
     *
     *  Produces the same text as dumpErrorInfo(QTextStream*), including the trailing newline.
     *
     *  \return The formatted line; see dumpErrorInfo(QTextStream*) for the layout.
     */
    QString makeErrorInfoString() const;

    /*!
     *  \brief Returns the shared dummy diagnostic.
     *
     *  The entry has dummy level / stage / code, position (0, 0), and the message
     *  "Invalid syntax error entry." finSyntaxErrorList returns it when an out-of-range entry is
     *  requested, so callers always receive a usable object.
     *
     *  \return Reference to a static entry; valid for the lifetime of the program.
     *
     *  \see finSyntaxErrorList::getEntryAt
     */
    static const finSyntaxError &dummySyntaxError();

    /*!
     *  \brief Returns the display name of a severity.
     *
     *  \param level  The severity to look up.
     *  \return A translated name such as "Debug" or "Warning"; "Dummy" for unrecognized values.
     */
    static QString getLevelName(Level level);

    /*!
     *  \brief Returns the display name of a pipeline stage.
     *
     *  \param stage  The stage to look up.
     *  \return A translated name such as "Lex" or "Execute"; "Dummy" for unrecognized values.
     */
    static QString getStageName(Stage stage);

    /*!
     *  \brief Returns the display name of a category code.
     *
     *  \param code  The code to look up.
     *  \return A translated name such as "UnexpectedToken" or "UnknownSymbol"; "Dummy" for
     *          unrecognized values.
     */
    static QString getCodeName(Code code);

    /*!
     *  \brief Appends an execution-positioned diagnostic to a list.
     *
     *  Convenience wrapper for error producers: creates an entry whose source position is taken
     *  from \a lexnode, stores \a errinfo as its message, and appends the entry to \a errlist.
     *  Severity, stage, and code keep their dummy values; prefer finSyntaxErrorList::appendEntry()
     *  when those fields matter.
     *
     *  Throws finException with EC_NULL_POINTER when \a lexnode or \a errlist is \c nullptr.
     *
     *  \param lexnode  Token supplying the source row and column.
     *  \param errlist  List that receives the new entry.
     *  \param errinfo  Free-form message stored in the entry.
     */
    static void appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                     const QString &errinfo);
};

#endif // FINSYNTAXERROR_H

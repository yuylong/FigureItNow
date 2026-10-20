/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finSyntaxErrorList.h
 *  \brief Declaration of the diagnostic list collected across the FIN-7 pipeline.
 *
 *  This header defines finSyntaxErrorList, the ordered collection of finSyntaxError entries raised
 *  while a script is read, parsed, and executed, together with the severity filter and dump sink
 *  used to render those entries for the GUI and command-line front ends.
 */

#ifndef FINSYNTAXERRORLIST_H
#define FINSYNTAXERRORLIST_H

#include <QList>
#include <QString>
#include <QTextStream>

#include "finSyntaxErrorDump.h"
#include "finLexNode.h"
#include "finSyntaxError.h"


/*! \class finSyntaxErrorList
 *  \brief Ordered collection of diagnostics with severity filtering and pluggable output.
 *
 *  finSyntaxErrorList accumulates the finSyntaxError entries produced by the lexer, the parser, and
 *  the execution machine, and turns them into output for a finSyntaxErrorDump sink. It is the
 *  single consumer-facing error pipe of a script run: finExecMachine merges the parse-stage
 *  diagnostics of the syntax tree into its own list before executing, so callers see compile and
 *  runtime problems together in one place.
 *
 *  Entries are kept by value in the order they were appended. Two output modes are supported:
 *
 *  - on demand, through dumpList(), which renders every entry whose level reaches the requested
 *    starting level;
 *  - in realtime, through the flag set by setIsRealtimeDump(), which renders each qualifying entry
 *    at the moment it is appended.
 *
 *  Both modes share one starting level (default finSyntaxError::LV_WARNING) and one dumper, and
 *  both forward through finSyntaxError::dumpErrorInfo(), so a line rendered in realtime is
 *  identical to the same line rendered by a full dump. The dumper is a borrowed pointer; the list
 *  never takes ownership and never deletes it.
 *
 *  The list is a value type with no destructor logic: finSyntaxError entries are stored by value,
 *  so destroying the list releases them.
 *
 *  \see finSyntaxError
 *  \see finSyntaxErrorDump
 *  \see finExecMachine
 */
class finSyntaxErrorList
{
protected:
    QList<finSyntaxError> _list;          //!< Entries in append order; copied by value.
    bool _rtDump;                         //!< Whether qualifying entries are dumped as they are appended.
    finSyntaxErrorDump *_dumper;          //!< Borrowed dump sink; \c nullptr disables all output.
    finSyntaxError::Level _startingLevel; //!< Minimum severity used by the parameterless overloads.

public:
    /*! \brief Constructs an empty list with no dumper and a warning starting level. */
    finSyntaxErrorList();

    /*! \name Entry Queries
     *  \brief Count and inspect the stored diagnostics.
     */
    ///@{

    /*!
     *  \brief Returns the number of entries whose severity is at least \a level.
     *
     *  Levels are ordered by increasing severity (see finSyntaxError::Level), so this counts the
     *  entries that reach a given threshold.
     *
     *  \param level  Minimum severity to count from.
     *  \return The matching entry count; 0 when none qualify.
     */
    int getEntryCountFromLevel(finSyntaxError::Level level) const;

    /*!
     *  \brief Returns the number of entries whose severity reaches the configured starting level.
     *
     *  Equivalent to getEntryCountFromLevel(getDumpStartingLevel()).
     */
    int getEntryCountFromLevel() const;

    /*!
     *  \brief Returns the number of entries whose severity equals \a level exactly.
     *
     *  \param level  The severity to match.
     *  \return The matching entry count; 0 when none match.
     */
    int getEntryCountAtLevel(finSyntaxError::Level level) const;

    /*! \brief Returns the total number of stored entries, including those below the starting level. */
    int getEntryCount() const;

    /*!
     *  \brief Returns the entry at \a index.
     *
     *  Out-of-range indices are tolerated rather than rejected: the shared dummy entry is returned
     *  instead of throwing, so UI code that indexes while repainting always receives a usable value.
     *  Use getEntryCount() to decide whether an index is valid, or removeEntryAt() when an invalid
     *  index must be reported.
     *
     *  \param index  Zero-based position of the entry.
     *  \return A reference to the stored entry, or finSyntaxError::dummySyntaxError() when \a index
     *          is out of range.
     *
     *  \see finSyntaxError::dummySyntaxError
     */
    const finSyntaxError &getEntryAt(int index) const;
    ///@}

    /*! \name Dump Configuration
     *  \brief Inspect and change the sink, the realtime mode, and the starting level.
     */
    ///@{

    /*! \brief Returns whether entries are rendered as they are appended. */
    bool isRealtimeDump() const;

    /*!
     *  \brief Returns the currently installed dump sink.
     *
     *  The pointer is borrowed, not owned by the list.
     *
     *  \return The installed dumper, or \c nullptr when none was set.
     */
    finSyntaxErrorDump *getDumper() const;

    /*! \brief Returns the minimum severity used by the parameterless dump and count overloads. */
    finSyntaxError::Level getDumpStartingLevel() const;

    /*!
     *  \brief Enables or disables realtime dumping.
     *
     *  When enabled, an entry is rendered through the installed dumper as soon as it is appended and
     *  its severity reaches the starting level. Has no effect until a dumper is set.
     *
     *  \param rtdump  \c true to render entries on append, \c false to render them only on demand.
     */
    void setIsRealtimeDump(bool rtdump);

    /*!
     *  \brief Installs the sink used by realtime and on-demand output.
     *
     *  The list does not take ownership; \a dumper must outlive the list. Passing \c nullptr disables
     *  output: the realtime flag may stay set, but nothing is rendered until a valid sink is
     *  installed.
     *
     *  \param dumper  Sink to forward formatted entries to, or \c nullptr to detach the current one.
     */
    void setDumper(finSyntaxErrorDump *dumper);

    /*!
     *  \brief Sets the minimum severity used by the parameterless overloads.
     *
     *  Affects dumpList(), getEntryCountFromLevel(), and the realtime filter, but never removes
     *  entries already stored.
     *
     *  \param level  Minimum severity to render and count from.
     */
    void setDumpStartingLevel(finSyntaxError::Level level);
    ///@}

    /*! \name Entry Modification
     *  \brief Add and remove diagnostics.
     */
    ///@{

    /*!
     *  \brief Builds an entry from its fields and a source token, then appends it.
     *
     *  The row and column are taken from \a lexnode; when it is \c nullptr the position is reset to
     *  (0, 0) instead of being left stale. The entry's Code keeps its dummy value, so prefer
     *  finSyntaxError::setErrorInfo() on an entry built by the caller when a category matters.
     *
     *  Honours the realtime dump policy.
     *
     *  \param level     Severity of the new entry.
     *  \param stage     Pipeline stage reporting the problem.
     *  \param lexnode   Token supplying the source position; may be \c nullptr.
     *  \param info      Human-readable message stored in the entry.
     *
     *  \see appendEntry(const finSyntaxError &)
     */
    void appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
                     finLexNode *lexnode, QString info);

    /*!
     *  \brief Appends an already-constructed error entry, preserving all of its fields.
     *
     *  Used when forwarding entries parsed elsewhere, e.g. copying the syntax tree's diagnostics into
     *  the execution machine's list. Honours the realtime dump policy.
     *
     *  \param entry  The entry to copy into the list.
     */
    void appendEntry(const finSyntaxError &entry);

    /*!
     *  \brief Removes the entry at \a index.
     *
     *  Throws finException with EC_INVALID_PARAM when \a index is out of range.
     *
     *  \param index  Zero-based position of the entry to remove.
     */
    void removeEntryAt(int index);

    /*!
     *  \brief Removes every stored entry.
     *
     *  The dump sink, realtime flag, and starting level are left unchanged, so the list is ready to
     *  collect the diagnostics of the next run.
     */
    void clearAllErrorList();
    ///@}

    /*! \name Dump Output
     *  \brief Render the stored entries through a dump sink.
     */
    ///@{

    /*!
     *  \brief Renders every entry whose severity reaches \a startinglevel to \a dumper.
     *
     *  Entries are rendered in append order, each as one formatted line. Does nothing when no entry
     *  reaches the requested level.
     *
     *  Throws finException with EC_NULL_POINTER when \a dumper is \c nullptr.
     *
     *  \param dumper         Sink to forward the formatted entries to.
     *  \param startinglevel  Minimum severity to render from.
     */
    void dumpList(finSyntaxErrorDump *dumper, finSyntaxError::Level startinglevel) const;

    /*!
     *  \brief Renders every entry whose severity reaches \a startinglevel to the installed dumper.
     *
     *  Throws finException with EC_NULL_POINTER when no dumper is installed.
     *
     *  \param startinglevel  Minimum severity to render from.
     */
    void dumpList(finSyntaxError::Level startinglevel) const;

    /*!
     *  \brief Renders every entry whose severity reaches the configured starting level.
     *
     *  Uses the dumper and the starting level installed through setDumper() and
     *  setDumpStartingLevel(); equivalent to dumpList(getDumper(), getDumpStartingLevel()).
     *
     *  Throws finException with EC_NULL_POINTER when no dumper is installed.
     */
    void dumpList() const;
    ///@}
};

#endif // FINSYNTAXERRORLIST_H

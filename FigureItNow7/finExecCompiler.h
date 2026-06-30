/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 5th, 2017
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finExecCompiler.h
 *  \brief Declarations of the execution-layer wrapper around finSyntaxReader.
 *
 *  This header defines finExecCompiler, a small adapter used by the execution layer to feed script
 *  text into finSyntaxReader and retrieve the resulting finSyntaxTree. When compilation cannot produce
 *  a normal tree, the wrapper builds a fallback tree that carries syntax-error information.
 */
#ifndef FINEXECCOMPILER_H
#define FINEXECCOMPILER_H

#include "finErrorCode.h"
#include "finSyntaxReader.h"
#include "finSyntaxTree.h"


/*! \class finExecCompiler
 *  \brief Execution-layer wrapper that compiles script text into a syntax tree.
 *
 *  finExecCompiler stores the script text currently being compiled and owns a finSyntaxReader used to
 *  run the lexing and parsing pipeline. The execution machine uses this class as a narrow entry point:
 *  setScriptCode() installs the source text, and compile() drives finSyntaxReader until it produces a
 *  finSyntaxTree or an error tree containing a diagnostic message.
 *
 *  \see finSyntaxReader
 *  \see finSyntaxTree
 */
class finExecCompiler
{
private:
    QString _scriptCode;         //!< Script source text to be compiled on the next compile() call.
    finSyntaxReader _synReader;  //!< Wrapped parser pipeline used to build the syntax tree.

public:
    /*!
     *  \brief Constructs a compiler with empty script text.
     *
     *  Call setScriptCode() before invoking compile().
     */
    finExecCompiler();

    /*!
     *  \brief Destroys the compiler and stops any in-progress syntax-reader session.
     */
    ~finExecCompiler();

    /*!
     *  \brief Returns the script text currently stored in the compiler.
     *
     *  \return The source text that will be compiled by the next compile() call.
     */
    QString getScriptCode() const;

    /*!
     *  \brief Replaces the script text to be compiled.
     *
     *  The previous script text is overwritten immediately; no compilation is triggered until compile()
     *  is called.
     *
     *  \param script  Source text to store.
     *  \return \c finErrorKits::EC_SUCCESS after the new script text has been stored.
     */
    finErrorCode setScriptCode(const QString &script);

    /*!
     *  \brief Compiles the stored script text into a syntax tree.
     *
     *  This method resets any in-progress reader state, feeds the stored script text into finSyntaxReader,
     *  and drives token reading until parsing finishes. The returned tree is heap-allocated and owned by
     *  the caller. If parsing fails or no script text has been installed, the method returns a heap-
     *  allocated fallback tree containing syntax-error information instead of returning \c nullptr whenever
     *  possible.
     *
     *  \return A heap-allocated syntax tree representing the current script, or an error tree if
     *          compilation failed. Returns \c nullptr only if allocating the fallback tree fails.
     */
    finSyntaxTree *compile();

private:
    /*!
     *  \brief Builds a fallback syntax tree containing one syntax error.
     *
     *  \param errstr  Error message to attach to the tree. If empty, an empty tree is returned.
     *  \return A heap-allocated syntax tree, or \c nullptr if allocation fails.
     */
    finSyntaxTree *buildErrorTree(const QString &errstr) const;
};

#endif // FINEXECCOMPILER_H

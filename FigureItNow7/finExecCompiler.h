/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 5th, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finExecCompiler.h
 *  \brief The compiler wrap for the execution module.
 *
 * This file declares a class wrap the whole procedure of the syntax module, and give the interface to upper layer
 * code as a compiler. It give an interface that easily transform the script codes into a syntax tree. The syntax tree
 * then will be used by execution machine to generate the figure object list and other outputs.
 */
#ifndef FINEXECCOMPILER_H
#define FINEXECCOMPILER_H

#include "finErrorCode.h"
#include "finSyntaxReader.h"
#include "finSyntaxTree.h"


/*! \class finExecCompiler
 *  \brief The compiler wrap for the execution module.
 *
 * This class gives an interface that transform a piece of script code into syntax tree. It is used by the execution
 * machine, and wrap all the procedure of the lower layered lexical and syntax modules.
 */
class finExecCompiler
{
private:
    QString _scriptCode;         //!< The script code.
    finSyntaxReader _synReader;  //!< The wrapped syntax reader.

public:
    /*! \fn finExecCompiler
     *  \brief The constructor of class finExecCompiler.
     *
     * It constructs an instance with empty script code. The script code is needed to be set before compiling process
     * is invoked.
     */
    finExecCompiler();

    /*! \fn ~finExecCompiler
     *  \brief the destructor of class finExecCompiler.
     */
    ~finExecCompiler();

    /*! \fn getScriptCode
     *  \brief Read the string value of the script code installed in the class instance.
     *
     * \return The script code.
     */
    QString getScriptCode() const;

    /*! \fn setScriptCode
     *  \brief Setup a new script code.
     *
     * The old script code will be overwriten with the new one.
     *
     * \param script The script code.
     * \return The operation is successful or not.
     */
    finErrorCode setScriptCode(const QString &script);

    /*! \fn compile
     *  \brief The main procedure of the compiler.
     *
     * This method transform the script code installed in the instance into a syntax tree. The syntax tree in heap
     * space is generated after the invocation, and is needed to be deleted to avoid the memory leap.
     *
     * \return The syntax tree corresponding to the script code. If the compiling failed, returns NULL.
     */
    finSyntaxTree *compile();

private:
    finSyntaxTree *buildErrorTree(const QString &errstr) const;
};

#endif // FINEXECCOMPILER_H

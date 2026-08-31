/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finExecOperatorClac.h
 *  \brief The calculator of operators defined in FIN-7 system.
 *
 * This file declares the calculations of all the operators defined in FIN-7 system. When the execution a script and
 * it encounters an operator, the actural computation is taken by the methods defined in this file.
 */

#ifndef FINEXECOPERARTORCLAC_H
#define FINEXECOPERARTORCLAC_H

#include <QList>

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finExecVariable.h"


/*! \class finExecOperartorCalc
 *  \brief Evaluates FIN-script operators for the execution machine.
 *
 *  This utility class dispatches parsed operators to their implementation and creates the standard
 *  write-protected values used for logical results.
 */
class finExecOperartorCalc
{
public:
    /*! \brief Constructs an operator calculator. */
    finExecOperartorCalc();

    /*! \brief Reads a runtime variable using FIN-script logical conversion rules.
     *
     *  \param var  Variable to inspect. A null variable evaluates to false.
     *  \return The converted logical value.
     */
    static bool varLogicValue(finExecVariable *var);

    /*! \brief Creates a standard write-protected logical result variable.
     *
     *  \param blval  Logical value to store.
     *  \return A newly allocated result variable, or null if allocation fails.
     */
    static finExecVariable *buildStdLogicVar(bool blval);

    /*! \brief Evaluates an operator with its already evaluated operands.
     *
     *  \param optype   Parsed operator to execute.
     *  \param oprands  Operand list. Ownership may be transferred for bracket and comma operations.
     *  \param retval   Receives the operator result.
     *  \return An execution status code.
     */
    static finErrorCode execOpCalc(finLexOperatorType optype,
                                   QList<finExecVariable *> *oprands, finExecVariable **retval);
};

#endif // FINEXECOPERARTORCLAC_H

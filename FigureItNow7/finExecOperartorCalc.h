/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
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


class finExecOperartorCalc
{
public:
    finExecOperartorCalc();

    static bool varLogicValue(finExecVariable *var);
    static finExecVariable *buildStdLogicVar(bool blval);

    static finErrorCode execOpCalc(finLexOperatorType optype,
                                   QList<finExecVariable *> *oprands, finExecVariable **retval);
};

#endif // FINEXECOPERARTORCLAC_H

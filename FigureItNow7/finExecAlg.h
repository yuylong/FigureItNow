/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#ifndef FINEXECALG_H
#define FINEXECALG_H

#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finExecVariable.h"

class finExecAlg
{
public:
    finExecAlg();

    static finErrorCode stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static finErrorCode stringListToStrArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static finErrorCode stringListToArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static finErrorCode numArrayVarToStringList(finExecVariable *invar, QStringList *strlist);
    static finErrorCode arrayVarToStringList(finExecVariable *invar, QStringList *strlist);

    static finErrorCode csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar);
    static finErrorCode csStringToStrArrayVar(const QString &csstr, finExecVariable *outvar);
    static finErrorCode csStringToArrayVar(const QString &csstr, finExecVariable *outvar);
    static QString numArrayVarToCsString(finExecVariable *invar);
    static QString arrayVarToCsString(finExecVariable *invar);

    static finErrorCode numArrayVarToList(finExecVariable *invar, QList<double> *list);
    static finErrorCode numMatVarToList(finExecVariable *invar, QList<QList<double>> *list);
    static finErrorCode listToNumArrayVar(const QList<double> &list, finExecVariable *outvar);
    static finErrorCode listToNumMatVar(const QList<QList<double>> &list, finExecVariable *outvar);

    static finErrorCode listArrayAdd(const QList<double> &inlist1, const QList<double> &inlist2,
                                     QList<double> *outlist);

    static finErrorCode varArrayAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
};

#endif // FINEXECALG_H

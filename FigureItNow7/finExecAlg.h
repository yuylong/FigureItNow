/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#ifndef FINEXECALG_H
#define FINEXECALG_H

#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finExecVariable.h"

/*! \class finExecAlg
 *  \brief Algorithm function collection for FIN-Script execution.
 *
 * Class finExecAlg defines all the complicated algorithm implementations to support the computation in FIN-script
 * execution, which includes data type translation, string parsing, analytic geometry computing, and so on.
 */
class finExecAlg
{
public:
    finExecAlg();

    static void stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static void stringListToStrArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static void stringListToArrayVar(const QStringList &strlist, finExecVariable *outvar);
    static void numArrayVarToStringList(finExecVariable *invar, QStringList *strlist);
    static void arrayVarToStringList(finExecVariable *invar, QStringList *strlist);

    static void csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar);
    static void csStringToStrArrayVar(const QString &csstr, finExecVariable *outvar);
    static void csStringToArrayVar(const QString &csstr, finExecVariable *outvar);
    static QString numArrayVarToCsString(finExecVariable *invar);
    static QString arrayVarToCsString(finExecVariable *invar);

    static void numArrayVarToList(finExecVariable *invar, QList<double> *list);
    static void numMatVarToList(finExecVariable *invar, QList<QList<double>> *list);
    static void listToNumArrayVar(const QList<double> &list, finExecVariable *outvar);
    static void listToNumMatVar(const QList<QList<double>> &list, finExecVariable *outvar);

    static void listMatrixToArray(const QList< QList<double> > &inlist, QList<double> *outlist);
    static void varMatrixToArray(finExecVariable *invar, finExecVariable *outvar);
    static void varArrayCut(finExecVariable *invar, int from, int to, finExecVariable *outvar);
    static void varArrayJoin(const QList<finExecVariable *> &invarlist, finExecVariable *outvar);

    static void listArrayNeg(const QList<double> &inlist, QList<double> *outlist);
    static void listArrayAdd(const QList<double> &inlist1, const QList<double> &inlist2,
                             QList<double> *outlist);
    static void listArraySub(const QList<double> &inlist1, const QList<double> &inlist2,
                             QList<double> *outlist);
    static void listArraySum(const QList<double> &inlist, double *outval);
    static void listArrayAvg(const QList<double> &inlist, double *outval);

    static void listVectorNorm(const QList<double> &inlist, double *outval);
    static void listVectorNorm1(const QList<double> &inlist, double *outval);
    static void listVectorNormP(const QList<double> &inlist, double p, double *outval);
    static finErrorCode listVectorNormInf(const QList<double> &inlist, double *outval);
    static finErrorCode listVectorNormalize(const QList<double> &inlist, QList<double> *outlist);
    static finErrorCode listVectorDot(const QList<double> &inlist1, const QList<double> &inlist2, double *outval);

    static finErrorCode varArrayNeg(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varArrayAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    static finErrorCode varArraySub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    static finErrorCode varArraySum(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varArrayAvg(finExecVariable *invar, finExecVariable *outvar);

    static finErrorCode varVectorNorm(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varVectorNorm1(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varVectorNormP(finExecVariable *invar, finExecVariable *pvar, finExecVariable *outvar);
    static finErrorCode varVectorNormInf(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varVectorNormalize(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varVectorDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);

    static finErrorCode listMatTranspose(const QList<QList<double>> &inlist, QList<QList<double>> *outlist);
    static finErrorCode listMatAdd(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                   QList<QList<double>> *outlist);
    static finErrorCode listMatSub(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                   QList<QList<double>> *outlist);
    static finErrorCode listMatDot(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                   QList<QList<double>> *outlist);

    static finErrorCode varMatTranspose(finExecVariable *invar, finExecVariable *outvar);
    static finErrorCode varMatAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    static finErrorCode varMatSub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    static finErrorCode varMatDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
};

#endif // FINEXECALG_H

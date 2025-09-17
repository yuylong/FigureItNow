/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#include <qmath.h>

#include "finExecAlg.h"

finExecAlg::finExecAlg()
{
    /* Do Nothing because you should not call this constructor. */
    return;
}

void finExecAlg::stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() ) {
        finWarning << "Convert an empty string list to number array variable.";
        return;
    }

    outvar->preallocArrayLength(strlist.length());
    foreach ( QString stritem, strlist ) {
        finExecVariable *colvar = outvar->getVariableItemAt(curcol);
        bool numok = false;
        double valitem = stritem.toDouble(&numok);
        if ( !numok )
            valitem = 0.0;

        colvar->setType(finExecVariable::TP_NUMERIC);
        colvar->setNumericValue(valitem);
        curcol++;
    }
}

void finExecAlg::stringListToStrArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() ) {
        finWarning << "Convert an empty string list to text array variable.";
        return;
    }

    outvar->preallocArrayLength(strlist.length());
    foreach ( QString stritem, strlist ) {
        finExecVariable *colvar = outvar->getVariableItemAt(curcol);
        colvar->setType(finExecVariable::TP_STRING);
        colvar->setStringValue(stritem);
        curcol++;
    }
}

void finExecAlg::stringListToArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() ) {
        finWarning << "Convert an empty string list to array variable.";
        return;
    }

    outvar->preallocArrayLength(strlist.length());
    foreach ( QString stritem, strlist ) {
        finExecVariable *colvar = outvar->getVariableItemAt(curcol);
        bool numok = false;
        double valitem = stritem.toDouble(&numok);

        if ( numok ) {
            colvar->setType(finExecVariable::TP_NUMERIC);
            colvar->setNumericValue(valitem);
        } else if ( stritem.isEmpty() ) {
            colvar->setType(finExecVariable::TP_NULL);
        } else {
            colvar->setType(finExecVariable::TP_STRING);
            colvar->setStringValue(stritem);
        }
        curcol++;
    }
}

static inline void _appendNumVarToStrList(finExecVariable *invar, QStringList *strlist)
{
    if ( invar == nullptr || invar->getType() == finExecVariable::TP_NULL ) {
        strlist->append(QString());
        finWarning << "Append a null typed variable to string list.";
    } else if ( invar->getType() == finExecVariable::TP_STRING || invar->getType() == finExecVariable::TP_IMAGE ) {
        strlist->append(QString("0"));
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        strlist->append(QString::number(invar->getNumericValue()));
    }
}

void finExecAlg::numArrayVarToStringList(finExecVariable *invar, QStringList *strlist)
{
    strlist->clear();
    if ( invar == nullptr || invar->getType() != finExecVariable::TP_ARRAY ) {
        _appendNumVarToStrList(invar, strlist);
        return;
    }

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendNumVarToStrList(itemvar, strlist);
    }
}

static inline void _appendVarToStrList(finExecVariable *invar, QStringList *strlist)
{
    if ( invar == nullptr || invar->getType() == finExecVariable::TP_NULL ||
         invar->getType() == finExecVariable::TP_IMAGE ) {
        strlist->append(QString());
        finWarning << "Append a null/image typed variable to string list.";
    } else if ( invar->getType() == finExecVariable::TP_STRING ) {
        strlist->append(invar->getStringValue());
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        strlist->append(QString::number(invar->getNumericValue()));
    }
}

void finExecAlg::arrayVarToStringList(finExecVariable *invar, QStringList *strlist)
{
    strlist->clear();
    if ( invar == nullptr || invar->getType() != finExecVariable::TP_ARRAY ) {
        _appendVarToStrList(invar, strlist);
        return;
    }

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendVarToStrList(itemvar, strlist);
    }
}

void finExecAlg::csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return;
    }

    stringListToNumArrayVar(trimstr.split(','), outvar);
}

void finExecAlg::csStringToStrArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return;
    }

    stringListToStrArrayVar(trimstr.split(','), outvar);
}

void finExecAlg::csStringToArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return;
    }

    stringListToArrayVar(trimstr.split(','), outvar);
}

QString finExecAlg::numArrayVarToCsString(finExecVariable *invar)
{
    QStringList strlist;
    numArrayVarToStringList(invar, &strlist);
    return strlist.join(',');
}

QString finExecAlg::arrayVarToCsString(finExecVariable *invar)
{
    QStringList strlist;
    arrayVarToStringList(invar, &strlist);
    return strlist.join(',');
}

static inline void _appendVarToNumList(finExecVariable *invar, QList<double> *list)
{
    if ( invar == nullptr || invar->getType() == finExecVariable::TP_NULL ) {
        return;
    } else if ( invar->getType() == finExecVariable::TP_STRING || invar->getType() == finExecVariable::TP_IMAGE ) {
        list->append(0.0);
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        list->append(invar->getNumericValue());
    }
}

void finExecAlg::numArrayVarToList(finExecVariable *invar, QList<double> *list)
{
    list->clear();
    if ( invar == nullptr || invar->getType() != finExecVariable::TP_ARRAY ) {
        _appendVarToNumList(invar, list);
        return;
    }

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendVarToNumList(itemvar, list);
    }
}

void finExecAlg::numMatVarToList(finExecVariable *invar, QList<QList<double>> *list)
{
    list->clear();
    if ( invar == nullptr || invar->getType() != finExecVariable::TP_ARRAY ) {
        QList<double> tmplist;
        _appendVarToNumList(invar, &tmplist);
        list->append(tmplist);
        return;
    }

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        QList<double> itemlist;
        numArrayVarToList(itemvar, &itemlist);
        list->append(itemlist);
    }
}

void finExecAlg::listToNumArrayVar(const QList<double> &list, finExecVariable *outvar)
{
    outvar->setType(finExecVariable::TP_ARRAY);
    outvar->preallocArrayLength(list.length());

    int idx = 0;
    foreach ( double val, list ) {
        finExecVariable *itemvar = outvar->getVariableItemAt(idx);
        itemvar->setType(finExecVariable::TP_NUMERIC);
        itemvar->setNumericValue(val);
        idx++;
    }
}

void finExecAlg::listToNumMatVar(const QList<QList<double>> &list, finExecVariable *outvar)
{
    outvar->setType(finExecVariable::TP_ARRAY);
    outvar->preallocArrayLength(list.length());

    int idx = 0;
    foreach ( const QList<double> &sublist, list ) {
        finExecVariable *itemvar = outvar->getVariableItemAt(idx);
        listToNumArrayVar(sublist, itemvar);
        idx++;
    }
}

void finExecAlg::listMatrixToArray(const QList< QList<double> > &inlist, QList<double> *outlist)
{
    if ( outlist == nullptr ) {
        finThrow(finErrorCode::EC_NULL_POINTER, "Output list is null.");
    }

    outlist->clear();
    foreach ( const QList<double> &sublist, inlist ) {
        foreach ( double item, sublist ) {
            outlist->append(item);
        }
    }
}

static void _appendSubVar(finExecVariable *outvar, finExecVariable *subvar, int startpos, int *endpos)
{
    if ( subvar == nullptr || subvar->getType() == finExecVariable::TP_NULL ) {
        *endpos = startpos;
        return;
    }

    if ( subvar->getType() != finExecVariable::TP_ARRAY ) {
        outvar->preallocArrayLength(startpos + 1);
        finExecVariable *outitem = outvar->getVariableItemAt(startpos);
        if ( outitem == nullptr ) {
            finThrow(finErrorKits::EC_OUT_OF_MEMORY, QString("Cannot get output item at index %1.").arg(startpos));
        }

        outitem->copyVariable(subvar);
        *endpos = startpos + 1;
    }

    int sublen = subvar->getArrayLength();
    outvar->preallocArrayLength(startpos + sublen);
    for ( int i = 0; i < sublen; i++ ) {
        finExecVariable *initem = subvar->getVariableItemAt(i);
        finExecVariable *outitem = outvar->getVariableItemAt(startpos + i);
        if ( initem == nullptr || outitem == nullptr ) {
            finThrow(finErrorKits::EC_OUT_OF_MEMORY,
                     QString("Cannot get input item (%1) or output item (%2).").arg(i).arg(startpos + i));
        }

        outitem->copyVariable(initem);
    }
    *endpos = startpos + sublen;
}

void finExecAlg::varMatrixToArray(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr ) {
        finThrow(finErrorCode::EC_NULL_POINTER, "Input or output variable is null.");
    }

    int pos = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( invar->getType() != finExecVariable::TP_ARRAY ) {
        _appendSubVar(outvar, invar, 0, &pos);
        return;
    }

    int rowcnt = invar->getArrayLength();
    for ( int rowidx = 0; rowidx < rowcnt; rowidx++ ) {
        finExecVariable *inrowvar = invar->getVariableItemAt(rowidx);

        _appendSubVar(outvar, inrowvar, pos, &pos);
    }
}

void finExecAlg::varArrayCut(finExecVariable *invar, int from, int to, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr ) {
        finThrow(finErrorCode::EC_NULL_POINTER, "Input or output variable is null.");
    }
    if ( invar->getType() != finExecVariable::TP_ARRAY ) {
        finThrow(finErrorKits::EC_INVALID_PARAM,
                 QString("Array operation performs on non-array variable (%1)").arg(invar->getType()));
    }

    int inlen = invar->getArrayLength();
    from = (from < 0 ? 0 : from);
    to = ((to > inlen || to < 0) ? inlen : to);
    int realfrom = (from < to ? from : to);
    int realto = (from < to ? to : from);

    outvar->setType(finExecVariable::TP_ARRAY);
    outvar->preallocArrayLength(realto - realfrom);
    for (int outi = 0, ini = from; ini < to; outi++, ini++) {
        finExecVariable *initem = invar->getVariableItemAt(ini);
        finExecVariable *outitem = outvar->getVariableItemAt(outi);
        if ( initem == nullptr || outitem == nullptr )
            finThrow(finErrorKits::EC_OUT_OF_MEMORY,
                     QString("Cannot get array item at index %1-%2.").arg(ini).arg(outi));

        outitem->copyVariable(initem);
    }
}

void finExecAlg::varArrayJoin(const QList<finExecVariable *> &invarlist, finExecVariable *outvar)
{
    if ( outvar == nullptr ) {
        finThrow(finErrorCode::EC_NULL_POINTER, "The output variable is null.");
    }

    int pos = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    foreach ( finExecVariable *invar, invarlist ) {
        _appendSubVar(outvar, invar, pos, &pos);
    }
}

void finExecAlg::listArrayNeg(const QList<double> &inlist, QList<double> *outlist)
{
    if ( outlist == nullptr ) {
        finThrow(finErrorCode::EC_NULL_POINTER, "The output variable is null.");
    }
    outlist->clear();
    foreach ( double val, inlist) {
        outlist->append(-val);
    }
}

void finExecAlg::listArrayAdd(const QList<double> &inlist1, const QList<double> &inlist2,
                              QList<double> *outlist)
{
    if ( outlist == nullptr ) {
        finThrow(finErrorKits::EC_NULL_POINTER, "The output variable is null.");
    }
    if ( inlist1.length() != inlist2.length() ) {
        finThrow(finErrorKits::EC_INVALID_PARAM, "Input lists must have the same length.");
    }

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        double val1 = inlist1.at(i);
        double val2 = inlist2.at(i);
        outlist->append(val1 + val2);
    }
}

void finExecAlg::listArraySub(const QList<double> &inlist1, const QList<double> &inlist2,
                              QList<double> *outlist)
{
    if ( outlist == nullptr ) {
        finThrow(finErrorKits::EC_NULL_POINTER, "The output variable is null.");
    }
    if ( inlist1.length() != inlist2.length() ) {
        finThrow(finErrorKits::EC_INVALID_PARAM, "Input lists must have the same length.");
    }

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        double val1 = inlist1.at(i);
        double val2 = inlist2.at(i);
        outlist->append(val1 - val2);
    }
}

void finExecAlg::listArraySum(const QList<double> &inlist, double *outval)
{
    if ( outval == nullptr ) {
        finThrow(finErrorKits::EC_NULL_POINTER, "The output variable is null.");
    }

    double sumval = 0.0;
    foreach ( double val, inlist ) {
        sumval += val;
    }
    *outval = sumval;
}

void finExecAlg::listArrayAvg(const QList<double> &inlist, double *outval)
{
    if ( outval == nullptr ) {
        finThrow(finErrorKits::EC_NULL_POINTER, "The output variable is null.");
    }
    if ( inlist.length() == 0 ) {
        *outval = 0.0;
        finWarning << "The input list is empty.";
        return;
    }

    double sumval = 0.0;
    listArraySum(inlist, &sumval);

    *outval = sumval / inlist.length();
}

finErrorCode finExecAlg::listVectorNorm(const QList<double> &inlist, double *outval)
{
    if ( outval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    double norm2 = 0.0;
    foreach ( double val, inlist ) {
        norm2 += val * val;
    }
    *outval = sqrt(norm2);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorNorm1(const QList<double> &inlist, double *outval)
{
    if ( outval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    double norm = 0.0;
    foreach ( double val, inlist ) {
        norm += fabs(val);
    }
    *outval = norm;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorNormP(const QList<double> &inlist, double p, double *outval)
{
    if ( outval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    double normp = 0.0;
    foreach ( double val, inlist ) {
        normp += pow(fabs(val), p);
    }
    *outval = pow(normp, 1.0 / p);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorNormInf(const QList<double> &inlist, double *outval)
{
    if ( outval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    double normmax = 0.0;
    foreach ( double val, inlist ) {
        if ( fabs(val) > normmax )
            normmax = fabs(val);
    }
    *outval = normmax;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorNormalize(const QList<double> &inlist, QList<double> *outlist)
{
    if ( outlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    double norm = 1.0;
    finErrorCode errcode = listVectorNorm(inlist, &norm);
    if ( finErrorKits::isErrorResult(errcode) )
        norm = 1.0;

    outlist->clear();
    foreach ( double val, inlist ) {
        outlist->append(val/norm);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorDot(const QList<double> &inlist1, const QList<double> &inlist2, double *outval)
{
    if ( outval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int len = inlist1.length();
    if ( len > inlist2.length() )
        len = inlist2.length();

    double retval = 0.0;
    for ( int i = 0; i < len; i++ ) {
        double val1 = inlist1.at(i);
        double val2 = inlist2.at(i);
        retval += val1 * val2;
    }
    *outval = retval;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayNeg(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist, outlist;
    numArrayVarToList(invar, &inlist);
    listArrayNeg(inlist, &outlist);
    listToNumArrayVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varlen1 = 0, varlen2 = 0;
    bool isary1 = invar1->isNumericArray(&varlen1);
    bool isary2 = invar2->isNumericArray(&varlen2);
    if ( !isary1 || !isary2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varlen1 != varlen2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist1, inlist2, outlist;
    numArrayVarToList(invar1, &inlist1);
    numArrayVarToList(invar2, &inlist2);

    listArrayAdd(inlist1, inlist2, &outlist);
    listToNumArrayVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArraySub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varlen1 = 0, varlen2 = 0;
    bool isary1 = invar1->isNumericArray(&varlen1);
    bool isary2 = invar2->isNumericArray(&varlen2);
    if ( !isary1 || !isary2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varlen1 != varlen2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist1, inlist2, outlist;
    numArrayVarToList(invar1, &inlist1);
    numArrayVarToList(invar2, &inlist2);

    listArraySub(inlist1, inlist2, &outlist);
    listToNumArrayVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArraySum(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    listArraySum(inlist, &outval);

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayAvg(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    listArrayAvg(inlist, &outval);

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNorm(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    finErrorCode errcode = listVectorNorm(inlist, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNorm1(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    finErrorCode errcode = listVectorNorm1(inlist, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNormP(finExecVariable *invar, finExecVariable *pvar, finExecVariable *outvar)
{
    if ( invar == nullptr || pvar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;
    if ( pvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);
    double p = pvar->getNumericValue();

    double outval = 0.0;
    finErrorCode errcode = listVectorNormP(inlist, p, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNormInf(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    finErrorCode errcode = listVectorNormInf(inlist, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNormalize(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist, outlist;
    numArrayVarToList(invar, &inlist);

    finErrorCode errcode = listVectorNormalize(inlist, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    listToNumArrayVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varlen1 = 0, varlen2 = 0;
    bool isary1 = invar1->isNumericArray(&varlen1);
    bool isary2 = invar2->isNumericArray(&varlen2);
    if ( !isary1 || !isary2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varlen1 != varlen2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist1, inlist2;
    numArrayVarToList(invar1, &inlist1);
    numArrayVarToList(invar2, &inlist2);

    double outval = 0.0;
    finErrorCode errcode = listVectorDot(inlist1, inlist2, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatTranspose(const QList<QList<double>> &inlist, QList<QList<double>> *outlist)
{
    if ( outlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int outrow = 0, outcol = inlist.length();
    foreach ( const QList<double> &sublist, inlist ) {
        if ( sublist.length() > outrow )
            outrow = sublist.length();
    }

    outlist->clear();
    for ( int r = 0; r < outrow; r++ ) {
        QList<double> outsublist;
        for ( int c = 0; c < outcol; c++ ) {
            double val = 0.0;
            const QList<double> &insublist = inlist.at(c);
            if ( r < insublist.length() )
                val = insublist.at(r);

            outsublist.append(val);
        }
        outlist->append(outsublist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatAdd(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                    QList<QList<double>> *outlist)
{
    if ( outlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        const QList<double> &insublist1 = inlist1.at(i);
        const QList<double> &insublist2 = inlist2.at(i);
        QList<double> outsublist;

        listArrayAdd(insublist1, insublist2, &outsublist);
        outlist->append(outsublist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatSub(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                    QList<QList<double>> *outlist)
{
    if ( outlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        const QList<double> &insublist1 = inlist1.at(i);
        const QList<double> &insublist2 = inlist2.at(i);
        QList<double> outsublist;

        listArraySub(insublist1, insublist2, &outsublist);
        outlist->append(outsublist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatDot(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                    QList<QList<double>> *outlist)
{
    if ( outlist == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finErrorCode errcode;
    QList<QList<double>> inlist2t;
    errcode = listMatTranspose(inlist2, &inlist2t);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outlist->clear();
    foreach ( const QList<double> &in1row, inlist1 ) {
        QList<double> outrow;
        foreach ( const QList<double> &in2col, inlist2t ) {
            int len = in1row.length();
            if ( len != in2col.length() )
                return finErrorKits::EC_INVALID_PARAM;

            double itemval = 0.0;
            for ( int i = 0; i < len; i++ ) {
                double in1item = in1row.at(i);
                double in2item = in2col.at(i);
                itemval += in1item * in2item;
            }
            outrow.append(itemval);
        }
        outlist->append(outrow);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varMatTranspose(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericMatrix() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<QList<double>> inlist, outlist;
    numMatVarToList(invar, &inlist);

    finErrorCode errcode = listMatTranspose(inlist, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    listToNumMatVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varMatAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varrow1 = 0, varcol1 = 0, varrow2 = 0, varcol2 = 0;
    bool ismat1 = invar1->isNumericMatrix(&varrow1, &varcol1);
    bool ismat2 = invar2->isNumericMatrix(&varrow2, &varcol2);
    if ( !ismat1 || !ismat2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varrow1 != varrow2 || varcol1 != varcol2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList<QList<double>> inlist1, inlist2, outlist;
    numMatVarToList(invar1, &inlist1);
    numMatVarToList(invar2, &inlist2);

    finErrorCode errcode = listMatAdd(inlist1, inlist2, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    listToNumMatVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varMatSub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varrow1 = 0, varcol1 = 0, varrow2 = 0, varcol2 = 0;
    bool ismat1 = invar1->isNumericMatrix(&varrow1, &varcol1);
    bool ismat2 = invar2->isNumericMatrix(&varrow2, &varcol2);
    if ( !ismat1 || !ismat2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varrow1 != varrow2 || varcol1 != varcol2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList<QList<double>> inlist1, inlist2, outlist;
    numMatVarToList(invar1, &inlist1);
    numMatVarToList(invar2, &inlist2);

    finErrorCode errcode = listMatSub(inlist1, inlist2, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    listToNumMatVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varMatDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == nullptr || invar2 == nullptr || outvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    int varrow1 = 0, varcol1 = 0, varrow2 = 0, varcol2 = 0;
    bool ismat1 = invar1->isNumericMatrix(&varrow1, &varcol1);
    bool ismat2 = invar2->isNumericMatrix(&varrow2, &varcol2);
    if ( !ismat1 || !ismat2 )
        return finErrorKits::EC_INVALID_PARAM;
    if ( varcol1 != varrow2 )
        return finErrorKits::EC_INVALID_PARAM;

    QList< QList<double> > inlist1, inlist2, outlist;
    numMatVarToList(invar1, &inlist1);
    numMatVarToList(invar2, &inlist2);

    finErrorCode errcode = listMatDot(inlist1, inlist2, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    listToNumMatVar(outlist, outvar);
    return finErrorKits::EC_SUCCESS;
}

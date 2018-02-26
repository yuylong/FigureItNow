/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#include <qmath.h>

#include "finExecAlg.h"

finExecAlg::finExecAlg()
{
    /* Do Nothing because you should not call this constructor. */
    return;
}

finErrorCode finExecAlg::stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() )
        return finErrorKits::EC_REACH_BOTTOM;

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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::stringListToStrArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() )
        return finErrorKits::EC_REACH_BOTTOM;

    outvar->preallocArrayLength(strlist.length());
    foreach ( QString stritem, strlist ) {
        finExecVariable *colvar = outvar->getVariableItemAt(curcol);
        colvar->setType(finExecVariable::TP_STRING);
        colvar->setStringValue(stritem);
        curcol++;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::stringListToArrayVar(const QStringList &strlist, finExecVariable *outvar)
{
    int curcol = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( strlist.isEmpty() )
        return finErrorKits::EC_REACH_BOTTOM;

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
    return finErrorKits::EC_SUCCESS;
}

static inline finErrorCode _appendNumVarToStrList(finExecVariable *invar, QStringList *strlist)
{
    if ( invar == NULL || invar->getType() == finExecVariable::TP_NULL ) {
        strlist->append(QString());
        return finErrorKits::EC_NORMAL_WARN;
    } else if ( invar->getType() == finExecVariable::TP_STRING || invar->getType() == finExecVariable::TP_IMAGE ) {
        strlist->append(QString("0"));
        return finErrorKits::EC_NORMAL_WARN;
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        strlist->append(QString::number(invar->getNumericValue()));
        return finErrorKits::EC_SUCCESS;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }
}

finErrorCode finExecAlg::numArrayVarToStringList(finExecVariable *invar, QStringList *strlist)
{
    strlist->clear();
    if ( invar == NULL || invar->getType() != finExecVariable::TP_ARRAY )
        return _appendNumVarToStrList(invar, strlist);

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendNumVarToStrList(itemvar, strlist);
    }
    return finErrorKits::EC_SUCCESS;
}

static inline finErrorCode _appendVarToStrList(finExecVariable *invar, QStringList *strlist)
{
    if ( invar == NULL || invar->getType() == finExecVariable::TP_NULL ||
         invar->getType() == finExecVariable::TP_IMAGE ) {
        strlist->append(QString());
        return finErrorKits::EC_NORMAL_WARN;
    } else if ( invar->getType() == finExecVariable::TP_STRING ) {
        strlist->append(invar->getStringValue());
        return finErrorKits::EC_SUCCESS;
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        strlist->append(QString::number(invar->getNumericValue()));
        return finErrorKits::EC_SUCCESS;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }
}

finErrorCode finExecAlg::arrayVarToStringList(finExecVariable *invar, QStringList *strlist)
{
    strlist->clear();
    if ( invar == NULL || invar->getType() != finExecVariable::TP_ARRAY )
        return _appendVarToStrList(invar, strlist);

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendVarToStrList(itemvar, strlist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return finErrorKits::EC_REACH_BOTTOM;
    }

    return stringListToNumArrayVar(trimstr.split(','), outvar);
}

finErrorCode finExecAlg::csStringToStrArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return finErrorKits::EC_REACH_BOTTOM;
    }

    return stringListToStrArrayVar(trimstr.split(','), outvar);
}

finErrorCode finExecAlg::csStringToArrayVar(const QString &csstr, finExecVariable *outvar)
{
    QString trimstr = csstr.trimmed();
    if ( trimstr.isEmpty() ) {
        outvar->setType(finExecVariable::TP_ARRAY);
        return finErrorKits::EC_REACH_BOTTOM;
    }

    return stringListToArrayVar(trimstr.split(','), outvar);
}

QString finExecAlg::numArrayVarToCsString(finExecVariable *invar)
{
    QStringList strlist;
    finErrorCode errcode = numArrayVarToStringList(invar, &strlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return QString();

    return strlist.join(',');
}

QString finExecAlg::arrayVarToCsString(finExecVariable *invar)
{
    QStringList strlist;
    finErrorCode errcode = arrayVarToStringList(invar, &strlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return QString();

    return strlist.join(',');
}

static inline finErrorCode _appendVarToNumList(finExecVariable *invar, QList<double> *list)
{
    if ( invar == NULL || invar->getType() == finExecVariable::TP_NULL ) {
        return finErrorKits::EC_NORMAL_WARN;
    } else if ( invar->getType() == finExecVariable::TP_STRING || invar->getType() == finExecVariable::TP_IMAGE ) {
        list->append(0.0);
        return finErrorKits::EC_NORMAL_WARN;
    } else if ( invar->getType() == finExecVariable::TP_NUMERIC ) {
        list->append(invar->getNumericValue());
        return finErrorKits::EC_SUCCESS;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }
}

finErrorCode finExecAlg::numArrayVarToList(finExecVariable *invar, QList<double> *list)
{
    list->clear();
    if ( invar == NULL || invar->getType() != finExecVariable::TP_ARRAY )
        return _appendVarToNumList(invar, list);

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        _appendVarToNumList(itemvar, list);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::numMatVarToList(finExecVariable *invar, QList<QList<double>> *list)
{
    list->clear();
    if ( invar == NULL || invar->getType() != finExecVariable::TP_ARRAY ) {
        QList<double> tmplist;
        finErrorCode errcode = _appendVarToNumList(invar, &tmplist);
        list->append(tmplist);
        return errcode;
    }

    int itemcnt = invar->getArrayLength();
    for ( int i = 0; i < itemcnt; i++ ) {
        finExecVariable *itemvar = invar->getVariableItemAt(i);
        QList<double> itemlist;
        numArrayVarToList(itemvar, &itemlist);
        list->append(itemlist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listToNumArrayVar(const QList<double> &list, finExecVariable *outvar)
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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listToNumMatVar(const QList<QList<double>> &list, finExecVariable *outvar)
{
    outvar->setType(finExecVariable::TP_ARRAY);
    outvar->preallocArrayLength(list.length());

    int idx = 0;
    foreach ( const QList<double> &sublist, list ) {
        finExecVariable *itemvar = outvar->getVariableItemAt(idx);
        listToNumArrayVar(sublist, itemvar);
        idx++;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatrixToArray(const QList< QList<double> > &inlist, QList<double> *outlist)
{
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    outlist->clear();
    foreach ( const QList<double> &sublist, inlist ) {
        foreach ( double item, sublist ) {
            outlist->append(item);
        }
    }
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _appendSubVar(finExecVariable *outvar, finExecVariable *subvar, int startpos, int *endpos)
{
    if ( subvar == NULL || subvar->getType() == finExecVariable::TP_NULL ) {
        *endpos = startpos;
        return finErrorKits::EC_NORMAL_WARN;
    }

    if ( subvar->getType() != finExecVariable::TP_ARRAY ) {
        outvar->preallocArrayLength(startpos + 1);
        finExecVariable *outitem = outvar->getVariableItemAt(startpos);
        if ( outitem == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        outitem->copyVariable(subvar);
        *endpos = startpos + 1;
        return finErrorKits::EC_SUCCESS;
    }

    finErrorCode errcode;
    int sublen = subvar->getArrayLength();
    outvar->preallocArrayLength(startpos + sublen);
    for ( int i = 0; i < sublen; i++ ) {
        finExecVariable *initem = subvar->getVariableItemAt(i);
        finExecVariable *outitem = outvar->getVariableItemAt(startpos + i);
        if ( initem == NULL || outitem == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        errcode = outitem->copyVariable(initem);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    *endpos = startpos + sublen;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varMatrixToArray(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;

    int pos = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    if ( invar->getType() != finExecVariable::TP_ARRAY )
        return _appendSubVar(outvar, invar, 0, &pos);

    finErrorCode errcode;
    int rowcnt = invar->getArrayLength();
    for ( int rowidx = 0; rowidx < rowcnt; rowidx++ ) {
        finExecVariable *inrowvar = invar->getVariableItemAt(rowidx);

        errcode = _appendSubVar(outvar, inrowvar, pos, &pos);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayCut(finExecVariable *invar, int from, int to, finExecVariable *outvar)
{
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( invar->getType() != finExecVariable::TP_ARRAY )
        return finErrorKits::EC_INVALID_PARAM;

    int inlen = invar->getArrayLength();
    from = (from < 0 ? 0 : from);
    to = ((to > inlen || to < 0) ? inlen : to);
    int realfrom = (from < to ? from : to);
    int realto = (from < to ? to : from);

    finErrorCode errcode;
    outvar->setType(finExecVariable::TP_ARRAY);
    outvar->preallocArrayLength(realto - realfrom);
    for (int outi = 0, ini = from; ini < to; outi++, ini++) {
        finExecVariable *initem = invar->getVariableItemAt(ini);
        finExecVariable *outitem = outvar->getVariableItemAt(outi);
        if ( initem == NULL || outitem == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        errcode = outitem->copyVariable(initem);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayJoin(const QList<finExecVariable *> &invarlist, finExecVariable *outvar)
{
    if ( outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;

    int pos = 0;
    outvar->setType(finExecVariable::TP_ARRAY);
    foreach ( finExecVariable *invar, invarlist ) {
        finErrorCode errcode = _appendSubVar(outvar, invar, pos, &pos);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listArrayNeg(const QList<double> &inlist, QList<double> *outlist)
{
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    outlist->clear();
    foreach ( double val, inlist) {
        outlist->append(-val);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listArrayAdd(const QList<double> &inlist1, const QList<double> &inlist2,
                                      QList<double> *outlist)
{
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        double val1 = inlist1.at(i);
        double val2 = inlist2.at(i);
        outlist->append(val1 + val2);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listArraySub(const QList<double> &inlist1, const QList<double> &inlist2,
                                      QList<double> *outlist)
{
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        double val1 = inlist1.at(i);
        double val2 = inlist2.at(i);
        outlist->append(val1 - val2);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listArraySum(const QList<double> &inlist, double *outval)
{
    if ( outval == NULL )
        return finErrorKits::EC_NULL_POINTER;

    double sumval = 0.0;
    foreach ( double val, inlist ) {
        sumval += val;
    }
    *outval = sumval;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listArrayAvg(const QList<double> &inlist, double *outval)
{
    if ( outval == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist.length() == 0 ) {
        *outval = 0.0;
        return finErrorKits::EC_NORMAL_WARN;
    }

    double sumval = 0.0;
    finErrorCode errcode = listArraySum(inlist, &sumval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    *outval = sumval / inlist.length();
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listVectorNorm(const QList<double> &inlist, double *outval)
{
    if ( outval == NULL )
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
    if ( outval == NULL )
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
    if ( outval == NULL )
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
    if ( outval == NULL )
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
    if ( outlist == NULL )
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
    if ( outval == NULL )
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
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist, outlist;
    numArrayVarToList(invar, &inlist);

    finErrorCode errcode = listArrayNeg(inlist, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return listToNumArrayVar(outlist, outvar);
}

finErrorCode finExecAlg::varArrayAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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

    finErrorCode errcode = listArrayAdd(inlist1, inlist2, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return listToNumArrayVar(outlist, outvar);
}

finErrorCode finExecAlg::varArraySub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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

    finErrorCode errcode = listArraySub(inlist1, inlist2, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return listToNumArrayVar(outlist, outvar);
}

finErrorCode finExecAlg::varArraySum(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    finErrorCode errcode = listArraySum(inlist, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varArrayAvg(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist;
    numArrayVarToList(invar, &inlist);

    double outval = 0.0;
    finErrorCode errcode = listArrayAvg(inlist, &outval);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    outvar->setType(finExecVariable::TP_NUMERIC);
    outvar->setNumericValue(outval);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::varVectorNorm(finExecVariable *invar, finExecVariable *outvar)
{
    if ( invar == NULL || outvar == NULL )
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
    if ( invar == NULL || outvar == NULL )
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
    if ( invar == NULL || pvar == NULL || outvar == NULL )
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
    if ( invar == NULL || outvar == NULL )
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
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericArray() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<double> inlist, outlist;
    numArrayVarToList(invar, &inlist);

    finErrorCode errcode = listVectorNormalize(inlist, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return listToNumArrayVar(outlist, outvar);
}

finErrorCode finExecAlg::varVectorDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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
    if ( outlist == NULL )
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
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        const QList<double> &insublist1 = inlist1.at(i);
        const QList<double> &insublist2 = inlist2.at(i);
        QList<double> outsublist;

        finErrorCode errcode = listArrayAdd(insublist1, insublist2, &outsublist);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;

        outlist->append(outsublist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatSub(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                    QList<QList<double>> *outlist)
{
    if ( outlist == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( inlist1.length() != inlist2.length() )
        return finErrorKits::EC_INVALID_PARAM;

    int len = inlist1.length();
    outlist->clear();
    for ( int i = 0; i < len; i++ ) {
        const QList<double> &insublist1 = inlist1.at(i);
        const QList<double> &insublist2 = inlist2.at(i);
        QList<double> outsublist;

        finErrorCode errcode = listArraySub(insublist1, insublist2, &outsublist);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;

        outlist->append(outsublist);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecAlg::listMatDot(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                                    QList<QList<double>> *outlist)
{
    if ( outlist == NULL )
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
    if ( invar == NULL || outvar == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( !invar->isNumericMatrix() )
        return finErrorKits::EC_INVALID_PARAM;

    QList<QList<double>> inlist, outlist;
    numMatVarToList(invar, &inlist);

    finErrorCode errcode = listMatTranspose(inlist, &outlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return listToNumMatVar(outlist, outvar);
}

finErrorCode finExecAlg::varMatAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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

    return listToNumMatVar(outlist, outvar);
}

finErrorCode finExecAlg::varMatSub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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

    return listToNumMatVar(outlist, outvar);
}

finErrorCode finExecAlg::varMatDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar)
{
    if ( invar1 == NULL || invar2 == NULL || outvar == NULL )
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

    return listToNumMatVar(outlist, outvar);
}

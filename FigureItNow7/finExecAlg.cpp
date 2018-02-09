/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

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
{\
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

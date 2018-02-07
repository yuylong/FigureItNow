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

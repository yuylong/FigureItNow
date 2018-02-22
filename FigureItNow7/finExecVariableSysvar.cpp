/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#include <qmath.h>
#include <QString>

#include "finExecVariable.h"
#include "finExecEnvironment.h"


static finExecVariable *_sysvar_nil();
static finExecVariable *_sysvar_pi();
static finExecVariable *_sysvar_e();
static finExecVariable *_sysvar_pythagoras_const();
static finExecVariable *_sysvar_theodorus_const();
static finExecVariable *_sysvar_euler_mascheroni_const();
static finExecVariable *_sysvar_golden_ratio();
static finExecVariable *_sysvar_meissel_mertens_const();
static finExecVariable *_sysvar_bernsteins_const();
static finExecVariable *_sysvar_gauss_kuzmin_wirsing_const();
static finExecVariable *_sysvar_hafner_sarnak_mccurley_const();
static finExecVariable *_sysvar_landaus_const();

typedef finExecVariable *(*_finSysvarGencall)();

_finSysvarGencall _finSysvarGencallList[] = {
    _sysvar_nil,
    _sysvar_pi,
    _sysvar_e,
    _sysvar_pythagoras_const,
    _sysvar_theodorus_const,
    _sysvar_euler_mascheroni_const,
    _sysvar_golden_ratio,
    _sysvar_meissel_mertens_const,
    _sysvar_bernsteins_const,
    _sysvar_gauss_kuzmin_wirsing_const,
    _sysvar_hafner_sarnak_mccurley_const,
    _sysvar_landaus_const,
    NULL
};

finErrorCode
finExecVariable::installSystemVariables(finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int success = 0, failed = 0;

    for ( int i = 0; _finSysvarGencallList[i] != NULL; i++ ) {
        finExecVariable *curvar = _finSysvarGencallList[i]();
        if ( curvar == NULL )
            goto item_bad;

        errcode = rootenv->addVariable(curvar);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        success++;
        continue;

item_bad:
        if ( curvar != NULL )
            delete curvar;
        failed++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorKits::EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorKits::EC_NOT_FOUND;
    else if ( failed != 0 )
        return finErrorKits::EC_NORMAL_WARN;
    else
        return finErrorKits::EC_SUCCESS;
}

static finExecVariable *_sysvar_nil()
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(QString("NIL"));
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::TP_NULL);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static inline finExecVariable *
_sysvar_gen_num_var(const QString &name, double val)
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(name);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::TP_NUMERIC);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setNumericValue(val);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static finExecVariable *_sysvar_pi()
{
    return _sysvar_gen_num_var(QString("PI"), M_PI);
}

static finExecVariable *_sysvar_e()
{
    return _sysvar_gen_num_var(QString("E"), M_E);
}

static finExecVariable *_sysvar_pythagoras_const()
{
    return _sysvar_gen_num_var(QString("PYTHAGORAS_CONST"), sqrt(2.0));
}

static finExecVariable *_sysvar_theodorus_const()
{
    return _sysvar_gen_num_var(QString("THEODORUS_CONST"), sqrt(3.0));
}

static finExecVariable *_sysvar_euler_mascheroni_const()
{
    return _sysvar_gen_num_var(QString("EULER_MASCHERONI_CONST"), 0.57721566490153286060651209008240243);
}

static finExecVariable *_sysvar_golden_ratio()
{
    return _sysvar_gen_num_var(QString("GOLDEN_RATIO"), 1.61803398874989484820458683436563811);
}

static finExecVariable *_sysvar_meissel_mertens_const()
{
    return _sysvar_gen_num_var(QString("MEISSEL_MERTENS_CONST"), 0.26149721284764278375542683860869585);
}

static finExecVariable *_sysvar_bernsteins_const()
{
    return _sysvar_gen_num_var(QString("BERNSTEINS_CONST"), 0.28016949902386913303);
}

static finExecVariable *_sysvar_gauss_kuzmin_wirsing_const()
{
    return _sysvar_gen_num_var(QString("GAUSS_KUZMIN_WIRSING_CONST"), 0.30366300289873265859744812190155623);
}

static finExecVariable *_sysvar_hafner_sarnak_mccurley_const()
{
    return _sysvar_gen_num_var(QString("HAFNER_SARNAK_MCCURLEY_CONST"), 0.35323637185499598454351655043268201);
}

static finExecVariable *_sysvar_landaus_const()
{
    return _sysvar_gen_num_var(QString("LANDAUS_CONST"), 0.5);
}

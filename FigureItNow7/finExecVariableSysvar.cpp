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

typedef finExecVariable *(*_finSysvarGencall)();

_finSysvarGencall _finSysvarGencallList[] = {
    _sysvar_nil,
    _sysvar_pi,
    _sysvar_e,
    _sysvar_pythagoras_const,
    _sysvar_theodorus_const,
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

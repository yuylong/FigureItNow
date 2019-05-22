/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecFlowControl.h"


static QString _defFuncCtg("Mathematics");

static finErrorCode _sysfunc_abs(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *num, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    num = finExecVariable::transLinkTarget(env->findVariable("num"));
    if ( num == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( num->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(fabs(num->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_abs = {
    /*._funcName     =*/ QString("abs"),
    /*._paramCsvList =*/ QString("num"),
    /*._funcCall     =*/ _sysfunc_abs,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("abs (num)"),
    /*._description  =*/ QString("Get the abstractive value of given number."),
};

static finErrorCode _sysfunc_sig(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *numvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    numvar = finExecVariable::transLinkTarget(env->findVariable("num"));
    if ( numvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( numvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double num = numvar->getNumericValue();
    double ret = 0.0;
    if ( num < 0.0 )
        ret = -1.0;
    else if ( num > 0.0 )
        ret = 1.0;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(ret);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_sig = {
    /*._funcName     =*/ QString("sig"),
    /*._paramCsvList =*/ QString("num"),
    /*._funcCall     =*/ _sysfunc_sig,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("sig (num)"),
    /*._description  =*/ QString("Get the signature of given number."),
};

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(sin(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_sin = {
    /*._funcName     =*/ QString("sin"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_sin,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("sin (rad)"),
    /*._description  =*/ QString("The mathematical function -- sin."),
};

static finErrorCode _sysfunc_cos(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(cos(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_cos = {
    /*._funcName     =*/ QString("cos"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_cos,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("cos (rad)"),
    /*._description  =*/ QString("The mathematical function -- cos."),
};

static finErrorCode _sysfunc_tan(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(tan(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_tan = {
    /*._funcName     =*/ QString("tan"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_tan,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("tan (rad)"),
    /*._description  =*/ QString("The mathematical function -- tan. An alias of tg."),
};

static struct finExecSysFuncRegItem _funcRegItem_tg = {
    /*._funcName     =*/ QString("tg"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_tan,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("tg (rad)"),
    /*._description  =*/ QString("The mathematical function -- tg. An alias of tan."),
};

static finErrorCode _sysfunc_cot(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(1.0 / tan(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_cot = {
    /*._funcName     =*/ QString("cot"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_cot,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("cot (rad)"),
    /*._description  =*/ QString("The mathematical function -- cot. An alias of ctg."),
};

static struct finExecSysFuncRegItem _funcRegItem_ctg = {
    /*._funcName     =*/ QString("ctg"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_cot,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ctg (rad)"),
    /*._description  =*/ QString("The mathematical function -- ctg. An alias of cot."),
};

static finErrorCode _sysfunc_sec(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(1.0 / cos(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_sec = {
    /*._funcName     =*/ QString("sec"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_sec,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("sec (rad)"),
    /*._description  =*/ QString("The mathematical function -- sec."),
};

static finErrorCode _sysfunc_csc(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(1.0 / sin(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_csc = {
    /*._funcName     =*/ QString("csc"),
    /*._paramCsvList =*/ QString("rad"),
    /*._funcCall     =*/ _sysfunc_csc,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("csc (rad)"),
    /*._description  =*/ QString("The mathematical function -- csc."),
};

static finErrorCode _sysfunc_ln(finExecFunction *self, finExecEnvironment *env,
                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *base, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    base = finExecVariable::transLinkTarget(env->findVariable("base"));
    if ( base == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( base->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(log(base->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_ln = {
    /*._funcName     =*/ QString("ln"),
    /*._paramCsvList =*/ QString("base"),
    /*._funcCall     =*/ _sysfunc_ln,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ln (base)"),
    /*._description  =*/ QString("The mathematical function -- ln."),
};

static finErrorCode _sysfunc_log(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *index, *base, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    index = finExecVariable::transLinkTarget(env->findVariable("index"));
    base = finExecVariable::transLinkTarget(env->findVariable("base"));
    if ( index == nullptr || base == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( index->getType() != finExecVariable::TP_NUMERIC ||
         base->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(log(base->getNumericValue()) / log(index->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_log = {
    /*._funcName     =*/ QString("log"),
    /*._paramCsvList =*/ QString("idx,base"),
    /*._funcCall     =*/ _sysfunc_log,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("log (idx, base)"),
    /*._description  =*/ QString("The mathematical function -- log."),
};

static finErrorCode _sysfunc_linear(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *avar, *bvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(a * x + b);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_linear = {
    /*._funcName     =*/ QString("linear"),
    /*._paramCsvList =*/ QString("x,a,b"),
    /*._funcCall     =*/ _sysfunc_linear,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("linear (x, a, b)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Linear equation."),
};

static finErrorCode _sysfunc_quadratic(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *avar, *bvar, *cvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    cvar = finExecVariable::transLinkTarget(env->findVariable("c"));
    if ( xvar == nullptr || avar == nullptr || bvar == nullptr || cvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC ||
         cvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double c = cvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue((a * x + b) * x + c);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_quadratic = {
    /*._funcName     =*/ QString("quadratic"),
    /*._paramCsvList =*/ QString("x,a,b,c"),
    /*._funcCall     =*/ _sysfunc_quadratic,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("quadratic (x, a, b, c)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Quadratic equation."),
};

static finErrorCode _sysfunc_frequency_curve(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    if ( xvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(sqrt(2 * M_PI * exp(-x * x / 2)));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_frequency_curve = {
    /*._funcName     =*/ QString("frequency_curve"),
    /*._paramCsvList =*/ QString("x"),
    /*._funcCall     =*/ _sysfunc_frequency_curve,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("frequency_curve (x)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Frequency curve."),
};

static finErrorCode _sysfunc_parm_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == nullptr || rvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         rvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(r * cos(t));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(r * sin(t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_circle = {
    /*._funcName     =*/ QString("parm_circle"),
    /*._paramCsvList =*/ QString("t,r"),
    /*._funcCall     =*/ _sysfunc_parm_circle,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_circle (t, r)"),
    /*._description  =*/ QString("The 2D parametric equation -- Circle."),
};

static finErrorCode _sysfunc_parm_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( tvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(a * cos(t));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(b * sin(t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_ellipse = {
    /*._funcName     =*/ QString("parm_ellipse"),
    /*._paramCsvList =*/ QString("t,a,b"),
    /*._funcCall     =*/ _sysfunc_parm_ellipse,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_ellipse (t, a, b)"),
    /*._description  =*/ QString("The 2D parametric equation -- Ellipse."),
};

static finErrorCode _sysfunc_parm_general_ellipse(finExecFunction *self, finExecEnvironment *env,
                                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *xcvar, *ycvar, *phivar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    xcvar = finExecVariable::transLinkTarget(env->findVariable("xc"));
    ycvar = finExecVariable::transLinkTarget(env->findVariable("yc"));
    phivar = finExecVariable::transLinkTarget(env->findVariable("phi"));
    if ( tvar == nullptr || avar == nullptr || bvar == nullptr || xcvar == nullptr || ycvar == nullptr || phivar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC ||
         xcvar->getType() != finExecVariable::TP_NUMERIC ||
         ycvar->getType() != finExecVariable::TP_NUMERIC ||
         phivar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double xc = xcvar->getNumericValue();
    double yc = ycvar->getNumericValue();
    double phi = phivar->getNumericValue();

    double cost = cos(t), sint = sin(t);
    double cosphi = cos(phi), sinphi = sin(phi);

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(xc + a * cost * cosphi - b * sint * sinphi);
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(yc + a * cost * sinphi + b * sint * cosphi);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_general_ellipse = {
    /*._funcName     =*/ QString("parm_general_ellipse"),
    /*._paramCsvList =*/ QString("t,a,b,xc,yc,phi"),
    /*._funcCall     =*/ _sysfunc_parm_general_ellipse,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_general_ellipse (t, a, b, xc, yc, phi)"),
    /*._description  =*/ QString("The 2D parametric equation -- General Ellipse."),
};

static finErrorCode _sysfunc_parm_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *hvar, *kvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    hvar = finExecVariable::transLinkTarget(env->findVariable("h"));
    kvar = finExecVariable::transLinkTarget(env->findVariable("k"));
    if ( tvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC ||
         (hvar != nullptr && hvar->getType() != finExecVariable::TP_NULL &&
                          hvar->getType() != finExecVariable::TP_NUMERIC) ||
         (kvar != nullptr && kvar->getType() != finExecVariable::TP_NULL &&
                          kvar->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double h = ((hvar != nullptr && hvar->getType() == finExecVariable::TP_NUMERIC) ?
                hvar->getNumericValue() : 0.0);
    double k = ((kvar != nullptr && kvar->getType() == finExecVariable::TP_NUMERIC) ?
                kvar->getNumericValue() : 0.0);

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(a / cos(t) + h);
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(b * tan(t) + k);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_hyperbola = {
    /*._funcName     =*/ QString("parm_hyperbola"),
    /*._paramCsvList =*/ QString("t,a,b,h,k"),
    /*._funcCall     =*/ _sysfunc_parm_hyperbola,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_hyperbola (t, a, b, h, k)"),
    /*._description  =*/ QString("The 2D parametric equation -- Hyperbola."),
};

static finErrorCode _sysfunc_parm_hyperbola_rational(finExecFunction *self, finExecEnvironment *env,
                                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *hvar, *kvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    hvar = finExecVariable::transLinkTarget(env->findVariable("h"));
    kvar = finExecVariable::transLinkTarget(env->findVariable("k"));
    if ( tvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC ||
         (hvar != nullptr && hvar->getType() != finExecVariable::TP_NULL &&
                          hvar->getType() != finExecVariable::TP_NUMERIC) ||
         (kvar != nullptr && kvar->getType() != finExecVariable::TP_NULL &&
                          kvar->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double h = ((hvar != nullptr && hvar->getType() == finExecVariable::TP_NUMERIC) ?
                hvar->getNumericValue() : 0.0);
    double k = ((kvar != nullptr && kvar->getType() == finExecVariable::TP_NUMERIC) ?
                kvar->getNumericValue() : 0.0);
    double t2 = t * t;
    double ratio = 1.0 - t2;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(a * (1 + t2) / ratio + h);
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(b * 2.0 * t / ratio + k);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_hyperbola_rational = {
    /*._funcName     =*/ QString("parm_hyperbola_rational"),
    /*._paramCsvList =*/ QString("t,a,b,h,k"),
    /*._funcCall     =*/ _sysfunc_parm_hyperbola_rational,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_hyperbola_rational (t, a, b, h, k)"),
    /*._description  =*/ QString("The 2D parametric equation -- Rational Hyperbola."),
};

static finErrorCode _sysfunc_parm_astroid(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    if ( tvar == nullptr || avar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    double cost = cos(t), sint = sin(t);
    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(a * cost * cost * cost);
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(a * sint * sint * sint);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_astroid = {
    /*._funcName     =*/ QString("parm_astroid"),
    /*._paramCsvList =*/ QString("t,a"),
    /*._funcCall     =*/ _sysfunc_parm_astroid,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_astroid (t, a)"),
    /*._description  =*/ QString("The 2D parametric equation -- Astroid."),
};

static finErrorCode _sysfunc_parm_lissajous(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *kxvar, *kyvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    kxvar = finExecVariable::transLinkTarget(env->findVariable("kx"));
    kyvar = finExecVariable::transLinkTarget(env->findVariable("ky"));
    if ( tvar == nullptr || avar == nullptr || bvar == nullptr || kxvar == nullptr || kyvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC ||
         kxvar->getType() != finExecVariable::TP_NUMERIC ||
         kyvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double kx = kxvar->getNumericValue();
    double ky = kyvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(a * cos(kx * t));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(b * sin(ky * t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_lissajous = {
    /*._funcName     =*/ QString("parm_lissajous"),
    /*._paramCsvList =*/ QString("t,a,b,kx,ky"),
    /*._funcCall     =*/ _sysfunc_parm_lissajous,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_lissajous (t, a, b, kx, ky)"),
    /*._description  =*/ QString("The 2D parametric equation -- Lissajous Curve."),
};

static finErrorCode _sysfunc_parm_involute(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == nullptr || rvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         rvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(r * (cos(t) + t * sin(t)));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(r * (sin(t) - t * cos(t)));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_involute = {
    /*._funcName     =*/ QString("parm_involute"),
    /*._paramCsvList =*/ QString("t,r"),
    /*._funcCall     =*/ _sysfunc_parm_involute,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_involute (t, r)"),
    /*._description  =*/ QString("The 2D parametric equation -- Involute Curve."),
};

static finErrorCode _sysfunc_parm_cycloid(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == nullptr || rvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         rvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(r * (t - sin(t)));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(r * (1 - cos(t)));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_cycloid = {
    /*._funcName     =*/ QString("parm_cycloid"),
    /*._paramCsvList =*/ QString("t,r"),
    /*._funcCall     =*/ _sysfunc_parm_cycloid,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_cycloid (t, r)"),
    /*._description  =*/ QString("The 2D parametric equation -- Cycloid."),
};

static finErrorCode _sysfunc_parm_hypotrochoid(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *Rvar, *rvar, *dvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    Rvar = finExecVariable::transLinkTarget(env->findVariable("R"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    dvar = finExecVariable::transLinkTarget(env->findVariable("d"));
    if ( tvar == nullptr || Rvar == nullptr || rvar == nullptr || dvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC ||
         Rvar->getType() != finExecVariable::TP_NUMERIC ||
         rvar->getType() != finExecVariable::TP_NUMERIC ||
         dvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double R = Rvar->getNumericValue();
    double r = rvar->getNumericValue();
    double d = dvar->getNumericValue();
    double diff = R -r;
    double ratio = t * (R - r) / r;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(diff * cos(t) + d * cos(ratio));
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(diff * sin(t) - d * sin(ratio));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_hypotrochoid = {
    /*._funcName     =*/ QString("parm_hypotrochoid"),
    /*._paramCsvList =*/ QString("t,R,r,d"),
    /*._funcCall     =*/ _sysfunc_parm_hypotrochoid,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_hypotrochoid (t, R, r, d)"),
    /*._description  =*/ QString("The 2D parametric equation -- Hypotrochoid."),
};

static finErrorCode _sysfunc_parm_butterfly(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *retvar, *retitemx, *retitemy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    if ( tvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double sint = sin(t), cost = cos(t);
    double sintd12 = sin(t / 12.0);
    double sin2td12 = sintd12 * sintd12;
    double ratio = pow(M_E, cost) - 2 * cos(4.0 * t) - sin2td12 * sin2td12 * sintd12;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == nullptr || retitemy == nullptr ) {
        delete retvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::TP_NUMERIC);
    retitemx->setNumericValue(sint * ratio);
    retitemy->setType(finExecVariable::TP_NUMERIC);
    retitemy->setNumericValue(cost * ratio);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_parm_butterfly = {
    /*._funcName     =*/ QString("parm_butterfly"),
    /*._paramCsvList =*/ QString("t"),
    /*._funcCall     =*/ _sysfunc_parm_butterfly,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("parm_butterfly (t)"),
    /*._description  =*/ QString("The 2D parametric equation -- Butterfly Curve."),
};

static finErrorCode _sysfunc_eq2d_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *rvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( xvar == nullptr || yvar == nullptr || rvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC ||
         yvar->getType() != finExecVariable::TP_NUMERIC ||
         rvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(x * x + y * y - r * r);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_eq2d_circle = {
    /*._funcName     =*/ QString("eq2d_circle"),
    /*._paramCsvList =*/ QString("x,y,r"),
    /*._funcCall     =*/ _sysfunc_eq2d_circle,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("eq2d_circle (x, y, r)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Circle."),
};

static finErrorCode _sysfunc_eq2d_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *avar, *bvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == nullptr || yvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC ||
         yvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double a2 = a * a, b2 = b * b;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(b2 * x * x + a2 * y * y - a2 * b2);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_eq2d_ellipse = {
    /*._funcName     =*/ QString("eq2d_ellipse"),
    /*._paramCsvList =*/ QString("x,y,a,b"),
    /*._funcCall     =*/ _sysfunc_eq2d_ellipse,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("eq2d_ellipse (x, y, a, b)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Ellipse."),
};

static finErrorCode _sysfunc_eq2d_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *avar, *bvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == nullptr || yvar == nullptr || avar == nullptr || bvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::TP_NUMERIC ||
         yvar->getType() != finExecVariable::TP_NUMERIC ||
         avar->getType() != finExecVariable::TP_NUMERIC ||
         bvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double a2 = a * a, b2 = b * b;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(b2 * x * x - a2 * y * y - a2 * b2);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_eq2d_hyperbola = {
    /*._funcName     =*/ QString("eq2d_hyperbola"),
    /*._paramCsvList =*/ QString("x,y,a,b"),
    /*._funcCall     =*/ _sysfunc_eq2d_hyperbola,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("eq2d_hyperbola (x, y, a, b)"),
    /*._description  =*/ QString("The 2D analytic geometry function -- Hyperbola."),
};

static struct finExecSysFuncRegItem _finSysFuncMathList[] = {
    _funcRegItem_abs,
    _funcRegItem_sig,

    _funcRegItem_sin,
    _funcRegItem_cos,
    _funcRegItem_tan,
    _funcRegItem_tg,
    _funcRegItem_cot,
    _funcRegItem_ctg,
    _funcRegItem_sec,
    _funcRegItem_csc,
    _funcRegItem_ln,
    _funcRegItem_log,

    _funcRegItem_linear,
    _funcRegItem_quadratic,
    _funcRegItem_frequency_curve,

    _funcRegItem_parm_circle,
    _funcRegItem_parm_ellipse,
    _funcRegItem_parm_general_ellipse,
    _funcRegItem_parm_hyperbola,
    _funcRegItem_parm_hyperbola_rational,
    _funcRegItem_parm_astroid,
    _funcRegItem_parm_lissajous,
    _funcRegItem_parm_involute,
    _funcRegItem_parm_cycloid,
    _funcRegItem_parm_hypotrochoid,
    _funcRegItem_parm_butterfly,

    _funcRegItem_eq2d_circle,
    _funcRegItem_eq2d_ellipse,
    _funcRegItem_eq2d_hyperbola,

    { QString(), QString(), nullptr }
};

finErrorCode finExecFunction::registSysFuncMath()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMathList, _defFuncCtg);
}

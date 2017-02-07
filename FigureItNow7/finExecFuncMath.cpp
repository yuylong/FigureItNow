/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 11th, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170111    0  Yulong Yu    Create this file.
 * 20170112    1  Yulong Yu    Add tan script function.
 * 20170115    2  Yulong Yu    Add cot script function.
 * 20170127    3  Yulong Yu    Add log and ln script functions.
 */

#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecFlowControl.h"

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_cos(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_tan(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_cot(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ln(finExecFunction *self, finExecEnvironment *env,
                                finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_log(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_linear(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_quadratic(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_eq2d_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_eq2d_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_eq2d_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncMathList[] = {
    { QString("sin"),            QString("rad"),      _sysfunc_sin            },
    { QString("cos"),            QString("rad"),      _sysfunc_cos            },
    { QString("tan"),            QString("rad"),      _sysfunc_tan            },
    { QString("tg"),             QString("rad"),      _sysfunc_tan            },
    { QString("cot"),            QString("rad"),      _sysfunc_cot            },
    { QString("ctg"),            QString("rad"),      _sysfunc_cot            },
    { QString("ln"),             QString("base"),     _sysfunc_ln             },
    { QString("log"),            QString("idx,base"), _sysfunc_log            },

    { QString("linear"),         QString ("x,a,b"),   _sysfunc_linear         },
    { QString("quadratic"),      QString ("x,a,b,c"), _sysfunc_quadratic      },

    { QString("eq2d_circle"),    QString ("x,y,r"),   _sysfunc_eq2d_circle    },
    { QString("eq2d_ellipse"),   QString ("x,y,a,b"), _sysfunc_eq2d_ellipse },
    { QString("eq2d_hyperbola"), QString ("x,y,a,b"), _sysfunc_eq2d_hyperbola },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncMath()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMathList);
}

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(sin(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_cos(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(cos(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_tan(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(tan(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_cot(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    if ( rad == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(1.0 / tan(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_ln(finExecFunction *self, finExecEnvironment *env,
                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *base, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    base = finExecVariable::transLinkTarget(env->findVariable("base"));
    if ( base == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( base->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(log(base->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_log(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *index, *base, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    index = finExecVariable::transLinkTarget(env->findVariable("index"));
    base = finExecVariable::transLinkTarget(env->findVariable("base"));
    if ( index == NULL || base == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( index->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         base->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(log(base->getNumericValue()) / log(index->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_linear(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *avar, *bvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(a * x + b);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_quadratic(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *avar, *bvar, *cvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    cvar = finExecVariable::transLinkTarget(env->findVariable("c"));
    if ( xvar == NULL || avar == NULL || bvar == NULL || cvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double c = cvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue((a * x + b) * x + c);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_eq2d_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *rvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( xvar == NULL || yvar == NULL || rvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         yvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(x * x + y * y - r * r);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_eq2d_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *avar, *bvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == NULL || yvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         yvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double a2 = a * a, b2 = b * b;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(b2 * x * x + a2 * y * y - a2 * b2);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_eq2d_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *xvar, *yvar, *avar, *bvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    xvar = finExecVariable::transLinkTarget(env->findVariable("x"));
    yvar = finExecVariable::transLinkTarget(env->findVariable("y"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( xvar == NULL || yvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( xvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         yvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double x = xvar->getNumericValue();
    double y = yvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double a2 = a * a, b2 = b * b;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(b2 * x * x - a2 * y * y - a2 * b2);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

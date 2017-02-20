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


static finErrorCode _sysfunc_abs(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_cos(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_tan(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_cot(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_sec(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_csc(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ln(finExecFunction *self, finExecEnvironment *env,
                                finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_log(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_linear(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_quadratic(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_parm_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_general_ellipse(finExecFunction *self, finExecEnvironment *env,
                                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_hyperbola_rational(finExecFunction *self, finExecEnvironment *env,
                                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_lissajous(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_involute(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_cycloid(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_hypotrochoid(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_parm_butterfly(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_eq2d_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_eq2d_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_eq2d_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncMathList[] = {
    { QString("abs"),                       QString("num"),              _sysfunc_abs                       },
    { QString("sin"),                       QString("rad"),              _sysfunc_sin                       },
    { QString("cos"),                       QString("rad"),              _sysfunc_cos                       },
    { QString("tan"),                       QString("rad"),              _sysfunc_tan                       },
    { QString("tg"),                        QString("rad"),              _sysfunc_tan                       },
    { QString("cot"),                       QString("rad"),              _sysfunc_cot                       },
    { QString("ctg"),                       QString("rad"),              _sysfunc_cot                       },
    { QString("sec"),                       QString("rad"),              _sysfunc_sec                       },
    { QString("csc"),                       QString("rad"),              _sysfunc_csc                       },
    { QString("ln"),                        QString("base"),             _sysfunc_ln                        },
    { QString("log"),                       QString("idx,base"),         _sysfunc_log                       },

    { QString("linear"),                    QString ("x,a,b"),           _sysfunc_linear                    },
    { QString("quadratic"),                 QString ("x,a,b,c"),         _sysfunc_quadratic                 },

    { QString("parm_circle"),               QString ("t,r"),             _sysfunc_parm_circle               },
    { QString("parm_ellipse"),              QString ("t,a,b"),           _sysfunc_parm_ellipse              },
    { QString("parm_general_ellipse"),      QString ("t,a,b,xc,yc,phi"), _sysfunc_parm_general_ellipse      },
    { QString("parm_hyperbola"),            QString ("t,a,b,h,k"),       _sysfunc_parm_hyperbola            },
    { QString("parm_hyperbola_rational"),   QString ("t,a,b,h,k"),       _sysfunc_parm_hyperbola_rational   },
    { QString("parm_lissajous"),            QString ("t,a,b,kx,ky"),     _sysfunc_parm_lissajous            },
    { QString("parm_involute"),             QString ("t,r"),             _sysfunc_parm_involute             },
    { QString("parm_cycloid"),              QString ("t,r"),             _sysfunc_parm_cycloid              },
    { QString("parm_hypotrochoid"),         QString ("t,R,r,d"),         _sysfunc_parm_hypotrochoid         },
    { QString("parm_butterfly"),            QString ("t"),               _sysfunc_parm_butterfly            },

    { QString("eq2d_circle"),               QString ("x,y,r"),           _sysfunc_eq2d_circle               },
    { QString("eq2d_ellipse"),              QString ("x,y,a,b"),         _sysfunc_eq2d_ellipse              },
    { QString("eq2d_hyperbola"),            QString ("x,y,a,b"),         _sysfunc_eq2d_hyperbola            },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncMath()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMathList);
}

static finErrorCode _sysfunc_abs(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *num, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    num = finExecVariable::transLinkTarget(env->findVariable("num"));
    if ( num == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( num->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(fabs(num->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

static finErrorCode _sysfunc_sec(finExecFunction *self, finExecEnvironment *env,
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
    retvar->setNumericValue(1.0 / cos(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_csc(finExecFunction *self, finExecEnvironment *env,
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
    retvar->setNumericValue(1.0 / sin(rad->getNumericValue()));
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

static finErrorCode _sysfunc_parm_circle(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == NULL || rvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(r * cos(t));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(r * sin(t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_ellipse(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    if ( tvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(a * cos(t));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(b * sin(t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_general_ellipse(finExecFunction *self, finExecEnvironment *env,
                                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *xcvar, *ycvar, *phivar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    xcvar = finExecVariable::transLinkTarget(env->findVariable("xc"));
    ycvar = finExecVariable::transLinkTarget(env->findVariable("yc"));
    phivar = finExecVariable::transLinkTarget(env->findVariable("phi"));
    if ( tvar == NULL || avar == NULL || bvar == NULL || xcvar == NULL || ycvar == NULL || phivar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         xcvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         ycvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         phivar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double xc = xcvar->getNumericValue();
    double yc = ycvar->getNumericValue();
    double phi = phivar->getNumericValue();

    double cost = cos(t), sint = sin(t);
    double cosphi = cos(phi), sinphi = sin(phi);

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(xc + a * cost * cosphi - b * sint * sinphi);
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(yc + a * cost * sinphi + b * sint * cosphi);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_hyperbola(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *hvar, *kvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    hvar = finExecVariable::transLinkTarget(env->findVariable("h"));
    kvar = finExecVariable::transLinkTarget(env->findVariable("k"));
    if ( tvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (hvar != NULL && hvar->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                          hvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) ||
         (kvar != NULL && kvar->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                          kvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double h = ((hvar != NULL && hvar->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC) ?
                hvar->getNumericValue() : 0.0);
    double k = ((kvar != NULL && kvar->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC) ?
                kvar->getNumericValue() : 0.0);

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(a / cos(t) + h);
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(b * tan(t) + k);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_hyperbola_rational(finExecFunction *self, finExecEnvironment *env,
                                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *hvar, *kvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    hvar = finExecVariable::transLinkTarget(env->findVariable("h"));
    kvar = finExecVariable::transLinkTarget(env->findVariable("k"));
    if ( tvar == NULL || avar == NULL || bvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (hvar != NULL && hvar->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                          hvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) ||
         (kvar != NULL && kvar->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                          kvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double h = ((hvar != NULL && hvar->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC) ?
                hvar->getNumericValue() : 0.0);
    double k = ((kvar != NULL && kvar->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC) ?
                kvar->getNumericValue() : 0.0);
    double t2 = t * t;
    double ratio = 1.0 - t2;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(a * (1 + t2) / ratio + h);
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(b * 2.0 * t / ratio + k);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_lissajous(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *avar, *bvar, *kxvar, *kyvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    avar = finExecVariable::transLinkTarget(env->findVariable("a"));
    bvar = finExecVariable::transLinkTarget(env->findVariable("b"));
    kxvar = finExecVariable::transLinkTarget(env->findVariable("kx"));
    kyvar = finExecVariable::transLinkTarget(env->findVariable("ky"));
    if ( tvar == NULL || avar == NULL || bvar == NULL || kxvar == NULL || kyvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         avar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         bvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         kxvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         kyvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double a = avar->getNumericValue();
    double b = bvar->getNumericValue();
    double kx = kxvar->getNumericValue();
    double ky = kyvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(a * cos(kx * t));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(b * sin(ky * t));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_involute(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == NULL || rvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(r * (cos(t) + t * sin(t)));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(r * (sin(t) - t * cos(t)));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_cycloid(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *rvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    if ( tvar == NULL || rvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double r = rvar->getNumericValue();

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(r * (t - sin(t)));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(r * (1 - cos(t)));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_hypotrochoid(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *Rvar, *rvar, *dvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    Rvar = finExecVariable::transLinkTarget(env->findVariable("R"));
    rvar = finExecVariable::transLinkTarget(env->findVariable("r"));
    dvar = finExecVariable::transLinkTarget(env->findVariable("d"));
    if ( tvar == NULL || Rvar == NULL || rvar == NULL || dvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         Rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         rvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         dvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double R = Rvar->getNumericValue();
    double r = rvar->getNumericValue();
    double d = dvar->getNumericValue();
    double diff = R -r;
    double ratio = t * (R - r) / r;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(diff * cos(t) + d * cos(ratio));
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(diff * sin(t) - d * sin(ratio));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_parm_butterfly(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *tvar, *retvar, *retitemx, *retitemy;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    tvar = finExecVariable::transLinkTarget(env->findVariable("t"));
    if ( tvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( tvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double t = tvar->getNumericValue();
    double sint = sin(t), cost = cos(t);
    double sintd12 = sin(t / 12.0);
    double sin2td12 = sintd12 * sintd12;
    double ratio = pow(M_E, cost) - 2 * cos(4.0 * t) - sin2td12 * sin2td12 * sintd12;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->preallocArrayLength(2);
    retitemx = retvar->getVariableItemAt(0);
    retitemy = retvar->getVariableItemAt(1);
    if ( retitemx == NULL || retitemy == NULL ) {
        delete retvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    retitemx->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemx->setNumericValue(sint * ratio);
    retitemy->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retitemy->setNumericValue(cost * ratio);
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

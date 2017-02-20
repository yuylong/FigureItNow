/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>
#include <QString>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureObject.h"
#include "finFigureConfig.h"
#include "finFigureArrow.h"
#include "finFigureContainer.h"
#include "finGraphConfig.h"
#include "finGraphTrans.h"
#include "finPlotDots.h"
#include "finPlotFunction.h"
#include "finPlotPolar.h"
#include "finPlotParametric.h"
#include "finPlotEquation2D.h"


static finErrorCode _sysfunc_plot_dots(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_line(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_function(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_polar(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_parametric(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_equation(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);

static finExecSysFuncRegItem _finSysFuncPlotList[] = {
    { QString("plot_dots"),         QString("xary,yary"),            _sysfunc_plot_dots          },
    { QString("plot_line"),         QString("xary,yary"),            _sysfunc_plot_line          },

    { QString("plot_function"),     QString("x1,x2,func"),           _sysfunc_plot_function      },
    { QString("plot_polar"),        QString("rad1,rad2,func"),       _sysfunc_plot_polar         },
    { QString("plot_parametric"),   QString("t1,t2,func"),           _sysfunc_plot_parametric    },
    { QString("plot_equation"),     QString("x1,x2,y1,y2,func"),     _sysfunc_plot_equation      },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncPlot()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncPlotList);
}

static finErrorCode _sysfunc_plot_dots(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finPlotDots dotplot;
    dotplot.appendPoints(ptlist);
    dotplot.setFigureContainer(env->getFigureContainer());
    errcode = dotplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_line(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finPlotDotsLine lnplot;
    lnplot.appendPoints(ptlist);
    lnplot.setFigureContainer(env->getFigureContainer());
    errcode = lnplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_function(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == NULL || funcvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *x1var, *x2var;
    x1var = finExecVariable::transLinkTarget(env->findVariable("x1"));
    x2var = finExecVariable::transLinkTarget(env->findVariable("x2"));
    if ( x1var == NULL || x2var == NULL ||
         x1var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         x2var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString funcname = funcvar->getStringValue();
    double x1 = x1var->getNumericValue();
    double x2 = x2var->getNumericValue();

    finPlotFunction plotfunc;
    plotfunc.setFunctionName(funcname);
    plotfunc.setFigureRange(x1, x2);
    plotfunc.setIndependentVarIndex(0);

    QList<finExecVariable *> extarglist = finExecFunction::getExtendArgList(env);
    plotfunc.setCallArgList(&extarglist);
    plotfunc.setEnvironment(env);
    plotfunc.setMachine(machine);
    plotfunc.setFlowControl(flowctl);
    plotfunc.setFigureContainer(env->getFigureContainer());

    finErrorCode errcode = plotfunc.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_polar(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == NULL || funcvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *rad1var, *rad2var;
    rad1var = finExecVariable::transLinkTarget(env->findVariable("rad1"));
    rad2var = finExecVariable::transLinkTarget(env->findVariable("rad2"));
    if ( rad1var == NULL || rad2var == NULL ||
         rad1var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         rad2var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString funcname = funcvar->getStringValue();
    double rad1 = rad1var->getNumericValue();
    double rad2 = rad2var->getNumericValue();

    finPlotPolar plotpolar;
    plotpolar.setFunctionName(funcname);
    plotpolar.setFigureRange(rad1, rad2);
    plotpolar.setRadianVarIndex(0);

    QList<finExecVariable *> extarglist = finExecFunction::getExtendArgList(env);
    plotpolar.setCallArgList(&extarglist);
    plotpolar.setEnvironment(env);
    plotpolar.setMachine(machine);
    plotpolar.setFlowControl(flowctl);
    plotpolar.setFigureContainer(env->getFigureContainer());

    finErrorCode errcode = plotpolar.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_parametric(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == NULL || funcvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *t1var, *t2var;
    t1var = finExecVariable::transLinkTarget(env->findVariable("t1"));
    t2var = finExecVariable::transLinkTarget(env->findVariable("t2"));
    if ( t1var == NULL || t2var == NULL ||
         t1var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         t2var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString funcname = funcvar->getStringValue();
    double t1 = t1var->getNumericValue();
    double t2 = t2var->getNumericValue();

    finPlotParametric plotpar;
    plotpar.setFunctionName(funcname);
    plotpar.setParameterValueRange(t1, t2);
    plotpar.setParameterVarIndex(0);

    QList<finExecVariable *> extarglist = finExecFunction::getExtendArgList(env);
    plotpar.setCallArgList(&extarglist);
    plotpar.setEnvironment(env);
    plotpar.setMachine(machine);
    plotpar.setFlowControl(flowctl);
    plotpar.setFigureContainer(env->getFigureContainer());

    finErrorCode errcode = plotpar.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_equation(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == NULL || funcvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *x1var, *x2var, *y1var, *y2var;
    x1var = finExecVariable::transLinkTarget(env->findVariable("x1"));
    x2var = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y1var = finExecVariable::transLinkTarget(env->findVariable("y1"));
    y2var = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( x1var == NULL || x2var == NULL || y1var == NULL || y2var == NULL ||
         x1var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         x2var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         y1var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC ||
         y2var->getType() != finExecVariableType::FIN_VR_TYPE_NUMERIC)
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString funcname = funcvar->getStringValue();
    double x1 = x1var->getNumericValue();
    double x2 = x2var->getNumericValue();
    double y1 = y1var->getNumericValue();
    double y2 = y2var->getNumericValue();

    finPlotEquation2D ploteq;
    ploteq.setFunctionName(funcname);
    ploteq.setFiguringRangeX(x1, x2);
    ploteq.setFiguringRangeY(y1, y2);
    ploteq.setVariableXIndex(0);
    ploteq.setVariableYIndex(1);

    QList<finExecVariable *> extarglist = finExecFunction::getExtendArgList(env);
    ploteq.setCallArgList(&extarglist);
    ploteq.setEnvironment(env);
    ploteq.setMachine(machine);
    ploteq.setFlowControl(flowctl);
    ploteq.setFigureContainer(env->getFigureContainer());

    finErrorCode errcode = ploteq.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

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
static finErrorCode _sysfunc_plot_stream(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_scatter(finExecFunction *self, finExecEnvironment *env,
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
    { QString("plot_stream"),       QString("xary,yary"),            _sysfunc_plot_stream        },
    { QString("plot_scatter"),      QString("dl,xary,yary"),         _sysfunc_plot_scatter       },

    { QString("plot_function"),     QString("x1,x2,func"),           _sysfunc_plot_function      },
    { QString("plot_polar"),        QString("rad1,rad2,func"),       _sysfunc_plot_polar         },
    { QString("plot_parametric"),   QString("t1,t2,func"),           _sysfunc_plot_parametric    },
    { QString("plot_equation"),     QString("x1,x2,y1,y2,func"),     _sysfunc_plot_equation      },

    { QString(), QString(), nullptr }
};

finErrorCode finExecFunction::registSysFuncPlot()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncPlotList, QString("Plotting"));
}

static finErrorCode _sysfunc_plot_dots(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finPlotDots dotplot;
    dotplot.appendPoints(ptlist);
    dotplot.setFigureContainer(env->getFigureContainer());
    errcode = dotplot.plot();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_line(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finPlotDotsLine lnplot;
    lnplot.appendPoints(ptlist);
    lnplot.setFigureContainer(env->getFigureContainer());
    errcode = lnplot.plot();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_stream(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finPlotDotsStream stmplot;
    stmplot.appendPoints(ptlist);
    stmplot.setFigureContainer(env->getFigureContainer());
    errcode = stmplot.plot();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_scatter(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *dislmtvar = finExecVariable::transLinkTarget(env->findVariable("dl"));
    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));
    if ( dislmtvar != nullptr && dislmtvar->getType() != finExecVariable::TP_NULL &&
                              dislmtvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    double dislmt = 0.1;
    if ( dislmtvar != nullptr && dislmtvar->getType() == finExecVariable::TP_NUMERIC )
        dislmt = dislmtvar->getNumericValue();

    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finPlotDotsScatter scplot;
    scplot.setDistanceLimit(dislmt);
    scplot.appendPoints(ptlist);
    scplot.setFigureContainer(env->getFigureContainer());
    errcode = scplot.plot();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_function(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == nullptr || funcvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *x1var, *x2var;
    x1var = finExecVariable::transLinkTarget(env->findVariable("x1"));
    x2var = finExecVariable::transLinkTarget(env->findVariable("x2"));
    if ( x1var == nullptr || x2var == nullptr ||
         x1var->getType() != finExecVariable::TP_NUMERIC ||
         x2var->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

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
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_polar(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == nullptr || funcvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *rad1var, *rad2var;
    rad1var = finExecVariable::transLinkTarget(env->findVariable("rad1"));
    rad2var = finExecVariable::transLinkTarget(env->findVariable("rad2"));
    if ( rad1var == nullptr || rad2var == nullptr ||
         rad1var->getType() != finExecVariable::TP_NUMERIC ||
         rad2var->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

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
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_parametric(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == nullptr || funcvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *t1var, *t2var;
    t1var = finExecVariable::transLinkTarget(env->findVariable("t1"));
    t2var = finExecVariable::transLinkTarget(env->findVariable("t2"));
    if ( t1var == nullptr || t2var == nullptr ||
         t1var->getType() != finExecVariable::TP_NUMERIC ||
         t2var->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

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
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_equation(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *funcvar;
    funcvar = finExecVariable::transLinkTarget(env->findVariable("func"));
    if ( funcvar == nullptr || funcvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *x1var, *x2var, *y1var, *y2var;
    x1var = finExecVariable::transLinkTarget(env->findVariable("x1"));
    x2var = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y1var = finExecVariable::transLinkTarget(env->findVariable("y1"));
    y2var = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( x1var == nullptr || x2var == nullptr || y1var == nullptr || y2var == nullptr ||
         x1var->getType() != finExecVariable::TP_NUMERIC ||
         x2var->getType() != finExecVariable::TP_NUMERIC ||
         y1var->getType() != finExecVariable::TP_NUMERIC ||
         y2var->getType() != finExecVariable::TP_NUMERIC)
        return finErrorKits::EC_INVALID_PARAM;

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
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    if ( !flowctl->isFlowNext() )
        return errcode;

    // Because all extended arguments are left values, we do not release the memory for arglist.
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

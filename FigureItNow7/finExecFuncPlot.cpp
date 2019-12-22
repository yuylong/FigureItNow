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


static QString _defFuncCtg("Plotting");

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

static struct finExecSysFuncRegItem _funcRegItem_plot_dots = {
    /*._funcName     =*/ QString("plot_dots"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_plot_dots,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_dots (xary, yary)"),
    /*._description  =*/ QString("Plot the dots on graph panel with the given point list."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_line = {
    /*._funcName     =*/ QString("plot_line"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_plot_line,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_line (xary, yary)"),
    /*._description  =*/ QString("Plot the continuous lines on graph panel with the given point list."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_stream = {
    /*._funcName     =*/ QString("plot_stream"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_plot_stream,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_stream (xary, yary)"),
    /*._description  =*/ QString("Plot the continuous lines on graph panel with the given point list. The generated "
                                 "lines will be spitted by the invalid points."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_scatter = {
    /*._funcName     =*/ QString("plot_scatter"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_plot_scatter,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_scatter (xary, yary)"),
    /*._description  =*/ QString("Plot the continuous lines on graph panel with the given point list. The lines are "
                                 "generated by connecting the adjacent points on the geometrical positions."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_function = {
    /*._funcName     =*/ QString("plot_function"),
    /*._paramCsvList =*/ QString("x1,x2,func"),
    /*._funcCall     =*/ _sysfunc_plot_function,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_function (x1, x2, func, ...)"),
    /*._description  =*/ QString("Plot the function figure on graph panel between the given two virables (x1 and x2). "
                                 "The function is given by its name as a string, which can be internal function or "
                                 "defined in script code. It requires that the first argument of the plotted function "
                                 "is the plotting variable x, and other arguments of the plotted function is given "
                                 "following the function name in the argument list."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_polar = {
    /*._funcName     =*/ QString("plot_polar"),
    /*._paramCsvList =*/ QString("rad1,rad2,func"),
    /*._funcCall     =*/ _sysfunc_plot_polar,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_polar (rad1, rad2, func, ...)"),
    /*._description  =*/ QString("Plot the polar function figure on graph panel between the given two radians (rad1 "
                                 "and rad2). The polar function is given by its name as a string, which can be "
                                 "internal function or defined in script code. It requires that the first argument of "
                                 "the plotted polar function is the plotting radian, and other arguments of the "
                                 "plotted function is given following the function name in the argument list."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_parametric = {
    /*._funcName     =*/ QString("plot_parametric"),
    /*._paramCsvList =*/ QString("t1,t2,func"),
    /*._funcCall     =*/ _sysfunc_plot_parametric,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_parametric (t1, t2, func, ...)"),
    /*._description  =*/ QString("Plot the parametric function group figure on graph panel between the given two "
                                 "parameter values (t1 and t2). The parametric function group is formatted as a script "
                                 "function, whose return value is an array return two numerical value from each sub-"
                                 "function. The plotted function is given by its name as a string, which can be "
                                 "internal function or defined in script code. It requires that the first argument of "
                                 "the plotted polar function is the plotting radian, and other arguments of the "
                                 "plotted function is given following the function name in the argument list."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_plot_equation = {
    /*._funcName     =*/ QString("plot_equation"),
    /*._paramCsvList =*/ QString("x1,x2,y1,y2,func"),
    /*._funcCall     =*/ _sysfunc_plot_equation,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("plot_equation (x1, x2, y1, y2, func, ...)"),
    /*._description  =*/ QString("Plot the 2D equation figure on graph panel in an rectangle region (x1, y1)-(x2, y2). "
                                 "The figure shows the curve whose points satisfy the equation func(x, y) = 0. The "
                                 "plotted function is given by its name as a string, which can be internal function or "
                                 "defined in script code. It requires that the first argument of the plotted polar "
                                 "function is the plotting radian, and other arguments of the plotted function is "
                                 "given following the function name in the argument list."),
};

static finExecSysFuncRegItem _finSysFuncPlotList[] = {
    _funcRegItem_plot_dots,
    _funcRegItem_plot_line,
    _funcRegItem_plot_stream,
    _funcRegItem_plot_scatter,

    _funcRegItem_plot_function,
    _funcRegItem_plot_polar,
    _funcRegItem_plot_parametric,
    _funcRegItem_plot_equation,

    { QString(), QString(), nullptr, _defFuncCtg, QString(), QString() }
};

finErrorCode finExecFunction::registSysFuncPlot()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncPlotList, QString("Plotting"));
}

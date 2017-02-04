
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


static finErrorCode _sysfunc_plot_dots(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_dots_xy(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_dots_mat(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_line(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_line_xy(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_line_mat(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_plot_function(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);

static finExecSysFuncRegItem _finSysFuncPlotList[] = {
    { QString("plot_dots"),         QString("ary"),                  _sysfunc_plot_dots          },
    { QString("plot_dots_xy"),      QString("xary,yary"),            _sysfunc_plot_dots_xy       },
    { QString("plot_dots_mat"),     QString("mat"),                  _sysfunc_plot_dots_mat      },
    { QString("plot_line"),         QString("ary"),                  _sysfunc_plot_line          },
    { QString("plot_line_xy"),      QString("xary,yary"),            _sysfunc_plot_line_xy       },
    { QString("plot_line_mat"),     QString("mat"),                  _sysfunc_plot_line_mat      },
    { QString("plot_function"),     QString("x1,x2,func"),           _sysfunc_plot_function      },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncPlot()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncPlotList);
}

static finErrorCode _sysfunc_plot_help_readdots_array(finPlotDots *plotdots, finExecVariable *ary)
{
    if ( plotdots == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( ary == NULL || ary->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        plotdots->clearPoints();
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    int arylen = 0;
    if ( !ary->isNumericArray(&arylen) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    plotdots->clearPoints();
    for ( int i = 0; i + 1 < arylen; i += 2 ) {
        finExecVariable *varx = ary->getVariableItemAt(i);
        finExecVariable *vary = ary->getVariableItemAt(i + 1);

        plotdots->appendPoint(varx->getNumericValue(), vary->getNumericValue());
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_help_readdots_xyarray(finPlotDots *plotdots,
                                                        finExecVariable *xary, finExecVariable *yary)
{
    if ( plotdots == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( (xary == NULL || xary->getType() == finExecVariable::FIN_VR_TYPE_NULL) ||
         (yary == NULL || yary->getType() == finExecVariable::FIN_VR_TYPE_NULL) ) {
        plotdots->clearPoints();
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    int arylen = 0, yarylen = 0;
    if ( !xary->isNumericArray(&arylen) || !yary->isNumericArray(&yarylen) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( arylen > yarylen )
        arylen = yarylen;

    plotdots->clearPoints();
    for ( int i = 0; i < arylen; i++ ) {
        finExecVariable *varx = xary->getVariableItemAt(i);
        finExecVariable *vary = yary->getVariableItemAt(i);

        plotdots->appendPoint(varx->getNumericValue(), vary->getNumericValue());
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_help_readdots_xymat(finPlotDots *plotdots, finExecVariable *mat)
{
    if ( plotdots == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( mat == NULL || mat->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        plotdots->clearPoints();
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    int row = 0, col = 0;
    if ( !mat->isNumericMatrix(&row, &col) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( col < 2 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    plotdots->clearPoints();
    for ( int i = 0; i < row; i++ ) {
        finExecVariable *ptvar = mat->getVariableItemAt(i);
        finExecVariable *varx = ptvar->getVariableItemAt(0);
        finExecVariable *vary = ptvar->getVariableItemAt(1);

        plotdots->appendPoint(varx->getNumericValue(), vary->getNumericValue());
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_dots(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));

    finPlotDots dotplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_array(&dotplot, aryvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    dotplot.setFigureContainer(env->getFigureContainer());
    errcode = dotplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_dots_xy(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    finPlotDots dotplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_xyarray(&dotplot, xaryvar, yaryvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    dotplot.setFigureContainer(env->getFigureContainer());
    errcode = dotplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_dots_mat(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));

    finPlotDots dotplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_xymat(&dotplot, matvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

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

    finExecVariable *aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));

    finPlotDotsLine lnplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_array(&lnplot, aryvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    lnplot.setFigureContainer(env->getFigureContainer());
    errcode = lnplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_line_xy(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));

    finPlotDotsLine lnplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_xyarray(&lnplot, xaryvar, yaryvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    lnplot.setFigureContainer(env->getFigureContainer());
    errcode = lnplot.plot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_plot_line_mat(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));

    finPlotDotsLine lnplot;
    finErrorCode errcode = _sysfunc_plot_help_readdots_xymat(&lnplot, matvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

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

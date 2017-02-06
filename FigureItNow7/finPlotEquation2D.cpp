#include "finPlotEquation2D.h"

#include <qmath.h>

#include "finExecFunction.h"

finPlotEquation2D::finPlotEquation2D()
    : _scrtPlot()
{
    this->_funcname = QString();
    this->_fromX = 0.0;
    this->_toX = 0.0;
    this->_fromY = 0.0;
    this->_toY = 0.0;
    this->_stepX = 0.1;
    this->_stepY = 0.1;
    this->_xidx = 0;
    this->_yidx = 1;
    this->_callArgList = NULL;
    this->_environment = NULL;
    this->_machine = NULL;
    this->_flowctl = NULL;
    this->_scrtPlot.setFigureContainer(NULL);
    this->_scrtPlot.clearPoints();
}

double finPlotEquation2D::getStartPositionX() const
{
    return this->_fromX;
}

double finPlotEquation2D::getEndPostionX() const
{
    return this->_toX;
}

double finPlotEquation2D::getStartPostionY() const
{
    return this->_fromY;
}

double finPlotEquation2D::getEndPostionY() const
{
    return this->_toY;
}

int finPlotEquation2D::getVariableXIndex() const
{
    return this->_xidx;
}

int finPlotEquation2D::getVariableYIndex() const
{
    return this->_yidx;
}

finErrorCode finPlotEquation2D::setFiguringRangeX(double x1, double x2)
{
    if ( x1 <= x2 ) {
        this->_fromX = x1;
        this->_toX = x2;
    } else {
        this->_fromX = x2;
        this->_toX = x1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setFiguringRangeY(double y1, double y2)
{
    if ( y1 <= y2 ) {
        this->_fromY = y1;
        this->_toY = y2;
    } else {
        this->_fromY = y2;
        this->_toY = y1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setVariableXIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_xidx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setVariableYIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_yidx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QList<finExecVariable *> *finPlotEquation2D::getCallArgList() const
{
    return this->_callArgList;
}

finExecEnvironment *finPlotEquation2D::getEnvironment() const
{
    return this->_environment;
}

finExecMachine *finPlotEquation2D::getMachine() const
{
    return this->_machine;
}

finExecFlowControl *finPlotEquation2D::getFlowControl() const
{
    return this->_flowctl;
}

finFigureContainer *finPlotEquation2D::getFigureContainer() const
{
    return this->_scrtPlot.getFigureContainer();
}

finErrorCode finPlotEquation2D::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setFigureContainer(finFigureContainer *figcontainer)
{
    return this->_scrtPlot.setFigureContainer(figcontainer);
}

bool finPlotEquation2D::checkValid() const
{
    if ( this->_funcname.isEmpty() || this->_fromX >= this->_toX || this->_fromY >= this->_toY)
        return false;
    if ( this->_callArgList == NULL || this->_environment == NULL || this->_machine == NULL ||
         this->_flowctl == NULL || this->_scrtPlot.getFigureContainer() == NULL )
        return false;
    if ( this->_xidx == this->_yidx ||
         this->_xidx < 0 || this->_xidx >= this->_callArgList->count() + 2 ||
         this->_yidx < 0 || this->_yidx >= this->_callArgList->count() + 2 )
        return false;

    return true;
}

double finPlotEquation2D::getBaseStep() const
{
    double step = 0.1;
    if ( this->_scrtPlot.getFigureContainer() == NULL )
        return step;

    finGraphConfig *graphcfg = this->_scrtPlot.getFigureContainer()->getGraphConfig();
    if ( graphcfg != NULL )
        step = 3.0 / graphcfg->getAxisUnitPixelSize();
    return step;
}

finErrorCode finPlotEquation2D::buildFuncArgList(QList<finExecVariable *> *varlist,
                                                 finExecVariable **xvar, finExecVariable **yvar)
{
    if ( varlist == NULL || xvar == NULL || yvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    *xvar = new finExecVariable();
    if ( *xvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    *yvar = new finExecVariable();
    if ( *yvar == NULL ) {
        delete *xvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    (*xvar)->setName("__fig_func_drv_arg_x");
    (*xvar)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    (*xvar)->setLeftValue();
    (*xvar)->clearWriteProtected();

    (*yvar)->setName("__fig_func_drv_arg_y");
    (*yvar)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    (*yvar)->setLeftValue();
    (*yvar)->clearWriteProtected();

    *varlist = *this->_callArgList;
    if ( this->_xidx < this->_yidx ) {
        varlist->insert(this->_xidx, *xvar);
        varlist->insert(this->_yidx, *yvar);
    } else {
        varlist->insert(this->_yidx, *yvar);
        varlist->insert(this->_xidx, *xvar);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;

}

finErrorCode finPlotEquation2D::calcAPoint(
        double x, double y, finExecFunction *func, QList<finExecVariable *> *varlist,
        finExecVariable *xvar, finExecVariable *yvar, double *retval, bool *goon)
{
    if ( varlist == NULL || xvar == NULL || xvar == NULL || retval == NULL || goon == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finErrorCode errcode;
    xvar->setNumericValue(x);
    yvar->setNumericValue(y);

    errcode = func->execFunction(varlist, this->_environment, this->_machine, this->_flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->_flowctl->checkFlowForExpress(goon, NULL, this->_machine);
    if ( finErrorCodeKits::isErrorResult(errcode) || !(*goon) )
        return errcode;

    finExecVariable *retvar = this->_flowctl->pickReturnVariable();
    if ( retvar == NULL || retvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        finExecVariable::releaseNonLeftVariable(retvar);
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = retvar->getNumericValue();
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::plot()
{
    if ( !this->checkValid() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

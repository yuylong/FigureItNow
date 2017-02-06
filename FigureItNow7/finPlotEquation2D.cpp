#include "finPlotEquation2D.h"

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

finErrorCode finPlotEquation2D::plot()
{
    if ( !this->checkValid() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

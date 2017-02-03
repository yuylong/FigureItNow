#include "finPlotFunction.h"

#include "finExecFunction.h"

finPlotFunction::finPlotFunction()
{
    this->_funcname = QString();
    this->_fromX = 0.0;
    this->_toX = 0.0;
    this->_xidx = 0;
    this->_callArgList = NULL;
    this->_environment = NULL;
    this->_machine = NULL;
    this->_flowctl = NULL;
    this->_figcontainer = NULL;
}

const QString &finPlotFunction::getFunctionName() const
{
    return this->_funcname;
}

double finPlotFunction::getFigureFromPosition() const
{
    return this->_fromX;
}

double finPlotFunction::getFigureToPosition() const
{
    return this->_toX;
}

int finPlotFunction::getIndependentVarIndex() const
{
    return this->_xidx;
}

finErrorCode finPlotFunction::setFunctionName(const QString &funcname)
{
    this->_funcname = funcname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::setFigureRange(double x1, double x2)
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

finErrorCode finPlotFunction::setIndependentVarIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_xidx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

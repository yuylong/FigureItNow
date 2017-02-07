#include "finPlotParametric.h"

#include <qmath.h>
#include <QPointF>

#include "finExecFunction.h"
#include "finFigureAlg.h"
#include "finFigureObject.h"
#include "finGraphConfig.h"


finPlotParametric::finPlotParametric()
    : _stmPlot()
{
    this->_funcname = QString();
    this->_fromT = 0.0;
    this->_toT = 0.0;
    this->_tIdx = 0;
    this->_callArgList = NULL;
    this->_environment = NULL;
    this->_machine = NULL;
    this->_flowctl = NULL;
    this->_stmPlot.setFigureContainer(NULL);
    this->_stmPlot.clearPoints();
    this->_stmPlot.clearBreakPoints();
}

const QString &finPlotParametric::getFunctionName() const
{
    return this->_funcname;
}

double finPlotParametric::getParameterFromValue() const
{
    return this->_fromT;
}

double finPlotParametric::getParameterToValue() const
{
    return this->_toT;
}

int finPlotParametric::getParameterVarIndex() const
{
    return this->_tIdx;
}

finErrorCode finPlotParametric::setFunctionName(const QString &funcname)
{
    this->_funcname = funcname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setParameterValueRange(double t1, double t2)
{
    if ( t1 <= t2 ) {
        this->_fromT = t1;
        this->_toT = t2;
    } else {
        this->_fromT = t2;
        this->_toT = t1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setParameterVarIndex(int idx)
{
    this->_tIdx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

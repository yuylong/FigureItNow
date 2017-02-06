#include "finPlotPolar.h"

#include "finPlotFunction.h"

#include <qmath.h>
#include <QPointF>

#include "finExecFunction.h"
#include "finFigureAlg.h"
#include "finFigureObject.h"
#include "finGraphConfig.h"


finPlotPolar::finPlotPolar()
    : _stmPlot()
{
    this->_funcname = QString();
    this->_fromRad = 0.0;
    this->_toRad = 0.0;
    this->_radIdx = 0;
    this->_callArgList = NULL;
    this->_environment = NULL;
    this->_machine = NULL;
    this->_flowctl = NULL;
    this->_stmPlot.setFigureContainer(NULL);
    this->_stmPlot.clearPoints();
    this->_stmPlot.clearBreakPoints();
}

const QString &finPlotPolar::getFunctionName() const
{
    return this->_funcname;
}

double finPlotPolar::getFigureFromRadian() const
{
    return this->_fromRad;
}

double finPlotPolar::getFigureToRadian() const
{
    return this->_toRad;
}

int finPlotPolar::getRadianVarIndex() const
{
    return this->_radIdx;
}

finErrorCode finPlotPolar::setFunctionName(const QString &funcname)
{
    this->_funcname = funcname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setFigureRange(double rad1, double rad2)
{
    if ( rad1 <= rad2 ) {
        this->_fromRad = rad1;
        this->_toRad = rad2;
    } else {
        this->_fromRad = rad2;
        this->_toRad = rad1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setRadianVarIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_radIdx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

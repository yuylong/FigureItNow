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

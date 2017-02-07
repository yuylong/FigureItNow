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

QList<finExecVariable *> *finPlotParametric::getCallArgList() const
{
    return this->_callArgList;
}

finExecEnvironment *finPlotParametric::getEnvironment() const
{
    return this->_environment;
}

finExecMachine *finPlotParametric::getMachine() const
{
    return this->_machine;
}

finExecFlowControl *finPlotParametric::getFlowControl() const
{
    return this->_flowctl;
}

finFigureContainer *finPlotParametric::getFigureContainer() const
{
    return this->_stmPlot.getFigureContainer();
}

finErrorCode finPlotParametric::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotParametric::setFigureContainer(finFigureContainer *figcontainer)
{
    return this->_stmPlot.setFigureContainer(figcontainer);
}

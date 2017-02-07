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

QList<finExecVariable *> *finPlotPolar::getCallArgList() const
{
    return this->_callArgList;
}

finExecEnvironment *finPlotPolar::getEnvironment() const
{
    return this->_environment;
}

finExecMachine *finPlotPolar::getMachine() const
{
    return this->_machine;
}

finExecFlowControl *finPlotPolar::getFlowControl() const
{
    return this->_flowctl;
}

finFigureContainer *finPlotPolar::getFigureContainer() const
{
    return this->_stmPlot.getFigureContainer();
}

finErrorCode finPlotPolar::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::setFigureContainer(finFigureContainer *figcontainer)
{
    return this->_stmPlot.setFigureContainer(figcontainer);
}

bool finPlotPolar::checkValid() const
{
    if ( this->_funcname.isEmpty() )
        return false;
    if ( this->_callArgList == NULL || this->_environment == NULL || this->_machine == NULL ||
         this->_flowctl == NULL || this->_stmPlot.getFigureContainer() == NULL )
        return false;
    if ( this->_radIdx < 0 || this->_radIdx > this->_callArgList->count() )
        return false;

    return true;
}

finErrorCode finPlotPolar::buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **radvar)
{
    if ( varlist == NULL || radvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    *radvar = new finExecVariable();
    if ( *radvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    (*radvar)->setName(QString());
    (*radvar)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    (*radvar)->setLeftValue();
    (*radvar)->clearWriteProtected();
    this->_environment->addVariable(*radvar);

    *varlist = *this->_callArgList;
    varlist->insert(this->_radIdx, *radvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::calcAPoint(double rad, finExecFunction *func, QList<finExecVariable *> *varlist,
                                      finExecVariable *radvar, QPointF *pt, bool *goon)
{
    if ( varlist == NULL || radvar == NULL || pt == NULL || goon == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finErrorCode errcode;
    radvar->setNumericValue(rad);

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

    double rlen = retvar->getNumericValue();
    pt->setX(rlen * cos(rad));
    pt->setY(rlen * sin(rad));
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotPolar::plot()
{
    if ( !this->checkValid() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

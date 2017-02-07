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

double finPlotPolar::refineRadianStep(double rad) const
{
    if ( rad < M_PI / 1800.0 )
        return M_PI / 1800.0;
    else if ( rad > M_PI / 18.0 )
        return M_PI / 18.0;
    else
        return rad;

}
double finPlotPolar::getBaseStep() const
{
    double axisstep = 0.1;
    finFigureContainer *figcontainer = this->_stmPlot.getFigureContainer();
    if ( figcontainer != NULL && figcontainer->getGraphConfig() != NULL )
        axisstep = 3.0 / figcontainer->getGraphConfig()->getAxisUnitPixelSize();

    double radstep = atan(axisstep);
    return this->refineRadianStep(radstep);
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
                                      finExecVariable *radvar, double *retrlen, QPointF *pt, bool *goon)
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
    if ( retrlen != NULL )
        *retrlen = rlen;
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

double finPlotPolar::getRadianStep(double basestep, double rlen) const
{
    double radstep = basestep / rlen;
    return this->refineRadianStep(radstep);
}

finErrorCode finPlotPolar::plot()
{
    if ( !this->checkValid() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();

    finErrorCode errcode;
    double basestep = this->getBaseStep();

    finExecFunction *func = this->_environment->findFunction(this->_funcname);
    if ( func == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    QList<finExecVariable *> funcarglist;
    finExecVariable *radvar;
    errcode = this->buildFuncArgList(&funcarglist, &radvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true, loopit = true;
    double curstep = basestep;
    double rlen;
    QPointF curpt;

    for ( double rad = this->_fromRad; loopit; rad += curstep ) {
        if ( rad > this->_toRad ) {
            rad = this->_toRad;
            loopit = false;
        }

        errcode = this->calcAPoint(rad, func, &funcarglist, radvar, &rlen, &curpt, &goon);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;

        this->_stmPlot.appendPoint(curpt);

        // Radian is set to default when current or previous points do not exist.
        if ( loopit ) {
            if ( qIsNaN(curpt.x()) || qIsNaN(curpt.y()) || qIsInf(curpt.x()) || qIsInf(curpt.y()) )
                curstep = basestep;
            else
                curstep = this->getRadianStep(basestep, rlen);
        }
    }

    // Because all extended arguments are left values, we do not release the memory for arglist.
    errcode = this->_stmPlot.plot();
    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();
    return errcode;
}

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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
    return finErrorKits::EC_SUCCESS;
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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotParametric::setParameterVarIndex(int idx)
{
    this->_tIdx = idx;
    return finErrorKits::EC_SUCCESS;
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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotParametric::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotParametric::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotParametric::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotParametric::setFigureContainer(finFigureContainer *figcontainer)
{
    return this->_stmPlot.setFigureContainer(figcontainer);
}

bool finPlotParametric::checkValid() const
{
    if ( this->_funcname.isEmpty() )
        return false;
    if ( this->_callArgList == NULL || this->_environment == NULL || this->_machine == NULL ||
         this->_flowctl == NULL || this->_stmPlot.getFigureContainer() == NULL )
        return false;
    if ( this->_tIdx < 0 || this->_tIdx > this->_callArgList->count() )
        return false;

    return true;
}

double finPlotParametric::getBaseStep() const
{
    double step = 0.1;
    if ( this->_stmPlot.getFigureContainer() == NULL )
        return step;

    finGraphConfig *graphcfg = this->_stmPlot.getFigureContainer()->getGraphConfig();
    if ( graphcfg != NULL )
        step = 6.0 / graphcfg->getAxisUnitPixelSize();
    return step;
}

finErrorCode finPlotParametric::buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **tvar)
{
    if ( varlist == NULL || tvar == NULL )
        return finErrorKits::EC_NULL_POINTER;

    *tvar = new finExecVariable();
    if ( *tvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    (*tvar)->setName(QString());
    (*tvar)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    (*tvar)->setLeftValue();
    (*tvar)->clearWriteProtected();
    this->_environment->addVariable(*tvar);

    *varlist = *this->_callArgList;
    varlist->insert(this->_tIdx, *tvar);
    return finErrorKits::EC_SUCCESS;
}

double finPlotParametric::getCurrentStep(double basestep, double detlen, double prevstep)
{
    if ( detlen < 1.0e-8 )
        return M_PI / 18.0;

    double curstep = basestep * prevstep / detlen;
    if ( curstep < M_PI / 1800.0 )
        curstep = M_PI / 1800.0;
    else if ( curstep > M_PI / 18.0 )
        curstep = M_PI / 18.0;
    return curstep;
}

finErrorCode finPlotParametric::calcAPoint(double t, finExecFunction *func, QList<finExecVariable *> *varlist,
                                           finExecVariable *tvar, QPointF *pt, bool *goon)
{
    if ( varlist == NULL || tvar == NULL || pt == NULL || goon == NULL )
        return finErrorKits::EC_NULL_POINTER;

    finErrorCode errcode;
    tvar->setNumericValue(t);

    errcode = func->execFunction(varlist, this->_environment, this->_machine, this->_flowctl);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->_flowctl->checkFlowForExpress(goon, NULL, this->_machine);
    if ( finErrorKits::isErrorResult(errcode) || !(*goon) )
        return errcode;

    finExecVariable *retvar = this->_flowctl->pickReturnVariable();
    int retvararylen = 0;
    if ( retvar == NULL || !retvar->isNumericArray(&retvararylen) ) {
        finExecVariable::releaseNonLeftVariable(retvar);
        return finErrorKits::EC_INVALID_PARAM;
    }
    if ( retvararylen < 2 ) {
        finExecVariable::releaseNonLeftVariable(retvar);
        return finErrorKits::EC_INVALID_PARAM;
    }

    pt->setX(retvar->getVariableItemAt(0)->getNumericValue());
    pt->setY(retvar->getVariableItemAt(1)->getNumericValue());
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

bool finPlotParametric::isDuplicatedNaN(double t, const QPointF &prevpt, const QPointF &curpt)
{
    if ( t <= this->_fromT )
        return false;

    if ( qIsNaN(prevpt.x()) || qIsNaN(prevpt.y()) || qIsNaN(curpt.x()) || qIsNaN(curpt.y()) )
        return true;
    else
        return false;
}

double finPlotParametric::calcDetLength(double t, const QPointF &prevpt, const QPointF &curpt)
{
    if ( t <= this->_fromT )
        return 65535.0;

    if ( qIsNaN(prevpt.x()) || qIsNaN(prevpt.y()) || qIsNaN(curpt.x()) || qIsNaN(curpt.y()) ||
         qIsInf(prevpt.x()) || qIsInf(prevpt.y()) || qIsInf(curpt.x()) || qIsInf(curpt.y()) )
        return 65535.0;

    return finFigureAlg::pointsDistance(prevpt, curpt);
}

finErrorCode finPlotParametric::plot()
{
    if ( !this->checkValid() )
        return finErrorKits::EC_STATE_ERROR;

    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();

    finErrorCode errcode;
    double basestep = this->getBaseStep();

    finExecFunction *func = this->_environment->findFunction(this->_funcname);
    if ( func == NULL )
        return finErrorKits::EC_NOT_FOUND;

    QList<finExecVariable *> funcarglist;
    finExecVariable *tvar;
    errcode = this->buildFuncArgList(&funcarglist, &tvar);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true, loopit = true;
    double curstep = M_PI / 180.0;
    double detlen = 1.0;
    QPointF prevpt, curpt;

    for ( double t = this->_fromT; loopit; t += curstep ) {
        if ( t > this->_toT ) {
            t = this->_toT;
            loopit = false;
        }

        errcode = this->calcAPoint(t, func, &funcarglist, tvar, &curpt, &goon);
        if ( finErrorKits::isErrorResult(errcode) || !goon )
            return errcode;

        if ( !this->isDuplicatedNaN(t, prevpt, curpt) )
            this->_stmPlot.appendPoint(curpt);

        if ( loopit ) {
            detlen = this->calcDetLength(t, prevpt, curpt);
            curstep = this->getCurrentStep(basestep, detlen, curstep);
            prevpt = curpt;
        }
    }

    // Because all extended arguments are left values, we do not release the memory for arglist.
    errcode = this->_stmPlot.plot();
    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();
    return errcode;
}

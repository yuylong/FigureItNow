/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#include "finPlotFunction.h"

#include <qmath.h>
#include <QPointF>

#include "finExecFunction.h"
#include "finFigureAlg.h"
#include "finGraphConfig.h"


finPlotFunction::finPlotFunction()
    : _stmPlot()
{
    this->_funcname = QString();
    this->_fromX = 0.0;
    this->_toX = 0.0;
    this->_xidx = 0;
    this->_callArgList = nullptr;
    this->_environment = nullptr;
    this->_machine = nullptr;
    this->_flowctl = nullptr;
    this->_stmPlot.setFigureContainer(nullptr);
    this->_stmPlot.clearPoints();
    this->_stmPlot.clearBreakPoints();
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

void finPlotFunction::setFunctionName(const QString &funcname)
{
    this->_funcname = funcname;
}

void finPlotFunction::setFigureRange(double x1, double x2)
{
    if ( x1 <= x2 ) {
        this->_fromX = x1;
        this->_toX = x2;
    } else {
        this->_fromX = x2;
        this->_toX = x1;
    }
}

void finPlotFunction::setIndependentVarIndex(int idx)
{
    if ( idx < 0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, QString("Index cannot be negative."));

    this->_xidx = idx;
}

QList<finExecVariable *> *finPlotFunction::getCallArgList() const
{
    return this->_callArgList;
}

finExecEnvironment *finPlotFunction::getEnvironment() const
{
    return this->_environment;
}

finExecMachine *finPlotFunction::getMachine() const
{
    return this->_machine;
}

finExecFlowControl *finPlotFunction::getFlowControl() const
{
    return this->_flowctl;
}

finFigureContainer *finPlotFunction::getFigureContainer() const
{
    return this->_stmPlot.getFigureContainer();
}

void finPlotFunction::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
}

void finPlotFunction::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
}

void finPlotFunction::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
}

void finPlotFunction::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
}

void finPlotFunction::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_stmPlot.setFigureContainer(figcontainer);
}

bool finPlotFunction::checkValid() const
{
    if ( this->_funcname.isEmpty() )
        return false;
    if ( this->_callArgList == nullptr || this->_environment == nullptr || this->_machine == nullptr ||
         this->_flowctl == nullptr || this->_stmPlot.getFigureContainer() == nullptr )
        return false;
    if ( this->_xidx < 0 || this->_xidx > this->_callArgList->count() )
        return false;

    return true;
}

double finPlotFunction::getBaseStep() const
{
    double step = 0.1;
    if ( this->_stmPlot.getFigureContainer() == nullptr )
        return step;

    finGraphConfig *graphcfg = this->_stmPlot.getFigureContainer()->getGraphConfig();
    if ( graphcfg != nullptr )
        step = 6.0 / graphcfg->getAxisUnitPixelSize();
    return step;
}

void finPlotFunction::buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **xvar)
{
    if ( varlist == nullptr || xvar == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, QString("Null pointer in arguments."));

    *xvar = new finExecVariable();
    if ( *xvar == nullptr )
        finThrow(finErrorKits::EC_OUT_OF_MEMORY, QString("Failed to allocate memory for xvar."));

    (*xvar)->setName(QString());
    (*xvar)->setType(finExecVariable::TP_NUMERIC);
    (*xvar)->setLeftValue();
    (*xvar)->clearWriteProtected();
    this->_environment->addVariable(*xvar);

    *varlist = *this->_callArgList;
    varlist->insert(this->_xidx, *xvar);
}

double finPlotFunction::getCurrentStepWoRad(double basestep) const
{
    return basestep * 0.01;
}

double finPlotFunction::getCurrentStep(double rad, double basestep) const
{
    double curstep = basestep * cos(rad);
    double minstep = this->getCurrentStepWoRad(basestep);
    if ( curstep < minstep )
        return minstep;
    else
        return curstep;
}

finErrorCode finPlotFunction::calcAPoint(double x, finExecFunction *func, QList<finExecVariable *> *varlist,
                                         finExecVariable *xvar, QPointF *pt, bool *goon)
{
    if ( varlist == nullptr || xvar == nullptr || pt == nullptr || goon == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finErrorCode errcode;
    xvar->setNumericValue(x);

    errcode = func->execFunction(varlist, this->_environment, this->_machine, this->_flowctl);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->_flowctl->checkFlowForExpress(goon, nullptr, this->_machine);
    if ( finErrorKits::isErrorResult(errcode) || !(*goon) )
        return errcode;

    finExecVariable *retvar = this->_flowctl->pickReturnVariable();
    if ( retvar == nullptr || retvar->getType() != finExecVariable::TP_NUMERIC ) {
        finExecVariable::releaseNonLeftVariable(retvar);
        return finErrorKits::EC_INVALID_PARAM;
    }

    pt->setX(x);
    pt->setY(retvar->getNumericValue());
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotFunction::plot()
{
    if ( !this->checkValid() )
        return finErrorKits::EC_STATE_ERROR;

    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();

    finErrorCode errcode;
    double basestep = this->getBaseStep();

    finExecFunction *func = this->_environment->findFunction(this->_funcname);
    if ( func == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QList<finExecVariable *> funcarglist;
    finExecVariable *xvar;
    this->buildFuncArgList(&funcarglist, &xvar);

    bool goon = true, loopit = true;
    double curstep = this->getCurrentStepWoRad(basestep);
    double currad = M_PI * 0.499;
    QPointF prevpt, curpt;

    for ( double x = this->_fromX; loopit; x += curstep ) {
        if ( x > this->_toX ) {
            x = this->_toX;
            loopit = false;
        }

        errcode = this->calcAPoint(x, func, &funcarglist, xvar, &curpt, &goon);
        if ( finErrorKits::isErrorResult(errcode) || !goon )
            return errcode;

        // Avoid duplicated NaN points.
        if ( x > this->_fromX && !(qIsNaN(curpt.y()) && qIsNaN(prevpt.y())) )
            this->_stmPlot.appendPoint(curpt);

        // Radian is set to default when current or previous points do not exist.
        if ( loopit ) {
            if ( x <= this->_fromX ||
                 qIsNaN(curpt.y()) || qIsInf(curpt.y()) || qIsNaN(prevpt.y()) || qIsInf(prevpt.y()) )
                currad = M_PI * 0.499;
            else
                currad = finFigureAlg::getVectorRadian(curpt - prevpt);

            curstep = this->getCurrentStep(currad, basestep);
            prevpt = curpt;
        }
    }

    // Because all extended arguments are left values, we do not release the memory for arglist.
    errcode = this->_stmPlot.plot();
    this->_stmPlot.clearBreakPoints();
    this->_stmPlot.clearPoints();
    return errcode;
}

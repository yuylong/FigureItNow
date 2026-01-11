/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finPlotEquation2D.h"

#include <qmath.h>

#include "finExecFunction.h"


finPlotEquation2D::finPlotEquation2D()
    : _posListX(), _posListY(), _scrtPlot()
{
    this->_funcname = QString();
    this->_fromX = 0.0;
    this->_toX = 0.0;
    this->_fromY = 0.0;
    this->_toY = 0.0;
    this->_xidx = 0;
    this->_yidx = 1;
    this->_callArgList = nullptr;
    this->_environment = nullptr;
    this->_machine = nullptr;
    this->_flowctl = nullptr;
    this->_scrtPlot.setFigureContainer(nullptr);
    this->_scrtPlot.clearPoints();
}

const QString &finPlotEquation2D::getFunctionName() const
{
    return this->_funcname;
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

void finPlotEquation2D::setFunctionName(const QString &funcname)
{
    this->_funcname = funcname;
}

void finPlotEquation2D::setFiguringRangeX(double x1, double x2)
{
    if ( x1 <= x2 ) {
        this->_fromX = x1;
        this->_toX = x2;
    } else {
        this->_fromX = x2;
        this->_toX = x1;
    }
}

void finPlotEquation2D::setFiguringRangeY(double y1, double y2)
{
    if ( y1 <= y2 ) {
        this->_fromY = y1;
        this->_toY = y2;
    } else {
        this->_fromY = y2;
        this->_toY = y1;
    }
}

void finPlotEquation2D::setVariableXIndex(int idx)
{
    if ( idx < 0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Variable X index must be non-negative");

    this->_xidx = idx;
}

void finPlotEquation2D::setVariableYIndex(int idx)
{
    if ( idx < 0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Variable Y index must be non-negative");

    this->_yidx = idx;
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

void finPlotEquation2D::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
}

void finPlotEquation2D::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
}

void finPlotEquation2D::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
}

void finPlotEquation2D::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
}

void finPlotEquation2D::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_scrtPlot.setFigureContainer(figcontainer);
}

bool finPlotEquation2D::checkValid() const
{
    if ( this->_funcname.isEmpty() || this->_fromX >= this->_toX || this->_fromY >= this->_toY)
        return false;
    if ( this->_callArgList == nullptr || this->_environment == nullptr || this->_machine == nullptr ||
         this->_flowctl == nullptr || this->_scrtPlot.getFigureContainer() == nullptr )
        return false;
    if ( this->_xidx == this->_yidx ||
         this->_xidx < 0 || this->_xidx >= this->_callArgList->count() + 2 ||
         this->_yidx < 0 || this->_yidx >= this->_callArgList->count() + 2 )
        return false;

    return true;
}

double finPlotEquation2D::getBaseStep() const
{
    double step = 0.1;
    if ( this->_scrtPlot.getFigureContainer() == nullptr )
        return step;

    finGraphConfig *graphcfg = this->_scrtPlot.getFigureContainer()->getGraphConfig();
    if ( graphcfg != nullptr )
        step = 3.0 / graphcfg->getAxisUnitPixelSize();
    return step;
}

finErrorCode finPlotEquation2D::buildFuncArgList(QList<finExecVariable *> *varlist,
                                                 finExecVariable **xvar, finExecVariable **yvar)
{
    if ( varlist == nullptr || xvar == nullptr || yvar == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( this->_environment == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    *xvar = new finExecVariable();
    if ( *xvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;
    *yvar = new finExecVariable();
    if ( *yvar == nullptr ) {
        delete *xvar;
        return finErrorKits::EC_OUT_OF_MEMORY;
    }

    (*xvar)->setName(QString());
    (*xvar)->setType(finExecVariable::TP_NUMERIC);
    (*xvar)->setLeftValue();
    (*xvar)->clearWriteProtected();
    this->_environment->addVariable(*xvar);

    (*yvar)->setName(QString());
    (*yvar)->setType(finExecVariable::TP_NUMERIC);
    (*yvar)->setLeftValue();
    (*yvar)->clearWriteProtected();
    this->_environment->addVariable(*yvar);

    *varlist = *this->_callArgList;
    if ( this->_xidx < this->_yidx ) {
        varlist->insert(this->_xidx, *xvar);
        varlist->insert(this->_yidx, *yvar);
    } else {
        varlist->insert(this->_yidx, *yvar);
        varlist->insert(this->_xidx, *xvar);
    }
    return finErrorKits::EC_SUCCESS;
}


finErrorCode finPlotEquation2D::buildSearchPositions(double from, double to, double step, QList<double> *poslist)
{
    if ( poslist == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( step <= 0.0 )
        return finErrorKits::EC_INVALID_PARAM;

    poslist->clear();
    for ( double pos = from; pos < to; pos += step ) {
        poslist->append(pos);
    }
    poslist->append(to);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::buildSearchRangeList(double step)
{
    finErrorCode errcode;
    errcode = this->buildSearchPositions(this->_fromX, this->_toX, step, &this->_posListX);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->buildSearchPositions(this->_fromY, this->_toY, step, &this->_posListY);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::disposeSearchRangeList()
{
    this->_posListX.clear();
    this->_posListY.clear();
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::calcAPoint(
        double x, double y, finExecFunction *func, QList<finExecVariable *> *varlist,
        finExecVariable *xvar, finExecVariable *yvar, double *retval, bool *goon)
{
    if ( varlist == nullptr || xvar == nullptr || xvar == nullptr || retval == nullptr || goon == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finErrorCode errcode;
    xvar->setNumericValue(x);
    yvar->setNumericValue(y);

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

    *retval = retvar->getNumericValue();
    finExecVariable::releaseNonLeftVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

enum finPlotEquation2DSearchResFlag {
    FIN_PE_SRF_SELF   = 0x01,
    FIN_PE_SRF_SRCH_X = 0x02,
    FIN_PE_SRF_SRCH_Y = 0x04,
};

finErrorCode finPlotEquation2D::checkOnePosition(int xidx, int yidx, double curretval, QList<double> *prevretval,
                                                 unsigned long *srchflags)
{
    if ( xidx < 0 || yidx < 0 || (xidx > 0 && yidx >= prevretval->count()) )
        return finErrorKits::EC_INVALID_PARAM;
    if ( prevretval == nullptr || srchflags == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    *srchflags = 0x00;
    if ( curretval > -1.0e-8 && curretval < 1.0e-8 ) {
        *srchflags |= FIN_PE_SRF_SELF;
    } else {
        if ( yidx > 0 ) {
            double preyval = prevretval->at(yidx - 1);
            if ( !(preyval > -1.0e-8 && preyval < 1.0e-8) && (preyval * curretval < 0.0) )
                *srchflags |= FIN_PE_SRF_SRCH_Y;
        }
        if ( xidx > 0 ) {
            double prexval = prevretval->at(yidx);
            if ( !(prexval > -1.0e-8 && prexval < 1.0e-8) && (prexval * curretval < 0.0) )
                *srchflags |= FIN_PE_SRF_SRCH_X;
        }
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::updateRetValList(int xidx, int yidx, double curretval, QList<double> *prevretval)
{
    if ( xidx == 0 ) {
        while ( prevretval->count() < yidx )
            prevretval->append(0.0);
        prevretval->insert(yidx, curretval);
    } else {
        prevretval->replace(yidx, curretval);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::searchBinary(
        double x1, double y1, double retval1, double x2, double y2, double retval2,
        finExecFunction *func, QList<finExecVariable *> *varlist, finExecVariable *xvar, finExecVariable *yvar,
        double *xret, double *yret, bool *goon, int level)
{
    if ( func == nullptr || varlist == nullptr || xvar == nullptr || yvar == nullptr ||
         xret == nullptr || yret == nullptr || goon == nullptr)
        return finErrorKits::EC_NULL_POINTER;

    *goon = true;
    if ( retval1 > -1.0e-8 && retval1 < 1.0e-8) {
        *xret = x1;
        *yret = y1;
        return finErrorKits::EC_SUCCESS;
    } else if ( retval2 > -1.0e-8 && retval2 < 1.0e-8 ) {
        *xret = x1;
        *yret = y2;
        return finErrorKits::EC_SUCCESS;
    } else if ( retval1 * retval2 > 0.0 ) {
        return finErrorKits::EC_INVALID_PARAM;
    }

    if ( level > 5 ) {
        double absrv1 = fabs(retval1), absrv2 = fabs(retval2);
        *xret = (absrv1 * x1 + absrv2 * x2) / (absrv1 + absrv2);
        *yret = (absrv1 * y1 + absrv2 * y2) / (absrv1 + absrv2);
        return finErrorKits::EC_SUCCESS;
    }

    double xmid = (x1 + x2) / 2.0, ymid = (y1 + y2) / 2.0;
    double retvalmid = 0.0;
    finErrorCode errcode = this->calcAPoint(xmid, ymid, func, varlist, xvar, yvar, &retvalmid, goon);
    if ( finErrorKits::isErrorResult(errcode) || !(*goon) )
        return errcode;

    if ( retvalmid * retval1 < 0 ) {
        return this->searchBinary(x1, y1, retval1, xmid, ymid, retvalmid, func, varlist, xvar, yvar,
                                  xret, yret, goon, level + 1);
    } else if ( retvalmid * retval2 < 0 ) {
        return this->searchBinary(xmid, ymid, retvalmid, x2, y2, retval2, func, varlist, xvar, yvar,
                                  xret, yret, goon, level + 1);
    }
    *xret = xmid;
    *yret = ymid;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finPlotEquation2D::plot()
{
    if ( !this->checkValid() )
        return finErrorKits::EC_STATE_ERROR;

    finExecFunction *func = this->_environment->findFunction(this->_funcname);
    if ( func == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    finErrorCode errcode;
    double basestep = this->getBaseStep();
    this->_scrtPlot.setDistanceLimit(basestep * 3.0);

    errcode = this->buildSearchRangeList(basestep);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    QList<finExecVariable *> funcarglist;
    finExecVariable *xvar, *yvar;
    errcode = this->buildFuncArgList(&funcarglist, &xvar, &yvar);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    int xposcnt = this->_posListX.count(), yposcnt = this->_posListY.count();
    QList<double> retvallist;
    double curretval;
    bool goon = true;
    unsigned long srchflags = 0x00;

    this->_scrtPlot.clearPoints();
    for ( int xidx = 0; xidx < xposcnt; xidx++ ) {
        const double &xval = this->_posListX.at(xidx);

        for ( int yidx = 0; yidx < yposcnt; yidx++ ) {
            const double &yval = this->_posListY.at(yidx);

            errcode = this->calcAPoint(xval, yval, func, &funcarglist, xvar, yvar, &curretval, &goon);
            if ( finErrorKits::isErrorResult(errcode) || !goon )
                return errcode;

            errcode = this->checkOnePosition(xidx, yidx, curretval, &retvallist, &srchflags);
            if ( finErrorKits::isErrorResult(errcode) )
                return errcode;

            double realx = xval, realy = yval;
            if ( srchflags & FIN_PE_SRF_SELF ) {
                this->_scrtPlot.appendPoint(xval, yval);
            }
            if ( xidx > 0 && (srchflags & FIN_PE_SRF_SRCH_X) ) {
                errcode = this->searchBinary(this->_posListX.at(xidx - 1), yval, retvallist.at(yidx),
                                             xval, yval, curretval, func, &funcarglist, xvar, yvar,
                                             &realx, &realy, &goon);
                if ( finErrorKits::isErrorResult(errcode) || !goon )
                    return errcode;

                this->_scrtPlot.appendPoint(realx, realy);
            }
            if ( yidx > 0 && (srchflags & FIN_PE_SRF_SRCH_Y) ) {
                errcode = this->searchBinary(xval, this->_posListY.at(yidx - 1), retvallist.at(yidx - 1),
                                             xval, yval, curretval, func, &funcarglist, xvar, yvar,
                                             &realx, &realy, &goon);
                if ( finErrorKits::isErrorResult(errcode) || !goon )
                    return errcode;

                this->_scrtPlot.appendPoint(realx, realy);
            }

            errcode = this->updateRetValList(xidx, yidx, curretval, &retvallist);
            if ( finErrorKits::isErrorResult(errcode) )
                return errcode;
        }
    }

    return this->_scrtPlot.plot();
}

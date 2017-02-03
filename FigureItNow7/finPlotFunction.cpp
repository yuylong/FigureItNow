#include "finPlotFunction.h"

#include "finExecFunction.h"
#include "finFigureObject.h"
#include "finGraphConfig.h"


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
    return this->_figcontainer;
}

finErrorCode finPlotFunction::setCallArgList(QList<finExecVariable *> *arglist)
{
    this->_callArgList = arglist;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::setEnvironment(finExecEnvironment *env)
{
    this->_environment = env;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::setMachine(finExecMachine *machine)
{
    this->_machine = machine;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::setFlowControl(finExecFlowControl *flowctl)
{
    this->_flowctl = flowctl;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_figcontainer = figcontainer;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finPlotFunction::checkValid() const
{
    if ( this->_funcname.isEmpty() )
        return false;
    if ( this->_callArgList == NULL || this->_environment == NULL || this->_machine == NULL ||
         this->_flowctl == NULL || this->_figcontainer == NULL )
        return false;
    if ( this->_xidx < 0 || this->_xidx > this->_callArgList->count() )
        return false;

    return true;
}

double finPlotFunction::getCurrentStep() const
{
    double step = 0.1;
    if ( this->_figcontainer == NULL )
        return step;

    finGraphConfig *graphcfg = this->_figcontainer->getGraphConfig();
    if ( graphcfg != NULL )
        step = 3.0 * 1.0 / graphcfg->getAxisUnitPixelSize();
    return step;
}

finErrorCode finPlotFunction::buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **xvar)
{
    if ( varlist == NULL || xvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    *xvar = new finExecVariable();
    if ( *xvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    (*xvar)->setName("__fig_func_drv_arg_x");
    (*xvar)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    (*xvar)->clearLeftValue();
    (*xvar)->setWriteProtected();

    *varlist = *this->_callArgList;
    varlist->insert(this->_xidx, *xvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotFunction::plot()
{
    if ( !this->checkValid() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finErrorCode errcode;
    double step = this->getCurrentStep();

    finExecFunction *func = this->_environment->findFunction(this->_funcname);
    if ( func == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    QList<finExecVariable *> funcarglist;
    finExecVariable *xvar;
    errcode = this->buildFuncArgList(&funcarglist, &xvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finFigureObjectPolyline *funcfigobj = new finFigureObjectPolyline();
    if ( funcfigobj == NULL ) {
        delete xvar;
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    }

    for ( double x = this->_fromX; x <= this->_toX; x += step ) {
        xvar->setNumericValue(x);

        errcode = func->execFunction(&funcarglist, this->_environment, this->_machine, this->_flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete funcfigobj;
            delete xvar;
            return errcode;
        }

        bool goon = true;
        errcode = this->_flowctl->checkFlowForExpress(&goon, NULL, this->_machine);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon ) {
            delete funcfigobj;
            delete xvar;
            return errcode;
        }

        finExecVariable *retvar = this->_flowctl->pickReturnVariable();
        if ( retvar == NULL || retvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ) {
            finExecVariable::releaseNonLeftVariable(retvar);
            delete funcfigobj;
            delete xvar;
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        }

        double y = retvar->getNumericValue();
        finExecVariable::releaseNonLeftVariable(retvar);
        funcfigobj->appendPoint(x, y);
    }
    delete xvar;

    errcode = this->_figcontainer->appendFigureObject(funcfigobj);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete funcfigobj;
        return errcode;
    }

    // Because all extended arguments are left values, we do not release the memory for arglist.
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

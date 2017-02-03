#include "finPlotFunction.h"

#include "finExecFunction.h"

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

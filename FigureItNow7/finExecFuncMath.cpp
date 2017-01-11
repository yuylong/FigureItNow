
#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecFlowControl.h"

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl);


static struct finExecSysFuncRegItem _finSysFuncMathList[] = {
    { QString("sin"), QString("rad"), _sysfunc_sin },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncMath()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMathList);
}

static finErrorCode _sysfunc_sin(finExecFunction *self, finExecEnvironment *env,
                                 finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *rad, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rad = env->findVariable("rad")->getLinkTarget();
    if ( rad == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(sin(rad->getNumericValue()));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

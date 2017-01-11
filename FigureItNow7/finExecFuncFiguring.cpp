
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_line(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);

static finExecSysFuncRegItem _finSysFuncFigureList[] = {
    { QString("line"),    QString("x1,y1,x2,y2"), _sysfunc_line    },

    { QString(), QString(), NULL }
};


finErrorCode finExecFunction::registSysFuncFiguring()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFigureList);
}

static finErrorCode
_sysfunc_line(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *x1, *y1, *x2, *y2;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    x1 = env->findVariable("x1")->getLinkTarget();
    y1 = env->findVariable("y1")->getLinkTarget();
    x2 = env->findVariable("x2")->getLinkTarget();
    y2 = env->findVariable("y2")->getLinkTarget();

    if ( x1 == NULL || y1 == NULL || x2 == NULL || y2 == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( x1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         x2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectLine *foline = new finFigureObjectLine();
    if ( foline == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    foline->setPoint1(x1->getNumericValue(), y1->getNumericValue());
    foline->setPoint2(x2->getNumericValue(), y2->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(foline);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foline;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

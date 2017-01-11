
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncMatrixList[] = {
    { QString("mat_add"), QString("mat1,mat2"), _sysfunc_mat_add },

    { QString(), QString(), NULL }
};


finErrorCode finExecFunction::registSysFuncMatrix()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMatrixList);
}

static finErrorCode
_sysfunc_mat_add (finExecFunction *self, finExecEnvironment *env, finExecMachine *machine,
                  finExecFlowControl *flowctl)
{
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = env->findVariable("mat1")->getLinkTarget();
    mat2var = env->findVariable("mat2")->getLinkTarget();

    if ( mat1var == NULL || mat2var == NULL ) {
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }

    if ( mat1var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY ||
         mat2var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

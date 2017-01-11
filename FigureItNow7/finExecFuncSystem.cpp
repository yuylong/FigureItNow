
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

//static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
//                                     finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncSystemList[] = {
//    { QString("mat_add"), QString("mat1,mat2"), _sysfunc_mat_add },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncSystem()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncSystemList);
}

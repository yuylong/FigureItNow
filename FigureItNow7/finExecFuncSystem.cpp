
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_run_function(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncSystemList[] = {
    { QString("runfunction"), QString("funcname"), _sysfunc_run_function },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncSystem()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncSystemList);
}

static finErrorCode _sysfunc_run_function(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    fnvar = env->findVariable("funcname")->getLinkTarget();
    if ( fnvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    QString funcname = fnvar->getStringValue();

    finExecFunction *func = env->findFunction(funcname);
    if ( func == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    QList<finExecVariable *> arglist;
    finExecVariable *argvar;
    int idx = 0;
    while ( true ) {
        argvar = finExecFunction::getExtendArgAt(env, idx);
        if ( argvar == NULL )
            break;

        arglist.append(argvar);
        idx++;
    }

    return func->execFunction(&arglist, env, machine, flowctl);
}

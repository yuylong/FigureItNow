
#include "finExecFunction.h"

#include <QString>
#include <QStringList>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_run_function(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ext_arg(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ext_arg_count(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_call_stack_count(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_call_stack(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);


static struct finExecSysFuncRegItem _finSysFuncSystemList[] = {
    { QString("run_function"),     QString("funcname"), _sysfunc_run_function     },
    { QString("ext_arg"),          QString("idx"),      _sysfunc_ext_arg          },
    { QString("ext_arg_count"),    QString(""),         _sysfunc_ext_arg_count    },
    { QString("call_stack_count"), QString(""),         _sysfunc_call_stack_count },
    { QString("call_stack"),       QString(""),         _sysfunc_call_stack       },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncSystem()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncSystemList);
}

static finErrorCode _sysfunc_run_function(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *fnvar = finExecVariable::transLinkTarget(env->findVariable("funcname"));
    if ( fnvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

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

static finErrorCode _sysfunc_ext_arg(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *idxvar = finExecVariable::transLinkTarget(env->findVariable("idx"));
    if ( idxvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( idxvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    int idx = (int)idxvar->getNumericValue();
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = finExecFunction::getPreviousExtendArgAt(env, idx);
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_ext_arg_count(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    int extargcnt = finExecFunction::getPreviousExtendArgCount(env);
    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(extargcnt);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_call_stack_count(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    int funclevelcnt = env->getTotalBelongFunctionLevelCount() - 1;
    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(funclevelcnt);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_call_stack(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    retvar->setType(finExecVariable::FIN_VR_TYPE_ARRAY);

    QStringList funcnamelist;
    finErrorCode errcode = env->getBelongFunctionList(&funcnamelist);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->preallocArrayLength(funcnamelist.count());
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    for ( int i = 0; i < funcnamelist.count(); i++ ) {
        finExecVariable *subvar = retvar->getVariableItemAt(i);
        if ( subvar == NULL ) {
            errcode = finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
            goto err;
        }

        subvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
        subvar->setStringValue(funcnamelist.at(i));
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;

err:
    delete retvar;
    return errcode;
}

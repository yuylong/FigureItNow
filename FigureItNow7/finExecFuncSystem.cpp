/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <QDebug>
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
static finErrorCode _sysfunc_call_print_info(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_call_print_warn(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_call_print_err(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncSystemList[] = {
    { QString("run_function"),     QString("funcname"), _sysfunc_run_function     },
    { QString("ext_arg"),          QString("idx"),      _sysfunc_ext_arg          },
    { QString("ext_arg_count"),    QString(""),         _sysfunc_ext_arg_count    },
    { QString("call_stack_count"), QString(""),         _sysfunc_call_stack_count },
    { QString("call_stack"),       QString(""),         _sysfunc_call_stack       },

    { QString("print"),            QString(""),         _sysfunc_call_print_info  },
    { QString("print_info"),       QString(""),         _sysfunc_call_print_info  },
    { QString("print_warn"),       QString(""),         _sysfunc_call_print_warn  },
    { QString("print_err"),        QString(""),         _sysfunc_call_print_err   },

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

    QList<finExecVariable *> arglist = finExecFunction::getExtendArgList(env);

    // All the extended arguments are left values, we do not release the memory here.
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

enum _finDebugLevel {
    FIN_DBGLVL_INFO,
    FIN_DBGLVL_WARNING,
    FIN_DBGLVL_ERROR,
};

static inline QDebug _sysfunc_call_print_base_getOutStream(_finDebugLevel dbglevel)
{
    switch ( dbglevel ) {
      case FIN_DBGLVL_INFO:
        return qInfo().noquote();
        break;

      case FIN_DBGLVL_WARNING:
        return qWarning().noquote();
        break;

      case FIN_DBGLVL_ERROR:
        return qCritical().noquote();
        break;

      default:
        return qDebug().noquote();
        break;
    }
}

static inline finErrorCode _sysfunc_call_print_base_printProcess(QDebug &outstm, finExecVariable *var)
{
    if ( var == NULL )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    switch ( var->getType() ) {
      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        outstm << var->getNumericValue();
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        outstm << var->getStringValue();
        break;

      case finExecVariable::FIN_VR_TYPE_IMAGE:
        outstm.nospace() << "Image: " << var->getImageValue().width() << "x" << var->getImageValue().height();
        break;

      case finExecVariable::FIN_VR_TYPE_ARRAY:
        outstm.nospace() << "{ ";
        for ( int i = 0; i < var->getArrayLength(); i++ ) {
            if ( i > 0 )
                outstm.nospace() << ", ";
            _sysfunc_call_print_base_printProcess(outstm.nospace(), var->getVariableItemAt(i));
        }
        outstm.nospace() << " }";
        break;

      case finExecVariable::FIN_VR_TYPE_LINK:
        _sysfunc_call_print_base_printProcess(outstm, finExecVariable::transLinkTarget(var));
        break;

      default:
        break;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_call_print_base(_finDebugLevel dbglevel, finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QDebug outstm = _sysfunc_call_print_base_getOutStream(dbglevel);

    finExecVariable *var;
    int varcnt = finExecFunction::getExtendArgCount(env);
    for ( int i = 0; i < varcnt; i++ ) {
        var = finExecVariable::transLinkTarget(finExecFunction::getExtendArgAt(env, i));

        finErrorCode errcode = _sysfunc_call_print_base_printProcess(outstm, var);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_call_print_info(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_call_print_base(FIN_DBGLVL_INFO, self, env, machine, flowctl);
}

static finErrorCode _sysfunc_call_print_warn(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_call_print_base(FIN_DBGLVL_WARNING, self, env, machine, flowctl);
}

static finErrorCode _sysfunc_call_print_err(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_call_print_base(FIN_DBGLVL_ERROR, self, env, machine, flowctl);
}

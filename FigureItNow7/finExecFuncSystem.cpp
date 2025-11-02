/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <QDebug>
#include <QString>
#include <QStringList>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static QString _defFuncCtg("System");

static finErrorCode _sysfunc_run_function(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *fnvar = finExecVariable::transLinkTarget(env->findVariable("funcname"));
    if ( fnvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString funcname = fnvar->getStringValue();

    finExecFunction *func = env->findFunction(funcname);
    if ( func == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QList<finExecVariable *> arglist = finExecFunction::getExtendArgList(env);

    // All the extended arguments are left values, we do not release the memory here.
    return func->execFunction(&arglist, env, machine, flowctl);
}

static struct finExecSysFuncRegItem _funcRegItem_run_function = {
    /*._funcName     =*/ QString("run_function"),
    /*._paramCsvList =*/ QString("funcname"),
    /*._funcCall     =*/ _sysfunc_run_function,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("run_function (funcname, ...)"),
    /*._description  =*/ QString("Invoke a given function along with a sequence of arguments."),
};

static finErrorCode _sysfunc_ext_arg(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *idxvar = finExecVariable::transLinkTarget(env->findVariable("idx"));
    if ( idxvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( idxvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    int idx = (int)idxvar->getNumericValue();
    if ( idx < 0 )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *retvar = finExecFunction::getPreviousExtendArgAt(env, idx);
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_ext_arg = {
    /*._funcName     =*/ QString("ext_arg"),
    /*._paramCsvList =*/ QString("idx"),
    /*._funcCall     =*/ _sysfunc_ext_arg,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ext_arg (idx)"),
    /*._description  =*/ QString("Get the extended variable at the given index of the current function invocation."),
};

static finErrorCode _sysfunc_ext_arg_count(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    int extargcnt = finExecFunction::getPreviousExtendArgCount(env);
    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(extargcnt);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_ext_arg_count = {
    /*._funcName     =*/ QString("ext_arg_count"),
    /*._paramCsvList =*/ QString(""),
    /*._funcCall     =*/ _sysfunc_ext_arg_count,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ext_arg_count ()"),
    /*._description  =*/ QString("Get the number of extended arguments of the current function invocation."),
};

static finErrorCode _sysfunc_call_stack_count(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    /* Kick out the current 'call_stack' function from count. */
    int funclevelcnt = env->getTotalBelongFunctionLevelCount() - 1;
    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(funclevelcnt);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_call_stack_count = {
    /*._funcName     =*/ QString("call_stack_count"),
    /*._paramCsvList =*/ QString(""),
    /*._funcCall     =*/ _sysfunc_call_stack_count,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("call_stack_count ()"),
    /*._description  =*/ QString("Get the number of functions in call stack of the current function invocation."),
};

static finErrorCode _sysfunc_call_stack(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;
    retvar->setType(finExecVariable::TP_ARRAY);

    QStringList funcnamelist;
    finErrorCode errcode = env->getBelongFunctionList(&funcnamelist);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    /* Kick out the current 'call_stack' function from list. */
    funcnamelist.removeFirst();

    errcode = retvar->preallocArrayLength(funcnamelist.count());
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    for ( int i = 0; i < funcnamelist.count(); i++ ) {
        finExecVariable *subvar = retvar->getVariableItemAt(i);
        if ( subvar == nullptr ) {
            errcode = finErrorKits::EC_OUT_OF_MEMORY;
            goto err;
        }

        subvar->setType(finExecVariable::TP_STRING);
        subvar->setStringValue(funcnamelist.at(i));
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;

err:
    delete retvar;
    return errcode;
}

static struct finExecSysFuncRegItem _funcRegItem_call_stack = {
    /*._funcName     =*/ QString("call_stack"),
    /*._paramCsvList =*/ QString(""),
    /*._funcCall     =*/ _sysfunc_call_stack,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("call_stack ()"),
    /*._description  =*/ QString("Get function names in current call stack as a string array variable."),
};

enum _finDebugLevel {
    _DBGLVL_INFO,
    _DBGLVL_WARNING,
    _DBGLVL_ERROR,
};

static inline QDebug _sysfunc_print_base_getOutStream(_finDebugLevel dbglevel)
{
    switch ( dbglevel ) {
      case _DBGLVL_INFO:
        return qInfo().noquote();
        break;

      case _DBGLVL_WARNING:
        return qWarning().noquote();
        break;

      case _DBGLVL_ERROR:
        return qCritical().noquote();
        break;

      default:
        return qDebug().noquote();
        break;
    }
}

static inline finErrorCode _sysfunc_print_base_printProcess(QDebug &outstm, finExecVariable *var)
{
    if ( var == nullptr )
        return finErrorKits::EC_NORMAL_WARN;

    switch ( var->getType() ) {
      case finExecVariable::TP_NUMERIC:
        outstm << var->getNumericValue();
        break;

      case finExecVariable::TP_STRING:
        outstm << var->getStringValue();
        break;

      case finExecVariable::TP_IMAGE:
        outstm.nospace() << "Image: " << var->getImageValue().width() << "x" << var->getImageValue().height();
        break;

      case finExecVariable::TP_ARRAY:
        outstm.nospace() << "{ ";
        for ( int i = 0; i < var->getArrayLength(); i++ ) {
            if ( i > 0 )
                outstm.nospace() << ", ";
            _sysfunc_print_base_printProcess(outstm.nospace(), var->getVariableItemAt(i));
        }
        outstm.nospace() << " }";
        break;

      case finExecVariable::TP_LINK:
        _sysfunc_print_base_printProcess(outstm, finExecVariable::transLinkTarget(var));
        break;

      default:
        break;
    }
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_print_base(_finDebugLevel dbglevel, finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    QDebug outstm = _sysfunc_print_base_getOutStream(dbglevel);

    finExecVariable *var;
    int varcnt = finExecFunction::getExtendArgCount(env);
    for ( int i = 0; i < varcnt; i++ ) {
        var = finExecVariable::transLinkTarget(finExecFunction::getExtendArgAt(env, i));

        finErrorCode errcode = _sysfunc_print_base_printProcess(outstm, var);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_print_info(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_print_base(_DBGLVL_INFO, self, env, machine, flowctl);
}

static struct finExecSysFuncRegItem _funcRegItem_print = {
    ._funcName     = QString("print"),
    ._paramCsvList = QString(""),
    ._funcCall     = _sysfunc_print_info,
    ._category     = _defFuncCtg,
    ._prototype    = QString("print (...)"),
    ._description  = QString("Output the strings, numbers, or arrays into the standard output device. Each print "
                             "will create a singal line in output, and is allowed contains multiple variables to "
                             "print. The default print is under the output level \'INFO\'."),
};

static struct finExecSysFuncRegItem _funcRegItem_print_info = {
    ._funcName     = QString("print_info"),
    ._paramCsvList = QString(""),
    ._funcCall     = _sysfunc_print_info,
    ._category     = _defFuncCtg,
    ._prototype    = QString("print_info (...)"),
    ._description  = QString("Output the strings, numbers, or arrays into the standard output device under the "
                             "output level \'INFO\'. Each print will create a singal line in output, and is "
                             "allowed contains multiple variables to print."),
};

static finErrorCode _sysfunc_print_warn(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_print_base(_DBGLVL_WARNING, self, env, machine, flowctl);
}

static struct finExecSysFuncRegItem _funcRegItem_print_warn = {
    ._funcName     = QString("print_warn"),
    ._paramCsvList = QString(""),
    ._funcCall     = _sysfunc_print_warn,
    ._category     = _defFuncCtg,
    ._prototype    = QString("print_warn (...)"),
    ._description  = QString("Output the strings, numbers, or arrays into the standard output device under the "
                             "output level \'WARN\'. Each print will create a singal line in output, and is "
                             "allowed contains multiple variables to print."),
};

static finErrorCode _sysfunc_print_err(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_print_base(_DBGLVL_ERROR, self, env, machine, flowctl);
}

static struct finExecSysFuncRegItem _funcRegItem_print_err = {
    ._funcName     = QString("print_err"),
    ._paramCsvList = QString(""),
    ._funcCall     = _sysfunc_print_err,
    ._category     = _defFuncCtg,
    ._prototype    = QString("print_err (...)"),
    ._description  = QString("Output the strings, numbers, or arrays into the standard error output device under "
                             "the output level \'ERROR\'. Each print will create a singal line in output, and is "
                             "allowed contains multiple variables to print."),
};

static struct finExecSysFuncRegItem _finSysFuncSystemList[] = {
    _funcRegItem_run_function,
    _funcRegItem_ext_arg,
    _funcRegItem_ext_arg_count,
    _funcRegItem_call_stack_count,
    _funcRegItem_call_stack,

    _funcRegItem_print,
    _funcRegItem_print_info,
    _funcRegItem_print_warn,
    _funcRegItem_print_err,

    { QString(), QString(), nullptr, _defFuncCtg, QString(), QString() }
};

void finExecFunction::registSysFuncSystem()
{
    finExecFunction::registSysFuncFromArray(_finSysFuncSystemList, _defFuncCtg);
}

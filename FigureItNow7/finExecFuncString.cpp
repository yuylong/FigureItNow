
#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_str_len(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_left(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_right(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncStringList[] = {
    { QString("str_len"),   QString("str"),     _sysfunc_str_len   },
    { QString("str_left"),  QString("str,len"), _sysfunc_str_left  },
    { QString("str_right"), QString("str,len"), _sysfunc_str_right },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncString()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncStringList);
}

static finErrorCode _sysfunc_str_len(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    if ( strvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    QString str = strvar->getStringValue();

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue((double)str.length());
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_str_left(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *lenvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    lenvar = finExecVariable::transLinkTarget(env->findVariable("len"));
    if ( strvar == NULL || lenvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::FIN_VR_TYPE_STRING ||
         lenvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    QString str = strvar->getStringValue();
    int len = (int)floor(lenvar->getNumericValue());
    if ( len < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
    if ( len >= str.length() )
        retvar->setStringValue(str);
    else
        retvar->setStringValue(str.left(len));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_str_right(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *lenvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    lenvar = finExecVariable::transLinkTarget(env->findVariable("len"));
    if ( strvar == NULL || lenvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::FIN_VR_TYPE_STRING ||
         lenvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    QString str = strvar->getStringValue();
    int len = (int)floor(lenvar->getNumericValue());
    if ( len < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    retvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
    if ( len >= str.length() )
        retvar->setStringValue(str);
    else
        retvar->setStringValue(str.right(len));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

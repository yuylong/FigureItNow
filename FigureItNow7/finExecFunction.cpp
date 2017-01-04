/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2016 Yusoft. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161226    0  Yulong Yu    Create this file.
 */

#include "finExecFunction.h"

#include "finLexNode.h"
#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecOperartorClac.h"


QString finExecFunction::_extArgPrefix("__ext_arg_");

finExecFunction::finExecFunction()
{
    this->_type = FIN_FN_TYPE_DUMMY;
    this->_u._rawPointer = NULL;
}

finExecFunctionType finExecFunction::getFunctionType() const
{
    return this->_type;
}

QString finExecFunction::getFunctionName() const
{
    return this->_funcName;
}

int finExecFunction::getParameterCount() const
{
    return this->_paramList.count();
}

QString finExecFunction::getParameterName(int idx) const
{
    if ( idx < 0 )
        return QString();
    else if ( idx < this->_paramList.count() )
        return this->_paramList.at(idx);
    else
        return finExecFunction::_extArgPrefix.append(QString::number(idx - this->_paramList.count()));
}

finErrorCode finExecFunction::setFunctionType(finExecFunctionType type)
{
    if ( this->_type == type )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_type = type;
    this->_u._rawPointer = NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionName(const QString &funcname)
{
    if ( QString::compare(this->_funcName, funcname) == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_funcName = funcname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::appendParameterName(const QString &paramname)
{
    if ( paramname.isEmpty() || paramname.isNull() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_paramList.append(paramname);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::clearParameterNames()
{
    if ( this->_paramList.count() == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_paramList.clear();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionSyntaxNode(finSyntaxNode *funcnode)
{
    if ( this->_type != finExecFunction::FIN_FN_TYPE_USER )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_u._funcNode = funcnode;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionCall(finFunctionCall funccall)
{
    if ( this->_type != finExecFunction::FIN_FN_TYPE_SYSTEM )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_u._funcCall = funccall;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::execFunction(finSyntaxNode *argnode, finExecEnvironment *env, finExecMachine *machine,
                              finExecVariable **retval, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();

    if ( argnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR ||
         lexnode->getOperator() != finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finExecEnvironment *subenv;
    errcode = env->buildChildEnvironment(&subenv);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        machine->appendExecutionError(lexnode, QString("Internal error."));
        return errcode;
    }

    if ( argnode->getSubListCount() > 0 ) {
        errcode = this->processArgsInSubEnv(argnode->getSubSyntaxNode(0), subenv, machine);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        }
    }

    if ( this->_type == finExecFunction::FIN_FN_TYPE_SYSTEM ) {
        errcode = this->execSysFunction(subenv, machine, retval, flowctl);
    } else if ( this->_type == finExecFunction::FIN_FN_TYPE_USER ) {
        errcode = this->execUserFunction(subenv, machine, retval, flowctl);
    } else {
        machine->appendExecutionError(lexnode, QString("Function type unrecognized."));
        errcode = finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    delete subenv;
    return errcode;
}

finErrorCode
finExecFunction::processArgsInSubEnv(finSyntaxNode *argnode, finExecEnvironment *env, finExecMachine *machine)
{
    if ( argnode == NULL )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finLexNode *lexnode = argnode->getCommandLexNode();
    if ( argnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finErrorCode errcode;
    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
        for ( int i = 0; i < argnode->getSubListCount(); i++ ) {
            errcode = this->appendArgToSubenv(i, argnode->getSubSyntaxNode(i), env, machine);
            if ( finErrorCodeKits::isErrorResult(errcode) )
                return errcode;
        }
    } else {
        errcode = this->appendArgToSubenv(0, argnode, env, machine);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::appendArgToSubenv(int idx, finSyntaxNode *argnode, finExecEnvironment *env, finExecMachine *machine)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();
    finExecVariable *expvar, *argvar;
    finExecFlowControl flowctl;

    errcode = machine->instantExecute(argnode, env->getParentEnvironment(), &expvar, &flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    if ( expvar->isLeftValue() ) {
        argvar = new finExecVariable();
        if ( argvar == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        errcode = argvar->copyVariable(expvar);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            machine->appendExecutionError(lexnode, QString("Internal error."));
            delete argvar;
            return errcode;
        }
    } else {
        argvar = expvar;
    }
    argvar->clearWriteProtected();
    argvar->setLeftValue();
    argvar->setName(this->getParameterName(idx));

    errcode = env->addVariable(argvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        machine->appendExecutionError(lexnode, QString("Internal error."));
        delete argvar;
        return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::execUserFunction(finExecEnvironment *env, finExecMachine *machine,
                                  finExecVariable **retval, finExecFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecFunction::execSysFunction(finExecEnvironment *env, finExecMachine *machine,
                                 finExecVariable **retval, finExecFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

QString finExecFunction::getExtArgPrefix()
{
    return finExecFunction::_extArgPrefix;
}

static finErrorCode
_sysfunc_mat_add (finExecFunction *self, finExecVariable *retval, finExecEnvironment *execenv);

static struct {
    QString _funcName;
    QString _paramCsvList;
    finFunctionCall _funcCall;
} _finSystemFunctionList[] = {
    { QString("mat_add"), QString("mat1,mat2"), _sysfunc_mat_add },

    { QString(), QString(), NULL }
};

finErrorCode
finExecFunction::installSystemFunctions (finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int i, succcnt = 0;

    if ( rootenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    for ( i = 0; !_finSystemFunctionList[i]._funcName.isNull(); i++ ) {

        QStringList paramlist;
        finExecFunction *curfunc = new finExecFunction();
        if ( curfunc == NULL )
            goto item_bad;

        errcode = curfunc->setFunctionType(finExecFunction::FIN_FN_TYPE_SYSTEM);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = curfunc->setFunctionName(_finSystemFunctionList[i]._funcName);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        paramlist = _finSystemFunctionList[i]._paramCsvList.split(',');
        curfunc->clearParameterNames();
        for ( int j = 0; j < paramlist.count(); j++ ) {
            errcode = curfunc->appendParameterName(paramlist.at(i));
            if ( finErrorCodeKits::isErrorResult(errcode) )
                goto item_bad;
        }

        errcode = curfunc->setFunctionCall(_finSystemFunctionList[i]._funcCall);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = rootenv->addFunction(curfunc);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        succcnt++;
        continue;

item_bad:
        if ( curfunc != NULL )
            delete curfunc;
    }

    if ( succcnt == i ) {
        if ( succcnt > 0 )
            return finErrorCodeKits::FIN_EC_SUCCESS;
        else
            return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    } else {
        if ( succcnt > 0 )
            return finErrorCodeKits::FIN_EC_NORMAL_WARN;
        else
            return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }
}

static finErrorCode
_sysfunc_mat_add (finExecFunction *self, finExecVariable *retval, finExecEnvironment *execenv)
{
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || retval == NULL || execenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = execenv->findVariable("mat1");
    mat2var = execenv->findVariable("mat2");

    if ( mat1var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY ||
         mat2var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

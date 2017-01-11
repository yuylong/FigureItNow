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

bool finExecFunction::isParameterExist(const QString &paramname) const
{
    for ( int i = 0; i < this->_paramList.count(); i++ ) {
        if ( QString::compare(this->_paramList.at(i), paramname) == 0 )
            return true;
    }
    return false;
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
                              finExecFlowControl *flowctl)
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
    subenv->setEnvironmentName(this->_funcName);

    if ( argnode->getSubListCount() > 0 ) {
        errcode = this->processArgsInSubEnv(argnode->getSubSyntaxNode(0), subenv, machine, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        }

        bool arggoon = true;
        errcode = flowctl->checkFlowForExpress(&arggoon, lexnode, machine);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        } else if ( !arggoon ) {
            flowctl->retVarSwitchEnv(subenv);
            delete subenv;
            return errcode;
        }
    }

    flowctl->resetFlowControl();
    if ( this->_type == finExecFunction::FIN_FN_TYPE_SYSTEM ) {
        errcode = this->execSysFunction(subenv, machine, flowctl);
    } else if ( this->_type == finExecFunction::FIN_FN_TYPE_USER ) {
        errcode = this->execUserFunction(subenv, machine, flowctl);
    } else {
        machine->appendExecutionError(lexnode, QString("ERROR: Function type cannot be recognized."));
        delete subenv;
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }

    errcode = flowctl->checkFlowForProgram(NULL, lexnode, machine);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }
    flowctl->retVarSwitchEnv(subenv);
    delete subenv;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::processArgsInSubEnv(finSyntaxNode *argnode, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( argnode == NULL )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finLexNode *lexnode = argnode->getCommandLexNode();
    if ( argnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finErrorCode errcode;
    bool subgoon = true;
    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
        for ( int i = 0; i < argnode->getSubListCount(); i++ ) {
            flowctl->resetFlowControl();

            errcode = this->appendArgToSubenv(i, argnode->getSubSyntaxNode(i), env, machine, flowctl);
            if ( finErrorCodeKits::isErrorResult(errcode) )
                return errcode;

            errcode = flowctl->checkFlowForExpress(&subgoon, lexnode, machine);
            if ( finErrorCodeKits::isErrorResult(errcode) || !subgoon )
                return errcode;
        }
    } else {
        errcode = this->appendArgToSubenv(0, argnode, env, machine, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForExpress(&subgoon, lexnode, machine);
        if ( finErrorCodeKits::isErrorResult(errcode) || !subgoon )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::appendArgToSubenv(int idx, finSyntaxNode *argnode, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();
    finExecVariable *argvar;

    errcode = machine->instantExecute(argnode, env->getParentEnvironment(), flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool ingoon = true;
    errcode = flowctl->checkFlowForExpress(&ingoon, lexnode, machine);
    if ( finErrorCodeKits::isErrorResult(errcode) || !ingoon)
        return errcode;

    flowctl->buildLinkedLeftVar();
    argvar = flowctl->getReturnVariable();
    argvar->setName(this->getParameterName(idx));

    errcode = env->addVariable(argvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        machine->appendExecutionError(lexnode, QString("Internal error."));
        flowctl->pickReturnVariable();
        delete argvar;
        return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::execUserFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    return machine->instantExecute(this->_u._funcNode, env, flowctl);
}

finErrorCode
finExecFunction::execSysFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    return this->_u._funcCall(this, env, machine, flowctl);
}

QString finExecFunction::getExtArgPrefix()
{
    return finExecFunction::_extArgPrefix;
}

QList<finExecSysFuncRegItem> finExecFunction::_sysFuncList =
        QList<finExecSysFuncRegItem>();

finErrorCode
finExecFunction::registSysFuncFromArray(finExecSysFuncRegItem *sysfuncist)
{
    for ( int i = 0; !sysfuncist[i]._funcName.isNull(); i++ ) {
        finExecFunction::_sysFuncList.append(sysfuncist[i]);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::registSysFuncAll()
{
    finErrorCode errcode;

    errcode = finExecFunction::registSysFuncMath();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncMatrix();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncString();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncFile();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncSystem();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncFiguring();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncFiguring();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecFunction::installSystemFunctions (finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int i, succcnt = 0;

    if ( rootenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    errcode = finExecFunction::registSysFuncAll();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    for ( i = 0; i < finExecFunction::_sysFuncList.count(); i++ ) {
        const finExecSysFuncRegItem &sysfunc = finExecFunction::_sysFuncList.at(i);

        QStringList paramlist;
        finExecFunction *curfunc = new finExecFunction();
        if ( curfunc == NULL )
            goto item_bad;

        errcode = curfunc->setFunctionType(finExecFunction::FIN_FN_TYPE_SYSTEM);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = curfunc->setFunctionName(sysfunc._funcName);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        paramlist = sysfunc._paramCsvList.split(',');
        curfunc->clearParameterNames();
        for ( int j = 0; j < paramlist.count(); j++ ) {
            errcode = curfunc->appendParameterName(paramlist.at(j));
            if ( finErrorCodeKits::isErrorResult(errcode) )
                goto item_bad;
        }

        errcode = curfunc->setFunctionCall(sysfunc._funcCall);
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

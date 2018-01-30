/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
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
    this->_type = TP_DUMMY;
    this->_u._rawPointer = NULL;
}

finExecFunctionType finExecFunction::getFunctionType() const
{
    return this->_type;
}

const QString &finExecFunction::getFunctionName() const
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
        return finExecFunction::getExtArgName(idx - this->_paramList.count());
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
        return finErrorKits::EC_DUPLICATE_OP;

    this->_type = type;
    this->_u._rawPointer = NULL;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionName(const QString &funcname)
{
    if ( QString::compare(this->_funcName, funcname) == 0 )
        return finErrorKits::EC_DUPLICATE_OP;

    this->_funcName = funcname;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::appendParameterName(const QString &paramname)
{
    if ( paramname.isEmpty() || paramname.isNull() )
        return finErrorKits::EC_INVALID_PARAM;

    this->_paramList.append(paramname);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::clearParameterNames()
{
    if ( this->_paramList.count() == 0 )
        return finErrorKits::EC_DUPLICATE_OP;

    this->_paramList.clear();
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionSyntaxNode(finSyntaxNode *funcnode)
{
    if ( this->_type != finExecFunction::TP_USER )
        return finErrorKits::EC_STATE_ERROR;

    this->_u._funcNode = funcnode;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionCall(finFunctionCall funccall)
{
    if ( this->_type != finExecFunction::TP_SYSTEM )
        return finErrorKits::EC_STATE_ERROR;

    this->_u._funcCall = funccall;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::execFunction(finSyntaxNode *argnode, finExecEnvironment *env, finExecMachine *machine,
                              finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();

    if ( argnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorKits::EC_READ_ERROR;
    }

    if ( lexnode->getType() != finLexNode::TP_OPERATOR ||
         lexnode->getOperator() != finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorKits::EC_READ_ERROR;
    }

    finExecEnvironment *subenv;
    errcode = env->buildChildEnvironment(&subenv);
    if ( finErrorKits::isErrorResult(errcode) ) {
        machine->appendExecutionError(lexnode, QString("Internal error."));
        return errcode;
    }
    subenv->setEnvironmentName(this->_funcName);
    subenv->setBelongFunction(this);

    if ( argnode->getSubListCount() > 0 ) {
        errcode = this->processArgsInSubEnv(argnode->getSubSyntaxNode(0), subenv, machine, flowctl);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        }

        bool arggoon = true;
        errcode = flowctl->checkFlowForExpress(&arggoon, lexnode, machine);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        } else if ( !arggoon ) {
            flowctl->retVarSwitchEnv(subenv);
            delete subenv;
            return errcode;
        }
    }

    flowctl->resetFlowControl();
    if ( this->_type == finExecFunction::TP_SYSTEM ) {
        errcode = this->execSysFunction(subenv, machine, flowctl);
    } else if ( this->_type == finExecFunction::TP_USER ) {
        errcode = this->execUserFunction(subenv, machine, flowctl);
    } else {
        machine->appendExecutionError(lexnode, QString("ERROR: Function type cannot be recognized."));
        delete subenv;
        return finErrorKits::EC_READ_ERROR;
    }
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }

    errcode = flowctl->checkFlowForProgram(NULL, lexnode, machine);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }
    flowctl->retVarSwitchEnv(subenv);
    delete subenv;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::processArgsInSubEnv(finSyntaxNode *argnode, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( argnode == NULL )
        return finErrorKits::EC_SUCCESS;

    finLexNode *lexnode = argnode->getCommandLexNode();
    if ( argnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorKits::EC_READ_ERROR;
    }

    finErrorCode errcode;
    bool subgoon = true;
    if ( lexnode->getType() == finLexNode::TP_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
        for ( int i = 0; i < argnode->getSubListCount(); i++ ) {
            flowctl->resetFlowControl();

            errcode = this->appendArgToSubenv(i, argnode->getSubSyntaxNode(i), env, machine, flowctl);
            if ( finErrorKits::isErrorResult(errcode) )
                return errcode;

            errcode = flowctl->checkFlowForExpress(&subgoon, lexnode, machine);
            if ( finErrorKits::isErrorResult(errcode) || !subgoon )
                return errcode;
        }
    } else {
        errcode = this->appendArgToSubenv(0, argnode, env, machine, flowctl);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForExpress(&subgoon, lexnode, machine);
        if ( finErrorKits::isErrorResult(errcode) || !subgoon )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::appendArgToSubenv(int idx, finSyntaxNode *argnode, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();
    finExecVariable *argvar;

    errcode = machine->instantExecute(argnode, env->getParentEnvironment(), flowctl);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    bool ingoon = true;
    errcode = flowctl->checkFlowForExpress(&ingoon, lexnode, machine);
    if ( finErrorKits::isErrorResult(errcode) || !ingoon)
        return errcode;

    flowctl->buildLinkedLeftVar();
    argvar = flowctl->pickReturnVariable();
    if ( argvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;
    argvar->setName(this->getParameterName(idx));

    errcode = env->addVariable(argvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        machine->appendExecutionError(lexnode, QString("Internal error."));
        flowctl->pickReturnVariable();
        delete argvar;
        return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::execFunction(QList<finExecVariable *> *arglist, finExecEnvironment *env,
                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecEnvironment *subenv;

    errcode = env->buildChildEnvironment(&subenv);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;
    subenv->setEnvironmentName(this->_funcName);
    subenv->setBelongFunction(this);

    if ( arglist != NULL ) {
        errcode = processArgsInSubEnv(arglist, subenv);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete subenv;
            return errcode;
        }
    }

    if ( this->_type == finExecFunction::TP_SYSTEM ) {
        errcode = this->execSysFunction(subenv, machine, flowctl);
    } else if ( this->_type == finExecFunction::TP_USER ) {
        errcode = this->execUserFunction(subenv, machine, flowctl);
    } else {
        delete subenv;
        return finErrorKits::EC_READ_ERROR;
    }
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }

    errcode = flowctl->checkFlowForProgram(NULL, NULL, machine);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete subenv;
        return errcode;
    }
    flowctl->retVarSwitchEnv(subenv);
    delete subenv;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::processArgsInSubEnv(QList<finExecVariable *> *arglist, finExecEnvironment *env)
{
    finErrorCode errcode;

    for ( int i = 0; i < arglist->count(); i++ ) {
        finExecVariable *argvar = new finExecVariable();
        if ( argvar == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        errcode = argvar->setLinkTarget(arglist->at(i));
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete argvar;
            return errcode;
        }

        argvar->setName(this->getParameterName(i));
        argvar->setLeftValue();

        errcode = env->addVariable(argvar);
        if ( finErrorKits::isErrorResult(errcode) ) {
            arglist->removeAt(i);
            delete argvar;
            return errcode;
        }
    }
    return finErrorKits::EC_SUCCESS;
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

QString finExecFunction::getExtArgName(int idx)
{
    QString extargname = finExecFunction::_extArgPrefix;
    extargname.append(QString::number(idx));
    return extargname;
}

int finExecFunction::getExtendArgCountHere(finExecEnvironment *env)
{
    finExecVariable *extargvar = NULL;
    int idx = 0;

    do {
        extargvar = finExecFunction::getExtendArgAt(env, idx);
        if ( extargvar == NULL )
            break;
        idx++;
    } while ( true );
    return idx;
}

finExecVariable *finExecFunction::getExtendArgHereAt(finExecEnvironment *env, int idx)
{
    QString extargname = finExecFunction::getExtArgName(idx);
    finExecVariable *extargvar = env->getVariableHere(extargname);
    return extargvar;
}

int finExecFunction::getExtendArgCount(finExecEnvironment *env)
{
    int envlevel = env->getBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return -1;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getExtendArgAt(finExecEnvironment *env, int idx)
{
    int envlevel = env->getBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return NULL;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return NULL;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

QList<finExecVariable *> finExecFunction::getExtendArgList(finExecEnvironment *env)
{
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
    return arglist;
}

int finExecFunction::getPreviousExtendArgCount(finExecEnvironment *env)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return -1;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getPreviousExtendArgAt(finExecEnvironment *env, int idx)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return NULL;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return NULL;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

int finExecFunction::getPreviousExtendArgCount(finExecEnvironment *env, int level)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx(level);
    if ( envlevel < 0 )
        return -1;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getPreviousExtendArgAt(finExecEnvironment *env, int idx, int level)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx(level);
    if ( envlevel < 0 )
        return NULL;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == NULL )
        return NULL;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

QList<finExecSysFuncRegItem> finExecFunction::_sysFuncList =
        QList<finExecSysFuncRegItem>();

finErrorCode
finExecFunction::registSysFuncFromArray(finExecSysFuncRegItem *sysfuncist)
{
    for ( int i = 0; !sysfuncist[i]._funcName.isNull(); i++ ) {
        finExecFunction::_sysFuncList.append(sysfuncist[i]);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFunction::registSysFuncAll()
{
    finErrorCode errcode;

    errcode = finExecFunction::registSysFuncMath();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncMatrix();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncString();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncFile();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncSystem();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncFiguring();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    errcode = finExecFunction::registSysFuncPlot();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode
finExecFunction::installSystemFunctions (finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int i, succcnt = 0;

    if ( rootenv == NULL )
        return finErrorKits::EC_NULL_POINTER;

    errcode = finExecFunction::registSysFuncAll();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    for ( i = 0; i < finExecFunction::_sysFuncList.count(); i++ ) {
        const finExecSysFuncRegItem &sysfunc = finExecFunction::_sysFuncList.at(i);

        QStringList paramlist;
        finExecFunction *curfunc = new finExecFunction();
        if ( curfunc == NULL )
            goto item_bad;

        errcode = curfunc->setFunctionType(finExecFunction::TP_SYSTEM);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = curfunc->setFunctionName(sysfunc._funcName);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        if ( sysfunc._paramCsvList.length() > 0 ) {
            paramlist = sysfunc._paramCsvList.split(',');
            curfunc->clearParameterNames();
            for ( int j = 0; j < paramlist.count(); j++ ) {
                errcode = curfunc->appendParameterName(paramlist.at(j));
                if ( finErrorKits::isErrorResult(errcode) )
                    goto item_bad;
            }
        }

        errcode = curfunc->setFunctionCall(sysfunc._funcCall);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = rootenv->addFunction(curfunc);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        succcnt++;
        continue;

item_bad:
        if ( curfunc != NULL )
            delete curfunc;
    }

    if ( succcnt == i ) {
        if ( succcnt > 0 )
            return finErrorKits::EC_SUCCESS;
        else
            return finErrorKits::EC_REACH_BOTTOM;
    } else {
        if ( succcnt > 0 )
            return finErrorKits::EC_NORMAL_WARN;
        else
            return finErrorKits::EC_NOT_FOUND;
    }
}

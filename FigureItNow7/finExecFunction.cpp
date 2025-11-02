/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <memory>

#include "finLexNode.h"
#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"


QString finExecFunction::_extArgPrefix("__ext_arg_");

finExecFunction::finExecFunction()
{
    this->_type = TP_DUMMY;
    this->_u._rawPointer = nullptr;
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

void finExecFunction::setFunctionType(finExecFunctionType type)
{
    if ( this->_type == type )
        return;

    this->_type = type;
    this->_u._rawPointer = nullptr;
}

void finExecFunction::setFunctionName(const QString &funcname)
{
    if ( QString::compare(this->_funcName, funcname) == 0 )
        return;

    this->_funcName = funcname;
}

void finExecFunction::appendParameterName(const QString &paramname)
{
    if ( paramname.isEmpty() || paramname.isNull() )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Parameter name is empty.");

    this->_paramList.append(paramname);
}

void finExecFunction::clearParameterNames()
{
    if ( this->_paramList.count() == 0 )
        return;

    this->_paramList.clear();
}

void finExecFunction::setFunctionSyntaxNode(finSyntaxNode *funcnode)
{
    if ( this->_type != finExecFunction::TP_USER )
        finThrow(finErrorKits::EC_STATE_ERROR, "Function is not user-defined.");

    this->_u._funcNode = funcnode;
}

void finExecFunction::setFunctionCall(finFunctionCall funccall)
{
    if ( this->_type != finExecFunction::TP_SYSTEM )
        finThrow(finErrorKits::EC_STATE_ERROR, "Function is not system-defined.");

    this->_u._funcCall = funccall;
}

finErrorCode
finExecFunction::execFunction(finSyntaxNode *argnode, finExecEnvironment *env, finExecMachine *machine,
                              finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = argnode->getCommandLexNode();

    if ( argnode->getType() != finSyntaxNode::TP_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorKits::EC_READ_ERROR;
    }

    if ( lexnode->getType() != finLexNode::TP_OPERATOR ||
         lexnode->getOperator() != finLexNode::OP_L_RND_BRCKT ) {
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

    errcode = flowctl->checkFlowForProgram(nullptr, lexnode, machine);
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
    if ( argnode == nullptr )
        return finErrorKits::EC_SUCCESS;

    finLexNode *lexnode = argnode->getCommandLexNode();
    if ( argnode->getType() != finSyntaxNode::TP_EXPRESS ) {
        machine->appendExecutionError(lexnode, QString("Unrecognized function arguments."));
        return finErrorKits::EC_READ_ERROR;
    }

    finErrorCode errcode;
    bool subgoon = true;
    if ( lexnode->getType() == finLexNode::TP_OPERATOR &&
         lexnode->getOperator() == finLexNode::OP_COMMA ) {
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
    if ( argvar == nullptr )
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

    if ( arglist != nullptr ) {
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

    errcode = flowctl->checkFlowForProgram(nullptr, nullptr, machine);
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
        if ( argvar == nullptr )
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
    finExecVariable *extargvar = nullptr;
    int idx = 0;

    do {
        extargvar = finExecFunction::getExtendArgAt(env, idx);
        if ( extargvar == nullptr )
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
    if ( funcenv == nullptr )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getExtendArgAt(finExecEnvironment *env, int idx)
{
    int envlevel = env->getBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return nullptr;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == nullptr )
        return nullptr;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

QList<finExecVariable *> finExecFunction::getExtendArgList(finExecEnvironment *env)
{
    QList<finExecVariable *> arglist;
    finExecVariable *argvar;
    int idx = 0;
    while ( true ) {
        argvar = finExecFunction::getExtendArgAt(env, idx);
        if ( argvar == nullptr )
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
    if ( funcenv == nullptr )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getPreviousExtendArgAt(finExecEnvironment *env, int idx)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx();
    if ( envlevel < 0 )
        return nullptr;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == nullptr )
        return nullptr;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

int finExecFunction::getPreviousExtendArgCount(finExecEnvironment *env, int level)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx(level);
    if ( envlevel < 0 )
        return -1;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == nullptr )
        return -1;

    return finExecFunction::getExtendArgCountHere(funcenv);
}

finExecVariable *finExecFunction::getPreviousExtendArgAt(finExecEnvironment *env, int idx, int level)
{
    int envlevel = env->getPreviousBelongFunctionEnvLevelIdx(level);
    if ( envlevel < 0 )
        return nullptr;

    finExecEnvironment *funcenv = env->getParentEnvironment(envlevel);
    if ( funcenv == nullptr )
        return nullptr;

    return finExecFunction::getExtendArgHereAt(funcenv, idx);
}

QList<finExecSysFuncRegItem> finExecFunction::_sysFuncList =
        QList<finExecSysFuncRegItem>();

finErrorCode
finExecFunction::registSysFuncFromArray(finExecSysFuncRegItem *sysfunclist, const QString &category)
{
    for ( int i = 0; !sysfunclist[i]._funcName.isNull(); i++ ) {
        if ( sysfunclist[i]._category.isEmpty() )
            sysfunclist[i]._category = category;
        if ( sysfunclist[i]._prototype.isEmpty() )
            sysfunclist[i]._prototype = sysfunclist[i]._funcName + " (" + sysfunclist[i]._paramCsvList + ")";

        finExecFunction::_sysFuncList.append(sysfunclist[i]);
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

    if ( rootenv == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    errcode = finExecFunction::registSysFuncAll();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    for ( i = 0; i < finExecFunction::_sysFuncList.count(); i++ ) {
        const finExecSysFuncRegItem &sysfunc = finExecFunction::_sysFuncList.at(i);

        QStringList paramlist;
        std::unique_ptr<finExecFunction> curfunc(new finExecFunction());
        if ( curfunc.get() == nullptr )
            continue;

        curfunc->setFunctionType(finExecFunction::TP_SYSTEM);
        curfunc->setFunctionName(sysfunc._funcName);

        if ( sysfunc._paramCsvList.length() > 0 ) {
            paramlist = sysfunc._paramCsvList.split(',');
            curfunc->clearParameterNames();
            for ( int j = 0; j < paramlist.count(); j++ ) {
                curfunc->appendParameterName(paramlist.at(j));
            }
        }

        curfunc->setFunctionCall(sysfunc._funcCall);

        errcode = rootenv->addFunction(curfunc.get());
        if ( finErrorKits::isErrorResult(errcode) )
            continue;

        curfunc.release();
        succcnt++;
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

const QList<finExecSysFuncRegItem> &finExecFunction::getSysFuncRegList()
{
    return finExecFunction::_sysFuncList;
}

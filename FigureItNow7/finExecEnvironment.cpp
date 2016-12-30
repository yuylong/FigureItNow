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
 * 20161229    0  Yulong Yu    Create this file.
 */

#include "finExecEnvironment.h"

#include "finExecFunction.h"

finExecEnvironment::finExecEnvironment()
{
    this->_envName = QString("");
    this->_varList.clear();
    this->_funcList.clear();
    this->_figContainer = NULL;
    this->_prevEnv = NULL;
}

finExecEnvironment::~finExecEnvironment()
{
    while ( !this->_varList.empty() ) {
        finExecVariable *evar = this->_varList.first();
        this->_varList.removeFirst();
        delete evar;
    }
    while ( !this->_funcList.empty() ) {
        finExecFunction *efunc = this->_funcList.first();
        this->_funcList.removeFirst();
        delete efunc;
    }
}

finErrorCode
finExecEnvironment::buildChildEnvironment(finExecEnvironment **chdenv)
{
    if ( chdenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecEnvironment *newchdenv = new finExecEnvironment();
    if ( newchdenv == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    newchdenv->_figContainer = this->_figContainer;
    newchdenv->_prevEnv = this;

    *chdenv = newchdenv;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QString
finExecEnvironment::getEnvironmentName() const
{
    return this->_envName;
}

finErrorCode
finExecEnvironment::setEnvironmentName(const QString &envname)
{
    this->_envName = envname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecVariable *
finExecEnvironment::getVariableHere(const QString &varname)
{
    for ( int i = 0; i < this->_varList.count(); i++ ) {
        finExecVariable *var = this->_varList.at(i);

        if ( QString::compare(var->getName(), varname) == 0 )
            return var;
    }
    return NULL;
}

finExecVariable *
finExecEnvironment::findVariable(const QString &varname)
{
    finExecVariable *retvar;

    retvar = this->getVariableHere(varname);
    if ( retvar != NULL )
        return retvar;

    if ( this->_prevEnv != NULL )
        return this->_prevEnv->findVariable(varname);
    else
        return NULL;
}

finExecFunction *
finExecEnvironment::getFunctionHere(const QString &funcname)
{
    for ( int i = 0; i < this->_funcList.count(); i++ ) {
        finExecFunction *func = this->_funcList.at(i);

        if ( QString::compare(func->getFunctionName(), funcname) == 0 )
            return func;
    }
    return NULL;
}

finExecFunction *
finExecEnvironment::findFunction(const QString &funcname)
{
    finExecFunction *retfunc;

    retfunc = this->getFunctionHere(funcname);
    if ( retfunc != NULL )
        return retfunc;

    if ( this->_prevEnv != NULL )
        return this->_prevEnv->findFunction(funcname);
    else
        return NULL;
}

finErrorCode
finExecEnvironment::addVariable(finExecVariable *var)
{
    finExecVariable *oldvar = this->getVariableHere(var->getName());

    if ( oldvar != NULL )
        return finErrorCodeKits::FIN_EC_CONTENTION;

    this->_varList.append(var);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecEnvironment::addFunction(finExecFunction *func)
{
    finExecFunction *oldfunc = this->getFunctionHere(func->getFunctionName());

    if ( oldfunc != NULL )
        return finErrorCodeKits::FIN_EC_CONTENTION;

    this->_funcList.append(func);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finFigureContainer *
finExecEnvironment::getFigureContainer()
{
    return this->_figContainer;
}

finExecEnvironment *
finExecEnvironment::getParentEnvironment()
{
    return this->_prevEnv;
}

finErrorCode
finExecEnvironment::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_figContainer = figcontainer;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecEnvironment::setParentEnvironment(finExecEnvironment *prevenv)
{
    this->_prevEnv = prevenv;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecEnvironment *finExecEnvironment::_rootEnv = NULL;

finErrorCode
finExecEnvironment::setupRootEnvironment()
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecEnvironment::disposeRootEnvironment()
{
    if ( _rootEnv == NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    delete _rootEnv;
    _rootEnv = NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecEnvironment *
finExecEnvironment::getRootEnvironment()
{
    return _rootEnv;
}

finErrorCode
finExecEnvironment::buildRootChildEnvironment(finExecEnvironment **chdenv)
{
    return _rootEnv->buildChildEnvironment(chdenv);
}

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

#include "finExecVariable.h"
#include "finExecFunction.h"
#include "finExecMachine.h"
#include "finExecOperartorClac.h"


finExecEnvironment::finExecEnvironment()
{
    this->_envName = QString("");
    this->_varList.clear();
    this->_funcList.clear();
    this->_belongFunc = NULL;
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

    newchdenv->_belongFunc = NULL;
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
    if ( varname.isEmpty() )
        return NULL;

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
    finExecVariable *retvar = this->getVariableHere(varname);
    if ( retvar != NULL )
        return retvar;

    if ( this->_prevEnv != NULL )
        return this->_prevEnv->findVariable(varname);
    else
        return NULL;
}

finExecVariable *
finExecEnvironment::findVariableUntil(const QString &varname, const QString &envname)
{
    finExecVariable *retvar = this->getVariableHere(varname);
    if ( retvar != NULL )
        return retvar;

    if ( QString::compare(this->_envName, envname) != 0 && this->_prevEnv != NULL )
        return this->_prevEnv->findVariableUntil(varname, envname);
    else
        return NULL;
}

finExecVariable *
finExecEnvironment::findVariableUntil(const QString &varname, finExecFunction *blngfunc)
{
    finExecVariable *retvar = this->getVariableHere(varname);
    if ( retvar != NULL )
        return retvar;

    if ( blngfunc != this->_belongFunc && this->_prevEnv != NULL )
        return this->_prevEnv->findVariableUntil(varname, blngfunc);
    else
        return NULL;
}

finExecVariable *
finExecEnvironment::findVariableUntil(const QString &varname, int envlevel)
{
    if ( envlevel < 0 )
        return NULL;

    finExecVariable *retvar = this->getVariableHere(varname);
    if ( retvar != NULL )
        return retvar;

    if ( envlevel > 0 && this->_prevEnv != NULL )
        return this->_prevEnv->findVariableUntil(varname, envlevel - 1);
    else
        return NULL;
}

finExecFunction *
finExecEnvironment::getFunctionHere(const QString &funcname)
{
    if ( funcname.isEmpty() )
        return NULL;

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
    finExecFunction *retfunc = this->getFunctionHere(funcname);
    if ( retfunc != NULL )
        return retfunc;

    if ( this->_prevEnv != NULL )
        return this->_prevEnv->findFunction(funcname);
    else
        return NULL;
}

finExecFunction *
finExecEnvironment::findFunctionUntil(const QString &funcname, const QString &envname)
{
    finExecFunction *retfunc = this->getFunctionHere(funcname);
    if ( retfunc != NULL )
        return retfunc;

    if ( QString::compare(this->_envName, envname) != 0 && this->_prevEnv != NULL )
        return this->_prevEnv->findFunctionUntil(funcname, envname);
    else
        return NULL;
}

finExecFunction *
finExecEnvironment::findFunctionUntil(const QString &funcname, finExecFunction *blngfunc)
{
    finExecFunction *retfunc = this->getFunctionHere(funcname);
    if ( retfunc != NULL )
        return retfunc;

    if ( blngfunc != this->_belongFunc && this->_prevEnv != NULL )
        return this->_prevEnv->findFunctionUntil(funcname, blngfunc);
    else
        return NULL;
}

finExecFunction *finExecEnvironment::findFunctionUntil(const QString &funcname, int envlevel)
{
    if ( envlevel < 0 )
        return NULL;

    finExecFunction *retfunc = this->getFunctionHere(funcname);
    if ( retfunc != NULL )
        return retfunc;

    if ( envlevel > 0 && this->_prevEnv != NULL )
        return this->_prevEnv->findFunctionUntil(funcname, envlevel - 1);
    else
        return NULL;
}

bool finExecEnvironment::isVariableInEnv(finExecVariable *var)
{
    for ( int i = 0; i < this->_varList.count(); i++ ) {
        if ( this->_varList.at(i)->isVariableInside(var) )
            return true;
    }
    return false;
}

bool finExecEnvironment::isVariableDirectInEnv(finExecVariable *var)
{
    for ( int i = 0; i < this->_varList.count(); i++ ) {
        if ( this->_varList.at(i) == var )
            return true;
    }
    return false;
}

bool finExecEnvironment::isFunctionInEnv(finExecFunction *func)
{
    for ( int i = 0; i < this->_funcList.count(); i++ ) {
        if ( this->_funcList.at(i) == func )
            return true;
    }
    return false;
}

finErrorCode
finExecEnvironment::addVariable(finExecVariable *var)
{
    if ( !var->isLeftValue() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecVariable *oldvar = NULL;
    if ( !var->getName().isEmpty() )
        this->getVariableHere(var->getName());

    if ( oldvar != NULL )
        return finErrorCodeKits::FIN_EC_CONTENTION;

    this->_varList.append(var);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecEnvironment::addFunction(finExecFunction *func)
{
    finExecFunction *oldfunc = NULL;
    if ( !func->getFunctionName().isEmpty() )
        this->getFunctionHere(func->getFunctionName());

    if ( oldfunc != NULL )
        return finErrorCodeKits::FIN_EC_CONTENTION;

    this->_funcList.append(func);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecEnvironment::removeVariable(finExecVariable *var)
{
    for ( int i = this->_varList.count() - 1; i >= 0; i-- ) {
        if ( this->_varList.at(i) == var )
            this->_varList.removeAt(i);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecEnvironment::removeFunction(finExecFunction *func)
{
    for ( int i = this->_funcList.count() - 1; i >= 0; i-- ) {
        if ( this->_funcList.at(i) == func )
            this->_funcList.removeAt(i);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecFunction *finExecEnvironment::getBelongFunctionHere() const
{
    return this->_belongFunc;
}

finExecFunction *finExecEnvironment::getBelongFunction() const
{
    if ( this->_belongFunc != NULL )
        return this->_belongFunc;

    if ( this->_prevEnv != NULL )
        return this->_prevEnv->getBelongFunction();
    else
        return NULL;
}

finExecFunction *finExecEnvironment::getPreviousBelongFunction() const
{
    return this->getPreviousBelongFunction(1);
}

finExecFunction *finExecEnvironment::getPreviousBelongFunction(int level) const
{
    if ( level == 0 && this->_belongFunc != NULL )
        return this->_belongFunc;

    if ( this->_prevEnv == NULL )
        return NULL;

    if ( this->_belongFunc != NULL )
        return this->_prevEnv->getPreviousBelongFunction(level - 1);
    else
        return this->_prevEnv->getPreviousBelongFunction(level);
}

int finExecEnvironment::getBelongFuncEvnLevelIn(int curlevel) const
{
    if ( this->_belongFunc != NULL )
        return curlevel;

    if ( this->_prevEnv == NULL )
        return -1;

    return this->_prevEnv->getBelongFuncEvnLevelIn(curlevel + 1);
}

int finExecEnvironment::getBelongFunctionEnvLevelIdx() const
{
    return this->getBelongFuncEvnLevelIn(0);
}

int finExecEnvironment::getPrevBelongFuncEnvLevelIn(int level, int curlevel) const
{
    if ( level == 0 && this->_belongFunc != NULL )
        return curlevel;

    if ( this->_prevEnv == NULL )
        return -1;

    if ( this->_belongFunc != NULL )
        return this->_prevEnv->getPrevBelongFuncEnvLevelIn(level - 1, curlevel + 1);
    else
        return this->_prevEnv->getPrevBelongFuncEnvLevelIn(level, curlevel + 1);
}

int finExecEnvironment::getPreviousBelongFunctionEnvLevelIdx() const
{
    return this->getPrevBelongFuncEnvLevelIn(1, 0);
}

int finExecEnvironment::getPreviousBelongFunctionEnvLevelIdx(int level) const
{
    return this->getPrevBelongFuncEnvLevelIn(level, 0);
}

int finExecEnvironment::getTotalFucnLevelCountIn(int curlevel) const
{
    if ( this->_prevEnv == NULL ) {
        if ( this->_belongFunc != NULL )
            return curlevel + 1;
        else
            return curlevel;
    } else {
        if ( this->_belongFunc != NULL )
            return this->_prevEnv->getTotalEnvLevelCountIn(curlevel + 1);
        else
            return this->_prevEnv->getTotalEnvLevelCountIn(curlevel);
    }
}

int finExecEnvironment::getTotalBelongFunctionLevelCount() const
{
    return this->getTotalFucnLevelCountIn(0);
}

finErrorCode finExecEnvironment::getBelongFunctionList(QStringList *funcnamelist) const
{
    if ( this->_belongFunc != NULL )
        funcnamelist->append(this->_belongFunc->getFunctionName());

    if ( this->_prevEnv == NULL )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    return this->_prevEnv->getBelongFunctionList(funcnamelist);
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

finExecEnvironment *
finExecEnvironment::getParentEnvironment(int envlevel)
{
    if ( envlevel == 0 )
        return this;
    else if ( envlevel < 0 )
        return NULL;

    if ( this->_prevEnv == NULL )
        return NULL;
    else
        return this->_prevEnv->getParentEnvironment(envlevel - 1);
}

finErrorCode finExecEnvironment::setBelongFunction(finExecFunction *func)
{
    this->_belongFunc = func;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

int finExecEnvironment::getTotalEnvLevelCountIn(int curlevel)
{
    if ( this->_prevEnv == NULL )
        return curlevel + 1;
    else
        return this->_prevEnv->getTotalEnvLevelCountIn(curlevel + 1);
}

int finExecEnvironment::getTotalEnvironmentLevelCount()
{
    return getTotalEnvLevelCountIn(0);
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
    if ( _rootEnv != NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    _rootEnv = new finExecEnvironment();
    if ( _rootEnv == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    _rootEnv->setEnvironmentName(QString("root"));
    finExecVariable::installSystemVariables(_rootEnv);
    finExecFunction::installSystemFunctions(_rootEnv);
    _rootEnv->setFigureContainer(NULL);
    _rootEnv->setParentEnvironment(NULL);

    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    if ( _rootEnv == NULL )
        finExecEnvironment::setupRootEnvironment();

    return _rootEnv;
}

finErrorCode
finExecEnvironment::buildRootChildEnvironment(finExecEnvironment **chdenv)
{
    return _rootEnv->buildChildEnvironment(chdenv);
}

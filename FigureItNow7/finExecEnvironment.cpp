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
    finExecEnvironment *newchdenv = new finExecEnvironment();

    if ( newchdenv == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
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
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecEnvironment::addFunction(finExecFunction *func)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
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
finExecEnvironment::getSyntaxNodeValue(finSyntaxNode *synnode, finExecVariable *retval)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

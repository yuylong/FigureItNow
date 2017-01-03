/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 30th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161230    0  Yulong Yu    Create this file.
 */

#include "finExecMachine.h"

finExecMachine::finExecMachine()
{
    this->_name = QString();
    this->_baseEnv = NULL;
    this->_baseFigContainer = NULL;
    this->_scriptCode = QString();
    this->_isCompiled = false;
}

finExecMachine::finExecMachine(const QString &name)
{
    this->_name = name;
    this->_baseEnv = NULL;
    this->_baseFigContainer = NULL;
    this->_scriptCode = QString();
    this->_isCompiled = false;
}

finExecMachine::~finExecMachine()
{
    if ( this->_baseEnv != NULL )
        delete this->_baseEnv;
}

QString finExecMachine::getName() const
{
    return this->_name;
}

finExecEnvironment *finExecMachine::getBaseEnvironment()
{
    return this->_baseEnv;
}

finFigureContainer *finExecMachine::getFigureContainer()
{
    return this->_baseFigContainer;
}

QString finExecMachine::getScriptCode() const
{
    return this->_scriptCode;
}

QString finExecMachine::getCompiledScriptCode() const
{
    return this->_syntaxRdr.getScriptCode();
}

finErrorCode finExecMachine::setName(const QString &name)
{
    this->_name = name;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::initEnvironment(finExecEnvironment *prevenv)
{
    finErrorCode errcode;

    if ( prevenv == NULL )
        return this->initEnvironmentFromRoot();

    if ( this->_baseEnv != NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    errcode = prevenv->buildChildEnvironment(&this->_baseEnv);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->_baseEnv = NULL;
        return errcode;
    }

    this->_baseEnv->setFigureContainer(this->_baseFigContainer);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::initEnvironmentFromRoot()
{
    finErrorCode errcode;

    if ( finExecEnvironment::getRootEnvironment() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_baseEnv != NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    errcode = finExecEnvironment::buildRootChildEnvironment(&this->_baseEnv);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->_baseEnv = NULL;
        return errcode;
    }

    this->_baseEnv->setFigureContainer(this->_baseFigContainer);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_baseFigContainer = figcontainer;
    if ( this->_baseEnv != NULL )
        this->_baseEnv->setFigureContainer(figcontainer);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::setScriptCode(const QString &script)
{
    this->_scriptCode = script;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finExecMachine::isCompiled() const
{
    return this->_isCompiled;
}

finErrorCode finExecMachine::compile()
{
    finErrorCode errcode;

    if ( this->_isCompiled )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_syntaxRdr.stopRead();
    errcode = this->_syntaxRdr.setScriptCode(this->_scriptCode);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->_syntaxRdr.startRead();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = finErrorCodeKits::FIN_EC_SUCCESS;
    while ( errcode != finErrorCodeKits::FIN_EC_REACH_BOTTOM ) {
        errcode = this->_syntaxRdr.readNextToken();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }

    this->_isCompiled = true;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::releaseCompile()
{
    if ( !this->_isCompiled )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_syntaxRdr.stopRead();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::execute()
{
    finErrorCode errcode;

    if ( this->_baseEnv == NULL || this->_baseFigContainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( !this->_isCompiled )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finSyntaxTree *syntree = this->_syntaxRdr.getSyntaxTree();
    if ( syntree == NULL )
        return finErrorCodeKits::FIN_EC_READ_ERROR;

    finExecVariable *retvar = NULL;
    errcode = finExecMachine::instantExecute(syntree->getRootNode(), this->_baseEnv, &retvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    if ( retvar != NULL )
        delete retvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;

}

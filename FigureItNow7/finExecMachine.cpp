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
    return this->_scriptCode = script;
}

finErrorCode finExecMachine::compile()
{
    if ( this->_isCompiled )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finExecMachine::releaseCompile()
{
    if ( !this->_isCompiled )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finExecMachine::execute()
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}


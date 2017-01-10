/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yusoft. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170104    0  Yulong Yu    Create this file.
 */

#include "finExecFlowControl.h"

#include "finExecMachine.h"

finExecFlowControl::finExecFlowControl()
{
    this->_type = finExecFlowControl::FIN_FC_NEXT;
    this->_label = QString();
    this->_retVar = NULL;
}

void finExecFlowControl::resetFlowControl()
{
    this->_type = finExecFlowControl::FIN_FC_NEXT;
    this->_label = QString();
    this->releaseReturnVariable();
}

finErrorCode finExecFlowControl::copyFlowControl(finExecFlowControl *srcfc)
{
    if ( this->_retVar != NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_type = srcfc->getType();
    this->_label = srcfc->getGotoLabel();
    this->_retVar = srcfc->getReturnVariable();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecFlowControlType finExecFlowControl::getType() const
{
    return this->_type;
}

bool finExecFlowControl::isFlowNext() const
{
    return (this->_type == finExecFlowControl::FIN_FC_NEXT);
}

bool finExecFlowControl::isFlowGoto() const
{
    return (this->_type == finExecFlowControl::FIN_FC_GOTO);
}

QString finExecFlowControl::getGotoLabel() const
{
    if ( this->_type == finExecFlowControl::FIN_FC_GOTO )
        return this->_label;
    else
        return QString();
}

finExecVariable *finExecFlowControl::getReturnVariable()
{
    return this->_retVar;
}

bool finExecFlowControl::isFlowExit() const
{
    return this->_type == finExecFlowControl::FIN_FC_EXIT;
}

bool finExecFlowControl::isFlowExpressOk() const
{
    return (this->_type == finExecFlowControl::FIN_FC_NEXT ||
            this->_type == finExecFlowControl::FIN_FC_EXIT);
}
bool finExecFlowControl::isFlowProgramOk() const
{
    return (this->_type == finExecFlowControl::FIN_FC_NEXT ||
            this->_type == finExecFlowControl::FIN_FC_RETURN ||
            this->_type == finExecFlowControl::FIN_FC_EXIT);
}

finErrorCode finExecFlowControl::checkFlowForExpress(bool *goon, finExecFlowControl *outflowctl,
                                                     finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::FIN_FC_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        return finErrorCodeKits::FIN_EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::FIN_FC_EXIT ) {
        if ( goon != NULL )
            *goon = false;
        if ( outflowctl != NULL )
            outflowctl->copyFlowControl(this);
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
}

finErrorCode finExecFlowControl::checkFlowForStatement(bool *goon, finExecFlowControl *outflowctl,
                                                       finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::FIN_FC_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        return finErrorCodeKits::FIN_EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::FIN_FC_EXIT ||
                this->_type == finExecFlowControl::FIN_FC_RETURN ||
                this->_type == finExecFlowControl::FIN_FC_GOTO ||
                this->_type == finExecFlowControl::FIN_FC_BREAK ||
                this->_type == finExecFlowControl::FIN_FC_CONTINUE ) {
        if ( goon != NULL )
            *goon = false;
        if ( outflowctl != NULL )
            outflowctl->copyFlowControl(this);
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
}

finErrorCode finExecFlowControl::checkFlowForProgram(bool *goon, finExecFlowControl *outflowctl,
                                                     finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::FIN_FC_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        return finErrorCodeKits::FIN_EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::FIN_FC_RETURN ) {
        if ( goon != NULL )
            *goon = false;
        if ( outflowctl != NULL ) {
            outflowctl->copyFlowControl(this);
            outflowctl->setFlowNext();
        }
        return finErrorCodeKits::FIN_EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::FIN_FC_EXIT ) {
        if ( goon != NULL )
            *goon = false;
        if ( outflowctl != NULL )
            outflowctl->copyFlowControl(this);
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
}

bool finExecFlowControl::checkFlowExpressGoOn(finLexNode *lexnode, finExecMachine *machine, finErrorCode *errcode)
{
    if ( !this->isFlowExpressOk() ) {
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));
        if ( errcode != NULL )
            *errcode = finErrorCodeKits::FIN_EC_READ_ERROR;
        return false;
    } else if ( this->isFlowExit() ) {
        this->directPass();
        if ( errcode != NULL )
            *errcode = finErrorCodeKits::FIN_EC_NORMAL_WARN;
        return false;
    }
    this->setFlowNext();
    return true;
}

bool finExecFlowControl::checkFlowProgramGoOn(finLexNode *lexnode, finExecMachine *machine, finErrorCode *errcode)
{
    if ( !this->isFlowProgramOk() ) {
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));
        if ( errcode != NULL )
            *errcode = finErrorCodeKits::FIN_EC_READ_ERROR;
        return false;
    } else if ( this->isFlowExit() ) {
        this->directPass();
        if ( errcode != NULL )
            *errcode = finErrorCodeKits::FIN_EC_NORMAL_WARN;
        return false;
    }
    this->setFlowNext();
    return true;
}

finErrorCode finExecFlowControl::setType(finExecFlowControlType type)
{
    this->_type = type;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setLabel(const QString &label)
{
    this->_label = label;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setFlowNext()
{
    this->_type = finExecFlowControl::FIN_FC_NEXT;
    this->_label = QString();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setGotoAndLabel(const QString &label)
{
    this->_type = finExecFlowControl::FIN_FC_GOTO;
    this->_label = label;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setReturnVariable(finExecVariable *retvar)
{
    if ( this->_retVar != NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_retVar = retvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::retVarSwitchEnv(finExecEnvironment *subenv)
{
    this->_retVar = finExecVariable::buildFuncReturnVariable(this->_retVar, subenv);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finExecFlowControl::releaseReturnVariable()
{
    finExecVariable::releaseNonLeftVariable(this->_retVar);
    this->_retVar = NULL;
}


finErrorCode finExecFlowControl::directPass()
{
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFlowControl.h"

#include "finExecMachine.h"

finExecFlowControl::finExecFlowControl()
{
    this->_type = finExecFlowControl::TP_NEXT;
    this->_label = QString();
    this->_retVar = NULL;
}

void finExecFlowControl::resetFlowControl()
{
    this->_type = finExecFlowControl::TP_NEXT;
    this->_label = QString();
    this->releaseReturnVariable();
}

finErrorCode finExecFlowControl::copyFlowControl(finExecFlowControl *srcfc)
{
    if ( this->_retVar != NULL )
        return finErrorKits::EC_STATE_ERROR;

    this->_type = srcfc->getType();
    this->_label = srcfc->getGotoLabel();
    this->_retVar = srcfc->getReturnVariable();
    return finErrorKits::EC_SUCCESS;
}

finExecFlowControlType finExecFlowControl::getType() const
{
    return this->_type;
}

bool finExecFlowControl::isFlowNext() const
{
    return (this->_type == finExecFlowControl::TP_NEXT);
}

bool finExecFlowControl::isFlowGoto() const
{
    return (this->_type == finExecFlowControl::TP_GOTO);
}

QString finExecFlowControl::getGotoLabel() const
{
    if ( this->_type == finExecFlowControl::TP_GOTO )
        return this->_label;
    else
        return QString();
}

finExecVariable *finExecFlowControl::getReturnVariable()
{
    return this->_retVar;
}

finExecVariable *finExecFlowControl::pickReturnVariable()
{
    finExecVariable *retvar = this->_retVar;
    this->_retVar = NULL;
    return retvar;
}

bool finExecFlowControl::isFlowExit() const
{
    return this->_type == finExecFlowControl::TP_EXIT;
}

bool finExecFlowControl::isFlowExpressOk() const
{
    return (this->_type == finExecFlowControl::TP_NEXT ||
            this->_type == finExecFlowControl::TP_EXIT);
}
bool finExecFlowControl::isFlowProgramOk() const
{
    return (this->_type == finExecFlowControl::TP_NEXT ||
            this->_type == finExecFlowControl::TP_RETURN ||
            this->_type == finExecFlowControl::TP_EXIT);
}

finErrorCode
finExecFlowControl::checkFlowForExpress(bool *goon, finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::TP_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        this->directPass();
        return finErrorKits::EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::TP_EXIT ) {
        if ( goon != NULL )
            *goon = false;
        this->directPass();
        return finErrorKits::EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorKits::EC_READ_ERROR;
    }
}

finErrorCode
finExecFlowControl::checkFlowForStatement(bool *goon, finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::TP_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        this->directPass();
        return finErrorKits::EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::TP_EXIT ||
                this->_type == finExecFlowControl::TP_RETURN ||
                this->_type == finExecFlowControl::TP_GOTO ||
                this->_type == finExecFlowControl::TP_BREAK ||
                this->_type == finExecFlowControl::TP_CONTINUE ) {
        if ( goon != NULL )
            *goon = false;
        this->directPass();
        return finErrorKits::EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorKits::EC_READ_ERROR;
    }
}

finErrorCode
finExecFlowControl::checkFlowForProgram(bool *goon, finLexNode *lexnode, finExecMachine *machine)
{
    if ( this->_type == finExecFlowControl::TP_NEXT ) {
        if ( goon != NULL )
            *goon = true;
        this->directPass();
        return finErrorKits::EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::TP_RETURN ) {
        if ( goon != NULL )
            *goon = false;
        this->setFlowNext();
        return finErrorKits::EC_SUCCESS;
    } else if ( this->_type == finExecFlowControl::TP_EXIT ) {
        if ( goon != NULL )
            *goon = false;
        this->directPass();
        return finErrorKits::EC_NORMAL_WARN;
    } else {
        if ( goon != NULL )
            *goon = false;
        if ( lexnode != NULL && machine != NULL )
            machine->appendExecutionError(lexnode, QString("Encounter unhandlable flow control."));

        this->releaseReturnVariable();
        return finErrorKits::EC_READ_ERROR;
    }
}

finErrorCode finExecFlowControl::setType(finExecFlowControlType type)
{
    this->_type = type;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::setLabel(const QString &label)
{
    this->_label = label;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::setFlowNext()
{
    this->_type = finExecFlowControl::TP_NEXT;
    this->_label = QString();
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::setGotoAndLabel(const QString &label)
{
    this->_type = finExecFlowControl::TP_GOTO;
    this->_label = label;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::setReturnVariable(finExecVariable *retvar)
{
    if ( this->_retVar != NULL )
        return finErrorKits::EC_STATE_ERROR;

    this->_retVar = retvar;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::retVarSwitchEnv(finExecEnvironment *subenv)
{
    this->_retVar = finExecVariable::buildFuncReturnVariable(this->_retVar, subenv);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecFlowControl::buildLinkedLeftVar()
{
    this->_retVar = finExecVariable::buildLinkLeftVariable(this->_retVar);
    return finErrorKits::EC_SUCCESS;
}

void finExecFlowControl::releaseReturnVariable()
{
    finExecVariable::releaseNonLeftVariable(this->_retVar);
    this->_retVar = NULL;
}


finErrorCode finExecFlowControl::directPass()
{
    return finErrorKits::EC_SUCCESS;
}

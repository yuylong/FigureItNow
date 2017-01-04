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

    this->_syntaxRdr.stopRead();
    this->_syntaxRdr.disposeAllRead();
    this->disposeExecutionError();
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

int finExecMachine::getExecuteErrorCount() const
{
    return this->_errList.count();
}

const finSyntaxError *finExecMachine::getExecuteErrorAt(int idx) const
{
    return this->_errList.at(idx);
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

    finFlowControl flowctl;
    flowctl._type = finExecMachine::FIN_FC_NEXT;
    flowctl._label = QString();

    finExecVariable *retvar = NULL;
    errcode = this->instantExecute(syntree->getRootNode(), this->_baseEnv, &retvar, &flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    if ( retvar != NULL )
        delete retvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finExecMachine::disposeExecutionError()
{
    while ( !this->_errList.empty() ) {
        finSyntaxError *synerr = this->_errList.first();
        this->_errList.removeFirst();
        delete synerr;
    }
}

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finFlowControl *flowctl)
{
    if ( synnode == NULL || env == NULL || retvar == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finSyntaxNodeType syntype = synnode->getType();
    switch ( syntype ) {
      case finSyntaxNode::FIN_SN_TYPE_SINGLE:
        return this->instExecSingle(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_DECLARE:
        return this->instExecDeclare(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_STATEMENT:
        return this->instExecStatement(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_EXPRESS:
        return this->instExecExpress(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_FUNCTION:
        return this->instExecFunction(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_BRANCH:
        return this->instExecBranch(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_LOOP:
        return this->instExecLoop(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_LABEL:
        return this->instExecLabel(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_JUMP:
        return this->instExecJump(synnode, env, retvar, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_PROGRAM:
        return this->instExecProgram(synnode, env, retvar, flowctl);
        break;

      default:
        this->appendExecutionError(synnode->getCommandLexNode(), QString("Unimplemented syntax type."));
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
    }

    this->appendExecutionError(synnode->getCommandLexNode(), QString("Reach unreachable path."));
    return finErrorCodeKits::FIN_EC_UNKNOWN_ERROR;
}

finErrorCode
finExecMachine::appendExecutionError(finLexNode *lexnode, const QString &errinfo)
{
    return finSyntaxError::appendExecutionError(lexnode, &this->_errList, errinfo);
}

finErrorCode
finExecMachine::instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finFlowControl *flowctl)
{
    finExecVariable *tmpretvar;
    finLexNode *lexnode = synnode->getCommandLexNode();
    finLexNodeType lextype = lexnode->getType();

    if ( lextype == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        tmpretvar = env->findVariable(lexnode->getString());
        if ( tmpretvar == NULL ) {
            this->appendExecutionError(lexnode, QString("Cannot find variable."));
            return finErrorCodeKits::FIN_EC_NOT_FOUND;
        }
    } else if ( lextype == finLexNode::FIN_LN_TYPE_DECIMAL || lextype == finLexNode::FIN_LN_TYPE_STRING ) {
        tmpretvar = new finExecVariable();
        if ( tmpretvar == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        if ( lextype == finLexNode::FIN_LN_TYPE_DECIMAL ) {
            tmpretvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            tmpretvar->setNumericValue(lexnode->getFloatValue());
        } else /*if ( lexttype == finLexNode::FIN_LN_TYPE_STRING )*/ {
            tmpretvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
            tmpretvar->setStringValue(lexnode->getStringValue());
        }
        tmpretvar->clearLeftValue();
        tmpretvar->setWriteProtected();
    } else {
        this->appendExecutionError(lexnode, QString("Unrecognized symbol."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    *retvar = tmpretvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    finExecVariable *newvar = new finExecVariable();
    if ( newvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    newvar->setName(lexnode->getString());
    newvar->setType(finExecVariable::FIN_VR_TYPE_NULL);
    newvar->setLeftValue();
    newvar->clearWriteProtected();

    finErrorCode errcode;
    errcode = env->addVariable(newvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        if ( errcode == finErrorCodeKits::FIN_EC_CONTENTION )
            this->appendExecutionError(lexnode, QString("Variable has already existed."));
        else
            this->appendExecutionError(lexnode, QString("Environment reject the variable."));
        delete newvar;
        return errcode;
    }

    flowctl->_type = finExecMachine::FIN_FC_NEXT;
    flowctl->_label = QString();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 2 ) {
        this->appendExecutionError(lexnode, QString("Definition incompleted."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *varname_synnode = synnode->getSubSyntaxNode(0);
    finErrorCode errcode = this->instExecDeclareDirect(varname_synnode, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    finExecVariable *tmpretvar;
    errcode = this->instantExecute(synnode, env, &tmpretvar, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    tmpretvar->releaseNonLeftVariable(tmpretvar);
    // Directly pass the flow ctl.
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 1 ) {
        this->appendExecutionError(lexnode, QString("Declaration incompleted."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        errcode = this->instExecDeclareExpr(synnode->getSubSyntaxNode(i), env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        if ( flowctl->_type != finExecMachine::FIN_FC_NEXT )
            break;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        this->appendExecutionError(lexnode, QString("Declaration on invalid syntax symbol."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        return this->instExecDeclareDirect(synnode, env, flowctl);
    } else if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
                lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_LET ) {
        return this->instExecDeclareAssigned(synnode, env, flowctl);
    } else if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
                lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
        return this->instExecDeclareComma(synnode, env, flowctl);
    }

    this->appendExecutionError(lexnode, QString("Declaration with invalid expression."));
    // Directly pass the flow ctl.
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode
finExecMachine::instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() != 1 ) {
        this->appendExecutionError(lexnode, QString("Declaration with invalid code."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    errcode = this->instExecDeclareExpr(synnode->getSubSyntaxNode(0), env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    *retvar = NULL;
    // Directly pass the flow ctl.
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env,
                                  finExecVariable **retvar, finFlowControl *flowctl)
{
    finErrorCode errcode = finErrorCodeKits::FIN_EC_SUCCESS;
    finLexNode *lexnode = synnode->getCommandLexNode();
    finExecEnvironment *curenv = env;
    finExecVariable *tmpvar = NULL;

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT ) {
        env->buildChildEnvironment(&curenv);
    }

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        finExecVariable::releaseNonLeftVariable(tmpvar);

        errcode = this->instantExecute(synnode->getSubSyntaxNode(i), curenv, &tmpvar, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            finExecVariable::releaseNonLeftVariable(tmpvar);
            goto out;
        }

        // Handle 'goto' sub-statement, and other flow control cases.
        if ( flowctl->_type == finExecMachine::FIN_FC_GOTO ) {
            int labelidx = synnode->findLabelIdx(flowctl->_label);
            if ( labelidx >= 0 ) {
                flowctl->_type = finExecMachine::FIN_FC_NEXT;
                flowctl->_label = QString();
                i = labelidx;
                continue;
            }
        }
        if ( flowctl->_type != finExecMachine::FIN_FC_NEXT )
            break;
    }

    *retvar = new finExecVariable();
    if ( *retvar != NULL ) {
        (*retvar)->copyVariable(tmpvar);
        (*retvar)->setName(QString());
        (*retvar)->clearLeftValue();
        (*retvar)->setWriteProtected();
    }
    finExecVariable::releaseNonLeftVariable(tmpvar);

out:
    if ( curenv != env )
        delete curenv;
    return errcode;
}

finErrorCode
finExecMachine::instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl)
{


    flowctl->_type = finExecMachine::FIN_FC_NEXT;
    flowctl->_label = QString();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env,
                             finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env,
                              finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecJump(finSyntaxNode *synnode, finExecEnvironment *env,
                             finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

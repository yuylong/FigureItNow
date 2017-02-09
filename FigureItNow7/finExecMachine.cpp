/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 30th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecMachine.h"

#include "finExecVariable.h"
#include "finExecFunction.h"
#include "finExecEnvironment.h"
#include "finExecOperartorClac.h"

finExecMachine::finExecMachine()
{
    this->_name = QString();
    this->_baseEnv = NULL;
    this->_baseFigContainer = NULL;
    this->_synTree = NULL;
}

finExecMachine::finExecMachine(const QString &name)
{
    this->_name = name;
    this->_baseEnv = NULL;
    this->_baseFigContainer = NULL;
    this->_synTree = NULL;
}

finExecMachine::~finExecMachine()
{
    if ( this->_baseEnv != NULL )
        delete this->_baseEnv;

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
    return this->_compiler.getScriptCode();
}

QString finExecMachine::getCompiledScriptCode() const
{
    if ( this->_synTree == NULL )
        return QString();

    return this->_synTree->getScriptCode();
}

finSyntaxTree *finExecMachine::getSyntaxTree()
{
    return this->_synTree;
}

int finExecMachine::getExecuteErrorCount() const
{
    return this->_errList.count();
}

finSyntaxError finExecMachine::getExecuteErrorAt(int idx) const
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
    this->_compiler.setScriptCode(script);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finExecMachine::isCompiled() const
{
    return (this->_synTree != NULL);
}

finErrorCode finExecMachine::compile()
{
    if ( this->_synTree != NULL )
        delete this->_synTree;

    this->_synTree = this->_compiler.compile();

    if ( this->_synTree == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    if ( this->_synTree->getErrorCount() > 0 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::execute()
{
    if ( this->_baseEnv == NULL || this->_baseFigContainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;
    if ( this->_synTree == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->disposeExecutionError();

    finExecFlowControl flowctl;
    finErrorCode errcode = this->instantExecute(this->_synTree->getRootNode(), this->_baseEnv, &flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    flowctl.releaseReturnVariable();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finExecMachine::disposeExecutionError()
{
    this->_errList.clear();
}

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    if ( synnode == NULL || env == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finSyntaxNodeType syntype = synnode->getType();
    switch ( syntype ) {
      case finSyntaxNode::FIN_SN_TYPE_SINGLE:
        return this->instExecSingle(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_DECLARE:
        return this->instExecDeclare(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_STATEMENT:
        return this->instExecStatement(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_EXPRESS:
        return this->instExecExpress(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_FUNCTION:
        return this->instExecFunction(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_BRANCH:
        return this->instExecBranch(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_LOOP:
        return this->instExecLoop(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_LABEL:
        return this->instExecLabel(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_JUMP:
        return this->instExecJump(synnode, env, flowctl);
        break;

      case finSyntaxNode::FIN_SN_TYPE_PROGRAM:
        return this->instExecProgram(synnode, env, flowctl);
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
finExecMachine::instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    if ( synnode == NULL || env == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finLexNode *lexnode = synnode->getCommandLexNode();
    this->appendExecutionError(lexnode, QString("Unrecognized symbol."));

    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode
finExecMachine::instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( lexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(lexnode, QString("Variable name is not recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

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

    flowctl->setFlowNext();
    flowctl->setReturnVariable(newvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 2 ) {
        this->appendExecutionError(lexnode, QString("Definition incompleted."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *varname_synnode = synnode->getSubSyntaxNode(0);
    finLexNode *varname_lexnode = varname_synnode->getCommandLexNode();

    if ( varname_synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         varname_lexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(varname_lexnode, QString("Variable name is not recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( env->getVariableHere(varname_lexnode->getString()) != NULL ) {
        this->appendExecutionError(lexnode, QString("Variable has already existed."));
        return finErrorCodeKits::FIN_EC_CONTENTION;
    }

    errcode = this->instantExecute(synnode->getSubSyntaxNode(1), env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true;
    errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon)
        return errcode;

    finExecVariable *initvar = finExecVariable::buildCopyLeftVariable(flowctl->pickReturnVariable());
    if ( initvar == NULL ) {
        flowctl->resetFlowControl();
        return this->instExecDeclareDirect(varname_synnode, env, flowctl);
    }

    initvar->setName(varname_lexnode->getString());
    errcode = env->addVariable(initvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->appendExecutionError(lexnode, QString("Environment reject the variable."));
        delete initvar;
        return errcode;
    }
    flowctl->setFlowNext();
    flowctl->setReturnVariable(initvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 1 ) {
        this->appendExecutionError(lexnode, QString("Declaration incompleted."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        flowctl->resetFlowControl();

        errcode = this->instExecDeclareExpr(synnode->getSubSyntaxNode(i), env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        bool goon = true;
        errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
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
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode
finExecMachine::instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
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

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecStatIn(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    // For a statement without anything inside, just return to execute the next.
    flowctl->setFlowNext();

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        flowctl->resetFlowControl();

        errcode = this->instantExecute(synnode->getSubSyntaxNode(i), env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        // Handle 'goto' sub-statement, and other flow control cases.
        if ( flowctl->isFlowGoto() ) {
            int labelidx = synnode->findLabelIdx(flowctl->getGotoLabel());
            if ( labelidx >= 0 ) {
                flowctl->setFlowNext();
                i = labelidx - 1;
                continue;
            }
        }

        bool sttgoon;
        errcode = flowctl->checkFlowForStatement(&sttgoon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !sttgoon )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();
    finExecEnvironment *curenv = env;

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT ) {
        env->buildChildEnvironment(&curenv);
    }

    errcode = instExecStatIn(synnode, curenv, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    if ( curenv != env ) {
        flowctl->retVarSwitchEnv(curenv);
        delete curenv;
    }
    flowctl->directPass();
    return errcode;

err:
    if ( curenv != env )
        delete curenv;
    return errcode;
}

finErrorCode finExecMachine::instExecExprNull(finExecFlowControl *flowctl)
{
    finExecVariable *retvar = NULL;

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprVar(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finExecVariable *retvar;
    finLexNode *lexnode = synnode->getCommandLexNode();

    retvar = env->findVariable(lexnode->getString());
    if ( retvar == NULL ) {
        this->appendExecutionError(lexnode, QString("Cannot find variable."));
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprNum(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finLexNode *lexnode = synnode->getCommandLexNode();
    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    retvar->setNumericValue(lexnode->getFloatValue());
    retvar->clearLeftValue();
    retvar->setWriteProtected();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprStr(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    finExecVariable *retvar = new finExecVariable();;
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finLexNode *lexnode = synnode->getCommandLexNode();
    retvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
    retvar->setStringValue(lexnode->getStringValue());
    retvar->clearLeftValue();
    retvar->setWriteProtected();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprFunc(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() != 2 ) {
        this->appendExecutionError(lexnode, QString("Invalid function call."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *fnsynn = synnode->getSubSyntaxNode(0);
    finLexNode *fnlexn = fnsynn->getCommandLexNode();

    if ( fnsynn->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         fnlexn->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(fnlexn, QString("Invalid function name."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    QString funcname = fnlexn->getString();
    finExecFunction *func = env->findFunction(funcname);
    if ( func == NULL ) {
        this->appendExecutionError(fnlexn, QString("Function name not found."));
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }

    errcode = func->execFunction(synnode->getSubSyntaxNode(1), env, this, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->appendExecutionError(lexnode, QString("Execute function failed."));
        return errcode;
    }

    bool flowgoon = true;
    errcode = flowctl->checkFlowForExpress(&flowgoon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !flowgoon )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprOper(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode = finErrorCodeKits::FIN_EC_SUCCESS;
    finLexNode *lexnode = synnode->getCommandLexNode();
    QList<finExecVariable *> oprands;
    finExecVariable *retvar;

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        finExecVariable *oprand;
        flowctl->resetFlowControl();

        errcode = instantExecute(synnode->getSubSyntaxNode(i), env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto out;

        bool oprandgoon = true;
        errcode = flowctl->checkFlowForExpress(&oprandgoon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !oprandgoon )
            goto out;

        oprand = flowctl->pickReturnVariable();
        oprands.append(oprand);
    }

    errcode = finExecOperartorClac::execOpCalc(lexnode->getOperator(), &oprands, &retvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->appendExecutionError(lexnode, QString("Invalid expression."));
        goto out;
    }

    flowctl->resetFlowControl();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    errcode = finErrorCodeKits::FIN_EC_SUCCESS;
out:
    while ( !oprands.empty() ) {
        finExecVariable *var = oprands.first();
        oprands.removeFirst();
        finExecVariable::releaseNonLeftVariable(var);
    }
    return errcode;
}


finErrorCode
finExecMachine::instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();
    finLexNodeType lextype = finLexNode::FIN_LN_TYPE_DUMMY;
    if ( lexnode != NULL )
        lextype = lexnode->getType();

    if ( lextype == finLexNode::FIN_LN_TYPE_DUMMY ) {
        return this->instExecExprNull(flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        return this->instExecExprVar(synnode, env, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_DECIMAL ) {
        return this->instExecExprNum(synnode, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_STRING ) {
        return this->instExecExprStr(synnode, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_OPERATOR &&
                lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_FUNCTION ) {
        return this->instExecExprFunc(synnode, env, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_OPERATOR ) {
        return this->instExecExprOper(synnode, env, flowctl);
    }

    this->appendExecutionError(lexnode, QString("Invalid expression found."));
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode
finExecMachine::instExecFuncName(finSyntaxNode *synnode, finExecEnvironment *env, finExecFunction *func)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         lexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(lexnode, QString("Function name cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    QString funcname = lexnode->getString();
    if ( env->getFunctionHere(funcname) != NULL ) {
        this->appendExecutionError(lexnode, QString("Function is redefined."));
        return finErrorCodeKits::FIN_EC_CONTENTION;
    }

    func->setFunctionName(funcname);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecFuncArg(finSyntaxNode *synnode, finExecFunction *func)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         lexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(lexnode, QString("Function name cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    QString argname = lexnode->getString();
    if ( func->isParameterExist(argname) ) {
        this->appendExecutionError(lexnode, QString("Function is redefined."));
        return finErrorCodeKits::FIN_EC_CONTENTION;
    }

    func->appendParameterName(argname);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecFuncArgs(finSyntaxNode *synnode, finExecFunction *func)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR ||
         lexnode->getOperator() != finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        this->appendExecutionError(lexnode, QString("Function arguments cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( synnode->getSubListCount() <= 0 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

    finSyntaxNode *insynnode = synnode->getSubSyntaxNode(0);
    finLexNode *inlexnode = insynnode->getCommandLexNode();

    if ( insynnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        this->appendExecutionError(inlexnode, QString("Function arguments cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finErrorCode errcode;
    if ( inlexnode->getType() == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        errcode = this->instExecFuncArg(insynnode, func);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    } else if ( inlexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
                inlexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_COMMA ) {
        for ( int i = 0; i < insynnode->getSubListCount(); i++ ) {
            errcode = this->instExecFuncArg(insynnode->getSubSyntaxNode(i), func);
            if ( finErrorCodeKits::isErrorResult(errcode) )
                return errcode;
        }
    } else {
        this->appendExecutionError(inlexnode, QString("Function arguments cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecFuncBody(finSyntaxNode *synnode, finExecFunction *func)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_STATEMENT ) {
        this->appendExecutionError(lexnode, QString("Function body cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    func->setFunctionSyntaxNode(synnode);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 3 ) {
        this->appendExecutionError(lexnode, QString("Incomplete function definition."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *fnname_syn = synnode->getSubSyntaxNode(0);
    finSyntaxNode *fnarg_syn = synnode->getSubSyntaxNode(1);
    finSyntaxNode *fnbody_syn = synnode->getSubSyntaxNode(2);

    finExecFunction *newfunc = new finExecFunction();
    if ( newfunc == NULL )
        return finErrorCode::FIN_EC_OUT_OF_MEMORY;

    finErrorCode errcode;
    newfunc->setFunctionType(finExecFunction::FIN_FN_TYPE_USER);

    errcode = this->instExecFuncName(fnname_syn, env, newfunc);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = this->instExecFuncArgs(fnarg_syn, newfunc);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = this->instExecFuncBody(fnbody_syn, newfunc);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = env->addFunction(newfunc);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;

err:
    delete newfunc;
    return errcode;
}

finErrorCode finExecMachine::instExecBrCondVoid(bool *retblval, finExecFlowControl *flowctl)
{
    *retblval = false;
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finExecMachine::instExecBrCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                            bool *retblval, finExecFlowControl *flowctl)
{
    if ( synnode == NULL )
        return this->instExecBrCondVoid(retblval, flowctl);

    finLexNode *lexnode = synnode->getCommandLexNode();
    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        this->appendExecutionError(lexnode, QString("Cannot recognize the branch condition."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        if ( synnode->getSubListCount() < 1 )
            return this->instExecBrCondVoid(retblval, flowctl);

        synnode = synnode->getSubSyntaxNode(0);
        lexnode = synnode->getCommandLexNode();
    }

    finErrorCode errcode = this->instantExecute(synnode, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true;
    errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
        return errcode;

    *retblval = finExecOperartorClac::varLogicValue(flowctl->getReturnVariable());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();
    int bridx = 0;
    bool condok = false, goon = true;

    for ( bridx = 0; bridx + 1 < synnode->getSubListCount(); bridx += 2 ) {
        errcode = this->instExecBrCond(synnode->getSubSyntaxNode(bridx), env, &condok, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;

        if ( condok ) {
            bridx += 1;
            break;
        }
    }

    flowctl->resetFlowControl();
    if ( bridx < synnode->getSubListCount() ) {
        errcode = this->instantExecute(synnode->getSubSyntaxNode(bridx), env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForStatement(&goon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    } else {
        flowctl->setFlowNext();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::instExecLoopCondVoid(bool *retblval, finExecFlowControl *flowctl)
{
    *retblval = true;
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finExecMachine::instExecLoopCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                              bool *retblval, finExecFlowControl *flowctl)
{
    if ( synnode == NULL )
        return this->instExecLoopCondVoid(retblval, flowctl);

    finLexNode *lexnode = synnode->getCommandLexNode();
    if ( synnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        this->appendExecutionError(lexnode, QString("Cannot recognize the branch condition."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        if ( synnode->getSubListCount() < 1 )
            return this->instExecLoopCondVoid(retblval, flowctl);

        synnode = synnode->getSubSyntaxNode(0);
        lexnode = synnode->getCommandLexNode();
    }

    finErrorCode errcode = this->instantExecute(synnode, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true;
    errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
        return errcode;

    *retblval = finExecOperartorClac::varLogicValue(flowctl->getReturnVariable());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecLoopWhile(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();
    if ( synnode->getSubListCount() < 2 ) {
        this->appendExecutionError(lexnode, QString("Unrecognized while loop."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finErrorCode errcode;
    finSyntaxNode *whilecond = synnode->getSubSyntaxNode(0);
    finSyntaxNode *whilebody = synnode->getSubSyntaxNode(1);
    bool loopcondok = true, goon = true;

    while ( true ) {
        flowctl->resetFlowControl();
        errcode = this->instExecLoopCond(whilecond, env, &loopcondok, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;

        if ( !loopcondok )
            break;

        flowctl->resetFlowControl();
        errcode = this->instantExecute(whilebody, env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        if ( flowctl->getType() == finExecFlowControl::FIN_FC_CONTINUE ) {
            flowctl->setFlowNext();
            continue;
        } else if ( flowctl->getType() == finExecFlowControl::FIN_FC_BREAK ) {
            flowctl->setFlowNext();
            break;
        }

        errcode = flowctl->checkFlowForStatement(&goon, lexnode, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::instExecLoopForHead(finSyntaxNode *synnode,
                                                 finSyntaxNode **inithead, finSyntaxNode **condhead,
                                                 finSyntaxNode **stephead, finSyntaxNode **loopbody)
{
    if ( synnode == NULL || inithead == NULL || condhead == NULL || stephead == NULL || loopbody == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finLexNode *lexnode = synnode->getCommandLexNode();
    if ( synnode->getSubListCount() < 2 ) {
        this->appendExecutionError(lexnode, QString("Unrecognized for loop."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *headsn = synnode->getSubSyntaxNode(0);
    finLexNode *headlex = headsn->getCommandLexNode();
    if ( headsn->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS || headsn->getSubListCount() < 3 ) {
        this->appendExecutionError(headlex, QString("Unrecognized loop head."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    if ( headlex->getType() != finLexNode::FIN_LN_TYPE_OPERATOR ||
         headlex->getOperator() != finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        this->appendExecutionError(headlex, QString("Unrecognized loop head."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( headsn->getSubListCount() < 3 ) {
        this->appendExecutionError(headlex, QString("Unrecognized loop head."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    *inithead = headsn->getSubSyntaxNode(0);
    if ( !finSyntaxNode::isStatementLevelType((*inithead)->getType()) ) {
        this->appendExecutionError((*inithead)->getCommandLexNode(), QString("Unrecognized loop init head."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    *condhead = headsn->getSubSyntaxNode(1);
    finLexNode *condheadlex = (*condhead)->getCommandLexNode();
    if ( (*condhead)->getType() != finSyntaxNode::FIN_SN_TYPE_STATEMENT ||
         condheadlex->getType() != finLexNode::FIN_LN_TYPE_OPERATOR ||
         condheadlex->getOperator() != finLexNode::FIN_LN_OPTYPE_SPLIT ) {
        this->appendExecutionError(condheadlex, QString("Unrecognized loop condition head."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }
    if ( (*condhead)->getSubListCount() < 1 ) {
        *condhead = NULL;
    } else {
        *condhead = (*condhead)->getSubSyntaxNode(0);
        condheadlex = (*condhead)->getCommandLexNode();
        if ( !finSyntaxNode::isExpressLevelType((*condhead)->getType()) ) {
            this->appendExecutionError(condheadlex, QString("Unrecognized loop condition head."));
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        }
    }

    if ( headsn->getSubListCount() >= 3 )
        *stephead = headsn->getSubSyntaxNode(2);
    else
        *stephead = NULL;

    *loopbody = synnode->getSubSyntaxNode(1);
    if ( !finSyntaxNode::isStatementLevelType((*loopbody)->getType()) ) {
        this->appendExecutionError((*loopbody)->getCommandLexNode(), QString("Unrecognized loop body."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecLoopFor(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finSyntaxNode *inithead, *condhead, *stephead, *loopbody;
    finLexNode *lexnode = synnode->getCommandLexNode();
    bool goon = true, loopcondok = true;

    errcode = this->instExecLoopForHead(synnode, &inithead, &condhead, &stephead, &loopbody);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;
    finLexNode *condheadlex = (condhead == NULL ? lexnode : condhead->getCommandLexNode());

    if ( inithead == NULL || loopbody == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    errcode = this->instantExecute(inithead, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = flowctl->checkFlowForStatement(&goon, inithead->getCommandLexNode(), this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
        return errcode;

    while ( true ) {
        flowctl->resetFlowControl();
        errcode = this->instExecLoopCond(condhead, env, &loopcondok, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        errcode = flowctl->checkFlowForExpress(&goon, condheadlex, this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;

        if ( !loopcondok )
            break;

        flowctl->resetFlowControl();
        errcode = this->instantExecute(loopbody, env, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        if ( flowctl->getType() == finExecFlowControl::FIN_FC_CONTINUE ) {
            flowctl->setFlowNext();
            goto loopstep;
        } else if ( flowctl->getType() == finExecFlowControl::FIN_FC_BREAK ) {
            flowctl->setFlowNext();
            break;
        }

        errcode = flowctl->checkFlowForStatement(&goon, loopbody->getCommandLexNode(), this);
        if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
            return errcode;

      loopstep:
        if ( stephead != NULL ) {
            flowctl->resetFlowControl();
            errcode = this->instantExecute(stephead, env, flowctl);
            if ( finErrorCodeKits::isErrorResult(errcode) )
                return errcode;

            errcode = flowctl->checkFlowForStatement(&goon, stephead->getCommandLexNode(), this);
            if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
                return errcode;
        }
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();
    QString loophdstr = lexnode->getString();

    if ( QString::compare(loophdstr, QString("while")) == 0 ) {
        return this->instExecLoopWhile(synnode, env, flowctl);
    } else if ( QString::compare(loophdstr, QString("for")) == 0 ) {
        return this->instExecLoopFor(synnode, env, flowctl);
    } else {
        this->appendExecutionError(lexnode, QString("Encouter unsupported loop type."));
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }
}

finErrorCode
finExecMachine::instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    if ( synnode == NULL || env == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( lexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE ) {
        this->appendExecutionError(lexnode, QString("Cannot recognize the label name."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::instExecJumpGoto(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();
    if ( synnode->getSubListCount() < 1 ) {
        this->appendExecutionError(lexnode, QString("No jumping target is found."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    finSyntaxNode *lblsynnode = synnode->getSubSyntaxNode(0);
    finLexNode *lbllexnode = lblsynnode->getCommandLexNode();
    if ( lblsynnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ||
         lbllexnode->getType() != finLexNode::FIN_LN_TYPE_VARIABLE) {
        this->appendExecutionError(lbllexnode, QString("Jumping target cannot be recognized."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    flowctl->setGotoAndLabel(lbllexnode->getString());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::instExecJumpRetVoid(finExecFlowControl *flowctl)
{
    flowctl->setType(finExecFlowControl::FIN_FC_RETURN);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecJumpRetVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    errcode = this->instantExecute(synnode, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true;
    errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
         return errcode;

    flowctl->setType(finExecFlowControl::FIN_FC_RETURN);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecJumpRet(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 1 )
        return this->instExecJumpRetVoid(flowctl);

    finSyntaxNode *basesn = synnode->getSubSyntaxNode(0);
    finLexNode *baseln = basesn->getCommandLexNode();
    if ( basesn->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
        this->appendExecutionError(baseln, QString("Return value cannot be calculated."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    if ( baseln->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         baseln->getOperator() == finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
        if ( basesn->getSubListCount() < 1 )
            return this->instExecJumpRetVoid(flowctl);

        basesn = basesn->getSubSyntaxNode(0);
    }
    return this->instExecJumpRetVal(basesn, env, flowctl);

    this->appendExecutionError(lexnode, QString("ERROR: Script execution reaches a wrong path."));
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode finExecMachine::instExecJumpExitVoid(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    this->appendExecutionError(lexnode, QString("INFO: Exit requested with void value."));
    flowctl->setType(finExecFlowControl::FIN_FC_EXIT);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecJumpExitVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();

    errcode = this->instantExecute(synnode, env, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    bool goon = true;
    errcode = flowctl->checkFlowForExpress(&goon, lexnode, this);
    if ( finErrorCodeKits::isErrorResult(errcode) || !goon )
         return errcode;

    this->appendExecutionError(lexnode, QString("INFO: Exit requested with a value."));
    flowctl->setType(finExecFlowControl::FIN_FC_EXIT);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecJumpExit(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();

    if ( synnode->getSubListCount() < 1 ) {
        return this->instExecJumpExitVoid(synnode, flowctl);
    } else {
        finSyntaxNode *basesn = synnode->getSubSyntaxNode(0);
        finLexNode *baseln = basesn->getCommandLexNode();
        if ( basesn->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS ) {
            this->appendExecutionError(baseln, QString("Exit value cannot be calculated."));
            return finErrorCodeKits::FIN_EC_READ_ERROR;
        }

        if ( baseln->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
             baseln->getOperator() == finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT ) {
            if ( basesn->getSubListCount() < 1 )
                return this->instExecJumpExitVoid(basesn, flowctl);

            basesn = basesn->getSubSyntaxNode(0);
        }
        return this->instExecJumpExitVal(basesn, env, flowctl);
    }

    this->appendExecutionError(lexnode, QString("ERROR: Script execution reaches a wrong path."));
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode finExecMachine::instExecJumpConti(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    if ( synnode == NULL || flowctl == NULL)
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    flowctl->setType(finExecFlowControl::FIN_FC_CONTINUE);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::instExecJumpBreak(finSyntaxNode *synnode, finExecFlowControl *flowctl)
{
    if ( synnode == NULL || flowctl == NULL)
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    flowctl->setType(finExecFlowControl::FIN_FC_BREAK);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecJump(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finLexNode *lexnode = synnode->getCommandLexNode();
    QString jumpkw = lexnode->getString();

    if ( QString::compare(jumpkw, "goto") == 0 ) {
        return this->instExecJumpGoto(synnode, flowctl);
    } else if ( QString::compare(jumpkw, "return") == 0 ) {
        return this->instExecJumpRet(synnode, env, flowctl);
    } else if ( QString::compare(jumpkw, "exit") == 0 ) {
        return this->instExecJumpExit(synnode, env, flowctl);
    } else if ( QString::compare(jumpkw, "continue") == 0 ) {
        return this->instExecJumpConti(synnode, flowctl);
    } else if ( QString::compare(jumpkw, "break") == 0 ) {
        return this->instExecJumpBreak(synnode, flowctl);
    } else {
        this->appendExecutionError(lexnode, QString("The jump statement cannot be recognized."));
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }
}

finErrorCode
finExecMachine::instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    finExecEnvironment *curenv;
    env->buildChildEnvironment(&curenv);

    finErrorCode errcode = instExecStatIn(synnode, curenv, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete curenv;
        return errcode;
    }

    errcode = flowctl->checkFlowForProgram(NULL, synnode->getCommandLexNode(), this);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete curenv;
        return errcode;
    }

    flowctl->retVarSwitchEnv(curenv);
    delete curenv;
    return errcode;
}

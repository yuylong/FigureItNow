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

finErrorCode finExecMachine::releaseCompile()
{
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecMachine::execute()
{
    if ( this->_baseEnv == NULL || this->_baseFigContainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;
    if ( this->_synTree == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finExecFlowControl flowctl;
    flowctl.setFlowNext();

    this->disposeExecutionError();

    finExecVariable *retvar = NULL;
    finErrorCode errcode = this->instantExecute(this->_synTree->getRootNode(), this->_baseEnv, &retvar, &flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    if ( retvar != NULL )
        delete retvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finExecMachine::disposeExecutionError()
{
    this->_errList.clear();
}

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finExecFlowControl *flowctl)
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
                               finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Single!"); synnode->dump();
    if ( synnode == NULL || env == NULL || retvar == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finLexNode *lexnode = synnode->getCommandLexNode();

    this->appendExecutionError(lexnode, QString("Unrecognized symbol."));
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}

finErrorCode
finExecMachine::instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    printf("Declared Direct!"); synnode->dump();
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

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    printf("Declared Assigned!");synnode->dump();
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
    flowctl->directPass();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    printf("Declared Comma!");synnode->dump();
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

        if ( !flowctl->isFlowNext() )
            break;
    }
    flowctl->directPass();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl)
{
    printf("Declared Expr!");synnode->dump();
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
finExecMachine::instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Declare!");synnode->dump();
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
    flowctl->directPass();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecStatIn(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Statement Inner!");synnode->dump();
    finErrorCode errcode;
    finExecVariable *tmpvar = NULL;

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        finExecVariable::releaseNonLeftVariable(tmpvar);

        errcode = this->instantExecute(synnode->getSubSyntaxNode(i), env, &tmpvar, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            finExecVariable::releaseNonLeftVariable(tmpvar);
            return errcode;
        }

        // Handle 'goto' sub-statement, and other flow control cases.
        if ( flowctl->isFlowGoto() ) {
            int labelidx = synnode->findLabelIdx(flowctl->getGotoLabel());
            if ( labelidx >= 0 ) {
                flowctl->setFlowNext();
                i = labelidx - 1;
                continue;
            }
        }
        if ( !flowctl->isFlowNext() )
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

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env,
                                  finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Statement!");synnode->dump();
    finErrorCode errcode;
    finLexNode *lexnode = synnode->getCommandLexNode();
    finExecEnvironment *curenv = env;

    if ( lexnode->getType() == finLexNode::FIN_LN_TYPE_OPERATOR &&
         lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_L_FLW_BRCKT ) {
        env->buildChildEnvironment(&curenv);
    }

    errcode = instExecStatIn(synnode, env, retvar, flowctl);

    if ( curenv != env )
        delete curenv;
    return errcode;
}

finErrorCode
finExecMachine::instExecExprVar(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expr Var!");synnode->dump();
    finExecVariable *tmpretvar;
    finLexNode *lexnode = synnode->getCommandLexNode();

    tmpretvar = env->findVariable(lexnode->getString());
    if ( tmpretvar == NULL ) {
        this->appendExecutionError(lexnode, QString("Cannot find variable."));
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }

    *retvar = tmpretvar;
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprNum(finSyntaxNode *synnode, finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expr Num!");synnode->dump();
    finExecVariable *tmpretvar = new finExecVariable();
    if ( tmpretvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finLexNode *lexnode = synnode->getCommandLexNode();
    tmpretvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretvar->setNumericValue(lexnode->getFloatValue());
    tmpretvar->clearLeftValue();
    tmpretvar->setWriteProtected();

    *retvar = tmpretvar;
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprStr(finSyntaxNode *synnode, finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expr Str!");synnode->dump();
    finExecVariable *tmpretvar = new finExecVariable();;
    if ( tmpretvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finLexNode *lexnode = synnode->getCommandLexNode();
    tmpretvar->setType(finExecVariable::FIN_VR_TYPE_STRING);
    tmpretvar->setStringValue(lexnode->getStringValue());
    tmpretvar->clearLeftValue();
    tmpretvar->setWriteProtected();

    *retvar = tmpretvar;
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprFunc(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expr Func!");synnode->dump();
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

    errcode = func->execFunction(synnode->getSubSyntaxNode(1), env, this, retvar, flowctl);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->appendExecutionError(lexnode, QString("Execute function failed."));
        return errcode;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finExecMachine::instExecExprOper(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expr Operator!");synnode->dump();
    finErrorCode errcode = finErrorCodeKits::FIN_EC_SUCCESS;
    finLexNode *lexnode = synnode->getCommandLexNode();
    QList<finExecVariable *> oprands;

    for ( int i = 0; i < synnode->getSubListCount(); i++ ) {
        finExecVariable *oprand;

        errcode = instantExecute(synnode->getSubSyntaxNode(i), env, &oprand, flowctl);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto out;

        oprands.append(oprand);
    }

    errcode = finExecOperartorClac::execOpCalc(lexnode->getOperator(), &oprands, retvar);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        this->appendExecutionError(lexnode, QString("Invalid expression."));
        return errcode;
    }

out:
    while ( !oprands.empty() ) {
        finExecVariable *var = oprands.first();
        oprands.removeFirst();
        finExecVariable::releaseNonLeftVariable(var);
    }
    flowctl->setFlowNext();
    return errcode;
}


finErrorCode
finExecMachine::instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Expression!");synnode->dump();
    finLexNode *lexnode = synnode->getCommandLexNode();
    finLexNodeType lextype = lexnode->getType();

    if ( lextype == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        return this->instExecExprVar(synnode, env, retvar, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_DECIMAL ) {
        return this->instExecExprNum(synnode, retvar, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_STRING ) {
        return this->instExecExprStr(synnode, retvar, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_OPERATOR &&
                lexnode->getOperator() == finLexNode::FIN_LN_OPTYPE_FUNCTION ) {
        return this->instExecExprFunc(synnode, env, retvar, flowctl);
    } else if ( lextype == finLexNode::FIN_LN_TYPE_OPERATOR ) {
        return this->instExecExprOper(synnode, env, retvar, flowctl);
    }

    this->appendExecutionError(lexnode, QString("Invalid expression found."));
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Function!");synnode->dump();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Branch!");synnode->dump();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env,
                             finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Loop!");synnode->dump();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env,
                              finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Label!");synnode->dump();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecJump(finSyntaxNode *synnode, finExecEnvironment *env,
                             finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Jump!");synnode->dump();
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finExecMachine::instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finExecFlowControl *flowctl)
{
    printf("Program!");synnode->dump();
    finExecEnvironment *curenv;
    env->buildChildEnvironment(&curenv);

    finErrorCode errcode = instExecStatIn(synnode, env, retvar, flowctl);
    delete curenv;
    return errcode;
}

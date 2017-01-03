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

    finExecVariable *retvar = NULL;
    errcode = finExecMachine::instantExecute(syntree->getRootNode(), this->_baseEnv, &retvar, &this->_errList);
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

typedef finErrorCode (*finExecCall)(finSyntaxNode *synnode, finExecEnvironment *env,
                                    finExecVariable **retvar, QList<finSyntaxError *> *errlist);

static finErrorCode instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env,
                                   finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env,
                                    finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env,
                                      finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env,
                                    finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env,
                                     finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env,
                                   finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env,
                                  finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecJump(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, QList<finSyntaxError *> *errlist);
static finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env,
                                    finExecVariable **retvar, QList<finSyntaxError *> *errlist);

struct {
    finSyntaxNodeType _type;
    finExecCall _func;
} finExecCallMap[] = {
    { finSyntaxNode::FIN_SN_TYPE_SINGLE,    instExecSingle    },
    { finSyntaxNode::FIN_SN_TYPE_DECLARE,   instExecDeclare   },
    { finSyntaxNode::FIN_SN_TYPE_STATEMENT, instExecStatement },
    { finSyntaxNode::FIN_SN_TYPE_EXPRESS,   instExecExpress   },
    { finSyntaxNode::FIN_SN_TYPE_FUNCTION,  instExecFunction  },
    { finSyntaxNode::FIN_SN_TYPE_BRANCH,    instExecBranch    },
    { finSyntaxNode::FIN_SN_TYPE_LOOP,      instExecLoop      },
    { finSyntaxNode::FIN_SN_TYPE_LABEL,     instExecLabel     },
    { finSyntaxNode::FIN_SN_TYPE_JUMP,      instExecJump      },
    { finSyntaxNode::FIN_SN_TYPE_PROGRAM,   instExecProgram   },
    { finSyntaxNode::FIN_SN_TYPE_MAX,       NULL              },
};

finErrorCode
finExecMachine::instantExecute(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    if ( synnode == NULL || env == NULL || retvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finSyntaxNodeType syntype = synnode->getType();
    for ( int i = 0; finExecCallMap[i]._type != finSyntaxNode::FIN_SN_TYPE_MAX; i++ ) {
        if ( finExecCallMap[i]._type != syntype )
            continue;

        if ( finExecCallMap[i]._func == NULL )
            return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;

        return finExecCallMap[i]._func(synnode, env, retvar, errlist);
    }
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env,
               finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    finExecVariable *tmpretvar;
    finLexNode *lexnode = synnode->getCommandLexNode();
    finLexNodeType lextype = lexnode->getType();

    if ( lextype == finLexNode::FIN_LN_TYPE_VARIABLE ) {
        tmpretvar = env->findVariable(lexnode->getString());
        if ( tmpretvar == NULL ) {
            finSyntaxError::appendExecutionError(lexnode, errlist, QString("Cannot find variable."));
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
        finSyntaxError::appendExecutionError(lexnode, errlist, QString("Unrecognized symbol."));
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    }

    *retvar = tmpretvar;
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env,
                finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env,
                  finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env,
                finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env,
                 finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env,
               finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env,
             finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env,
              finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecJump(finSyntaxNode *synnode, finExecEnvironment *env,
             finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env,
                finExecVariable **retvar, QList<finSyntaxError *> *errlist)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

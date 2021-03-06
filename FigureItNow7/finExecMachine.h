/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 30th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finExecMachine.h
 *  \brief The declarations of the runtime executor.
 *
 * This file declares the runtime executor. The execution procedure of script is taken here.
 */

#ifndef FINEXECMACHINE_H
#define FINEXECMACHINE_H

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecFlowControl.h"
#include "finExecCompiler.h"
#include "finFigureContainer.h"
#include "finLexNode.h"
#include "finSyntaxReader.h"
#include "finSyntaxError.h"
#include "finSyntaxErrorDump.h"
#include "finSyntaxErrorList.h"
#include "finSyntaxTree.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;

class finExecMachine
{
protected:
    QString _name;

    finExecEnvironment *_baseEnv;
    finFigureContainer *_baseFigContainer;

    finExecCompiler _compiler;
    finSyntaxTree *_synTree;
    finSyntaxErrorList _errList;

public:
    finExecMachine();
    finExecMachine(const QString &name);
    ~finExecMachine();

    QString getName() const;
    finExecEnvironment *getBaseEnvironment();
    finFigureContainer *getFigureContainer();
    QString getScriptCode() const;
    QString getCompiledScriptCode() const;
    finSyntaxTree *getSyntaxTree();
    finSyntaxErrorDump *getExecuteErrorDumper() const;
    int getExecuteErrorCount() const;
    finSyntaxError getExecuteErrorAt(int idx) const;

    finErrorCode setName(const QString &name);

    finErrorCode initEnvironment(finExecEnvironment *prevenv);
    finErrorCode initEnvironmentFromRoot();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setScriptCode(const QString &script);
    finErrorCode setExecuteErrorDumper(finSyntaxErrorDump *dumper);

    bool isCompiled() const;
    finErrorCode compile();
    finErrorCode execute();
    void disposeExecutionError();

    finErrorCode instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    void appendExecutionOutput(finSyntaxError::Level level, finLexNode *lexnode, const QString &errinfo);
    void appendExecutionError(finLexNode *lexnode, const QString &errinfo);
    void appendExecutionWarning(finLexNode *lexnode, const QString &errinfo);

private:
    finErrorCode instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJump(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    finErrorCode instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    finErrorCode instExecStatIn(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    finErrorCode instExecExprNull(finExecFlowControl *flowctl);
    finErrorCode instExecExprVar(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExprNum(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecExprStr(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecExprFunc(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExprOper(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    finErrorCode instExecFuncName(finSyntaxNode *synnode, finExecEnvironment *env, finExecFunction *func);
    finErrorCode instExecFuncArg(finSyntaxNode *synnode, finExecFunction *func);
    finErrorCode instExecFuncArgs(finSyntaxNode *synnode, finExecFunction *func);
    finErrorCode instExecFuncBody(finSyntaxNode *synnode, finExecFunction *func);

    finErrorCode instExecBrCondVoid(bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecBrCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                bool *retblval, finExecFlowControl *flowctl);

    finErrorCode instExecLoopCondVoid(bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecLoopCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                  bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecLoopWhile(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLoopForHead(finSyntaxNode *synnode,
                                     finSyntaxNode **inithead, finSyntaxNode **condhead,
                                     finSyntaxNode **stephead, finSyntaxNode **loopbody);
    finErrorCode instExecLoopFor(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    finErrorCode instExecJumpGoto(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpRetVoid(finExecFlowControl *flowctl);
    finErrorCode instExecJumpRetVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpRet(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExitVoid(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExitVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExit(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpConti(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpBreak(finSyntaxNode *synnode, finExecFlowControl *flowctl);
};

#endif // FINEXECMACHINE_H
